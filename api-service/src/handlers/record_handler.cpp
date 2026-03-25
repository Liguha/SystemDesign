#include "record_handler.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>
#include <ctime>
#include <vector>

namespace handlers {

RecordHandler::RecordHandler(const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string RecordHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    const auto& path = request.GetRequestPath();
    
    auto check_authentication = [&](const userver::server::http::HttpRequest& req) {
        const auto& auth_header = req.GetHeader("authorization");
        return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
    };

    if (request.GetMethod() == userver::server::http::HttpMethod::kPost && path == "/records") {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        auto json = userver::formats::json::FromString(request.RequestBody());
        
        std::string patient_id = json["patient_id"].As<std::string>();
        
        // mock data - `Patient Service` should be called
        if (patient_id.empty() || patient_id.find("patient") == std::string::npos) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
            userver::formats::json::ValueBuilder error_builder;
            error_builder["error"] = "Patient not found";
            return userver::formats::json::ToString(error_builder.ExtractValue());
        }
        
        // mock data - `Medical Record Service` should be called
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kCreated);
        userver::formats::json::ValueBuilder builder;
        builder["code"] = "REC-" + std::to_string(std::time(nullptr) % 1000000) + 
                         "-" + std::to_string((std::time(nullptr) / 1000000) % 1000000);
        builder["patient_id"] = patient_id;
        builder["created_by"] = json["created_by"].As<std::string>();
        builder["title"] = json["title"].As<std::string>();
        builder["description"] = json["description"].As<std::string>("");
        builder["created_at"] = std::time(nullptr);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/records/") == 0) {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        std::string code = path.substr(9);
        
        // mock data - record exists
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder builder;
        builder["code"] = code;
        builder["patient_id"] = "id_patient_001";
        builder["created_by"] = "doctor_user";
        builder["title"] = "Doctor visit";
        builder["description"] = "Patient is in normal condition";
        builder["created_at"] = std::time(nullptr);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/patients/") == 0 && 
        path.find("/history") != std::string::npos) {
        
        size_t pos = path.find("/history");
        std::string patient_id = path.substr(9, pos - 9);
        
        // mock data - `Medical Record Service` should be called
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder array(userver::formats::json::Type::kArray);
        
        userver::formats::json::ValueBuilder record1;
        record1["code"] = "REC-001-001";
        record1["patient_id"] = patient_id;
        record1["created_by"] = "doctor_user";
        record1["title"] = "Initial checkup";
        record1["description"] = "Patient is healthy";
        record1["created_at"] = std::time(nullptr) - 86400;
        array.PushBack(record1.ExtractValue());
        
        userver::formats::json::ValueBuilder record2;
        record2["code"] = "REC-002-002";
        record2["patient_id"] = patient_id;
        record2["created_by"] = "doctor_user";
        record2["title"] = "Follow-up visit";
        record2["description"] = "Condition improved";
        record2["created_at"] = std::time(nullptr);
        array.PushBack(record2.ExtractValue());
        
        return userver::formats::json::ToString(array.ExtractValue());
    }
    
    request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
    return R"({"error": "Not found"})";
}

}  // namespace handlers
