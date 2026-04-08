#include <cctype>   
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>  
#include <algorithm>
#include "patient_handler.hpp"
#include "db_utils.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>

using namespace std;      

namespace handlers {    
    using StatusCode = userver::http::StatusCode;  
    using HttpMethod = userver::server::http::HttpMethod;
    using HttpHandlerBase = userver::server::handlers::HttpHandlerBase;
    using HttpRequest = userver::server::http::HttpRequest;       
    using JsonBuilder = userver::formats::json::ValueBuilder;

    PatientHandler::PatientHandler(const userver::components::ComponentConfig& config,  
                                const userver::components::ComponentContext& context)
        : HttpHandlerBase(config, context) {}

    static string ToLower(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    string PatientHandler::HandleRequestThrow(
        const HttpRequest& request,
        userver::server::request::RequestContext&) const {
        
        const auto& path = request.GetRequestPath();        
        
        auto check_authentication = [&](const HttpRequest& req) {
            const auto& auth_header = req.GetHeader("authorization");
            return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
        };

        if (request.GetMethod() == HttpMethod::kPost && path == "/patients") {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }
            auto json = userver::formats::json::FromString(request.RequestBody());
            string first_name = json["first_name"].As<string>();
            string last_name = json["last_name"].As<string>();  
            string patronymic = json["patronymic"].As<string>("");  
            string birth_date = json["birth_date"].As<string>();    
            string phone = json["phone"].As<string>("");
            string id = "patient_" + to_string(time(nullptr));
            try {   
                PgClient pg;
                const char* values[6] = {id.c_str(), first_name.c_str(), last_name.c_str(), patronymic.c_str(), birth_date.c_str(), phone.c_str()};
                auto result = pg.ExecParams(
                    "INSERT INTO patients (id, first_name, last_name, patronymic, birth_date, phone, registered_at) "
                    "VALUES ($1, $2, $3, $4, $5, $6, NOW())",
                    6, values);  
                if (!result || PQresultStatus(result.get()) != PGRES_COMMAND_OK) {
                    request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                    return R"({"error": "Database error"})";
                }
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }

            request.GetHttpResponse().SetStatus(StatusCode::kCreated);
            JsonBuilder builder;
            builder["id"] = id;
            builder["first_name"] = first_name;
            builder["last_name"] = last_name;   
            builder["patronymic"] = patronymic;  
            builder["birth_date"] = birth_date;
            builder["phone"] = phone;
            builder["registered_at"] = time(nullptr);
            return userver::formats::json::ToString(builder.ExtractValue());    
        }
        
        if (request.GetMethod() == HttpMethod::kGet && path.find("/patients/search") == 0) {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})"; 
            }
            auto fio = request.GetArg("fio");
            if (fio.empty()) {
                request.GetHttpResponse().SetStatus(StatusCode::kBadRequest);
                JsonBuilder error_builder;
                error_builder["error"] = "fio parameter required";          
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }

            try {
                PgClient pg;
                string pattern = "%" + fio + "%";
                const char* values[2] = {pattern.c_str(), pattern.c_str()};
                auto result = pg.ExecParams(
                    "SELECT id, first_name, last_name, patronymic, birth_date, phone, EXTRACT(EPOCH FROM registered_at)::BIGINT "
                    "FROM patients "
                    "WHERE CONCAT_WS(' ', first_name, last_name) ILIKE $1 OR CONCAT_WS(' ', last_name, first_name) ILIKE $2 "
                    "ORDER BY last_name, first_name LIMIT 50",
                    2, values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK) {
                    request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                    return R"({"error": "Database error"})";
                }

                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                JsonBuilder array(userver::formats::json::Type::kArray);
                int rows = PQntuples(result.get());
                for (int i = 0; i < rows; i++) {
                    JsonBuilder patient_obj;
                    patient_obj["id"] = PQgetvalue(result.get(), i, 0);
                    patient_obj["first_name"] = PQgetvalue(result.get(), i, 1);
                    patient_obj["last_name"] = PQgetvalue(result.get(), i, 2);
                    patient_obj["patronymic"] = PQgetvalue(result.get(), i, 3);       
                    patient_obj["birth_date"] = PQgetvalue(result.get(), i, 4);
                    patient_obj["phone"] = PQgetvalue(result.get(), i, 5);
                    patient_obj["registered_at"] = stoll(PQgetvalue(result.get(), i, 6));
                    array.PushBack(patient_obj.ExtractValue());
                }
                return userver::formats::json::ToString(array.ExtractValue());
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }
        }

        if (request.GetMethod() == HttpMethod::kGet && path.find("/patients/") == 0 && path.find("/history") != std::string::npos) {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }

            std::string patient_id = path.substr(std::string("/patients/").size());
            auto pos = patient_id.find("/history");
            if (pos != std::string::npos) {
                patient_id = patient_id.substr(0, pos);
            }

            try {
                PgClient pg;
                const char* values[1] = {patient_id.c_str()};
                auto check = pg.ExecParams("SELECT 1 FROM patients WHERE id = $1", 1, values);
                if (!check || PQresultStatus(check.get()) != PGRES_TUPLES_OK || PQntuples(check.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);
                    JsonBuilder error_builder;
                    error_builder["error"] = "Patient not found";
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }

                auto result = pg.ExecParams(
                    "SELECT code, patient_id, created_by, title, description, EXTRACT(EPOCH FROM created_at)::BIGINT "
                    "FROM medical_records WHERE patient_id = $1 ORDER BY created_at DESC",
                    1,
                    values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK) {
                    request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                    return R"({"error": "Database error"})";
                }

                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                JsonBuilder arr(userver::formats::json::Type::kArray);
                int rows = PQntuples(result.get());
                for (int i = 0; i < rows; i++) {
                    JsonBuilder record;
                    record["code"] = PQgetvalue(result.get(), i, 0);
                    record["patient_id"] = PQgetvalue(result.get(), i, 1);
                    record["created_by"] = PQgetvalue(result.get(), i, 2);
                    record["title"] = PQgetvalue(result.get(), i, 3);
                    record["description"] = PQgetvalue(result.get(), i, 4);
                    record["created_at"] = stoll(PQgetvalue(result.get(), i, 5));
                    arr.PushBack(record.ExtractValue());
                }
                return userver::formats::json::ToString(arr.ExtractValue());
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }
        }
        
        if (request.GetMethod() == HttpMethod::kGet && path.find("/patients/") == 0 && path.find("/history") == std::string::npos) {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }

            std::string patient_id = path.substr(10);
            try {
                PgClient pg;
                const char* values[1] = {patient_id.c_str()};
                auto result = pg.ExecParams(
                    "SELECT id, first_name, last_name, patronymic, birth_date, phone, EXTRACT(EPOCH FROM registered_at)::BIGINT "
                    "FROM patients WHERE id = $1",
                    1,
                    values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK || PQntuples(result.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);
                    JsonBuilder error_builder;
                    error_builder["error"] = "Patient not found";
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }

                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                JsonBuilder builder; 
                builder["id"] = PQgetvalue(result.get(), 0, 0);
                builder["first_name"] = PQgetvalue(result.get(), 0, 1);
                builder["last_name"] = PQgetvalue(result.get(), 0, 2);  
                builder["patronymic"] = PQgetvalue(result.get(), 0, 3);
                builder["birth_date"] = PQgetvalue(result.get(), 0, 4);
                builder["phone"] = PQgetvalue(result.get(), 0, 5);
                builder["registered_at"] = stoll(PQgetvalue(result.get(), 0, 6));
                return userver::formats::json::ToString(builder.ExtractValue());
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }
        }
        
        request.GetHttpResponse().SetStatus(StatusCode::kNotFound); 
        return R"({"error": "Not found"})";
    }
}  // namespace handlers    