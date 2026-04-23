#include "record_handler.hpp"
#include <vector>  
#include <cstdlib>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "db_utils.hpp"
#include "../mongo_client.hpp"
#include <userver/components/component_context.hpp> // Fixes "incomplete type"
#include <userver/storages/mongo/component.hpp>
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
        : HttpHandlerBase(config, context),
          mongo_pool_(context.FindComponent<userver::components::Mongo>("mongo-medical").GetPool()) {}

    static string GenerateRecordCode() {
        auto ts = to_string(time(nullptr));
        auto suffix = to_string(rand() % 1000000);
        return "REC-" + ts + "-" + suffix;
    }

    static string TimestampToISOString(time_t ts) {
        stringstream ss;
        auto tm = *gmtime(&ts);
        ss << put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }

    string RecordHandler::HandleRequestThrow(
        const HttpRequest& request,
        userver::server::request::RequestContext&) const {

        const auto& path = request.GetRequestPath();
        auto check_auth = [&](const HttpRequest& req) {
            const auto& auth_header = req.GetHeader("authorization");
            return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
        };

        // POST /records
        if (request.GetMethod() == HttpMethod::kPost && path == "/records") {
            if (!check_auth(request)) { 
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";
            }
            
            try {
                auto json = userver::formats::json::FromString(request.RequestBody());
                string patient_id = json["patient_id"].As<string>();
                string created_by = json["created_by"].As<string>();
                string title = json["title"].As<string>();  
                string record_code = GenerateRecordCode();
                
                PgClient pg;
                const char* check_vals[1] = {patient_id.c_str()};
                auto res = pg.ExecParams("SELECT 1 FROM patients WHERE id = $1", 1, check_vals);
                if (!res || PQntuples(res.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);   
                    return R"({"error": "Patient not found"})";
                }

                MongoClient mongo(mongo_pool_);
                time_t now = time(nullptr);
                string iso_time = TimestampToISOString(now);
                
                stringstream doc;
                doc << R"({)"
                    << R"("code": ")" << record_code << R"(",)"
                    << R"("patient_id": ")" << patient_id << R"(",)"
                    << R"("title": ")" << title << R"(",)"
                    << R"("created_at": {"$date": ")" << iso_time << R"("},)"
                    << R"("is_deleted": false)"
                    << R"(})";
                
                string oid = mongo.InsertOne("medical_records", doc.str());
                
                request.GetHttpResponse().SetStatus(StatusCode::kCreated);
                JsonBuilder builder;
                builder["code"] = record_code;
                builder["_id"] = oid;
                return userver::formats::json::ToString(builder.ExtractValue());
                
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                return string(R"({"error": ")") + ex.what() + R"("})";
            }
        }
        
        // GET /records/{code}
        if (request.GetMethod() == HttpMethod::kGet && path.find("/records/") == 0) {
            string code = path.substr(9);
            MongoClient mongo(mongo_pool_);
            string res = mongo.FindOne("medical_records", R"({"code": ")" + code + R"("})");
            if (res.empty()) {
                request.GetHttpResponse().SetStatus(StatusCode::kNotFound);
                return R"({"error": "Not found"})";
            }
            return res;
        }

        // GET /patients/{id}/history
        if (request.GetMethod() == HttpMethod::kGet && path.find("/history") != string::npos) {
            size_t pos = path.find("/patients/") + 10;
            size_t end_pos = path.find("/history");
            string p_id = path.substr(pos, end_pos - pos);

            MongoClient mongo(mongo_pool_);
            auto items = mongo.FindMany("medical_records", R"({"patient_id": ")" + p_id + R"("})");
            
            JsonBuilder array_builder;
            int idx = 0;
            for (const auto& item : items) {
                array_builder[idx++] = userver::formats::json::FromString(item);
            }
            return userver::formats::json::ToString(array_builder.ExtractValue());
        }

        request.GetHttpResponse().SetStatus(StatusCode::kNotFound);
        return R"({"error": "Not found"})";   
    }
}