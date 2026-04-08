#include <vector>  
#include <cstdlib>
#include <string>
#include <ctime>
#include "record_handler.hpp"
#include "db_utils.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>

using namespace std;

namespace handlers {      
    using HttpMethod = userver::server::http::HttpMethod;
    using HttpRequest = userver::server::http::HttpRequest;     
    using StatusCode = userver::http::StatusCode;
    using HttpHandlerBase = userver::server::handlers::HttpHandlerBase;
    using JsonBuilder = userver::formats::json::ValueBuilder;

    RecordHandler::RecordHandler(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& context)  
        : HttpHandlerBase(config, context) {}

    static string GenerateRecordCode() {
        auto ts = to_string(time(nullptr));
        auto suffix = to_string(rand() % 1000000);
        return "REC-" + ts + "-" + suffix;
    }

    string RecordHandler::HandleRequestThrow(
        const HttpRequest& request,
        userver::server::request::RequestContext&) const {

        const auto& path = request.GetRequestPath();
        auto check_authentication = [&](const HttpRequest& req) {
            const auto& auth_header = req.GetHeader("authorization");
            return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
        };
        if (request.GetMethod() == HttpMethod::kPost && path == "/records") {
            if (!check_authentication(request)) { 
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }
            auto json = userver::formats::json::FromString(request.RequestBody());
            string patient_id = json["patient_id"].As<string>();
            string created_by = json["created_by"].As<string>();
            string title = json["title"].As<string>();  
            string description = json["description"].As<string>("");
            std::string code = GenerateRecordCode();
            try {
                PgClient pg;
                const char* check_values[1] = {patient_id.c_str()};
                auto check = pg.ExecParams("SELECT 1 FROM patients WHERE id = $1", 1, check_values);
                if (!check || PQresultStatus(check.get()) != PGRES_TUPLES_OK || PQntuples(check.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);   
                    JsonBuilder error_builder;
                    error_builder["error"] = "Patient not found";   
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }

                const char* values[5] = {code.c_str(), patient_id.c_str(), created_by.c_str(), title.c_str(), description.c_str()};
                auto result = pg.ExecParams(
                    "INSERT INTO medical_records (code, patient_id, created_by, title, description, created_at) "
                    "VALUES ($1, $2, $3, $4, $5, NOW())",
                    5, values);   
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
            builder["code"] = code;
            builder["patient_id"] = patient_id;
            builder["created_by"] = created_by;
            builder["title"] = title;  
            builder["description"] = description;
            builder["created_at"] = time(nullptr);
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        if (request.GetMethod() == HttpMethod::kGet && path.find("/records/") == 0) {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }
            string code = path.substr(9);
            try {
                PgClient pg;
                const char* values[1] = {code.c_str()};
                auto result = pg.ExecParams(
                    "SELECT code, patient_id, created_by, title, description, EXTRACT(EPOCH FROM created_at)::BIGINT "
                    "FROM medical_records WHERE code = $1",
                    1,
                    values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK || PQntuples(result.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);
                    JsonBuilder error_builder;
                    error_builder["error"] = "Record not found"; 
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }
                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                JsonBuilder builder;
                builder["code"] = PQgetvalue(result.get(), 0, 0);
                builder["patient_id"] = PQgetvalue(result.get(), 0, 1);
                builder["created_by"] = PQgetvalue(result.get(), 0, 2);
                builder["title"] = PQgetvalue(result.get(), 0, 3);
                builder["description"] = PQgetvalue(result.get(), 0, 4);
                builder["created_at"] = stoll(PQgetvalue(result.get(), 0, 5));
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