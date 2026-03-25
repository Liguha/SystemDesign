#include "patient_handler.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cctype>

namespace handlers {

PatientHandler::PatientHandler(const userver::components::ComponentConfig& config,
                              const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string PatientHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    const auto& path = request.GetRequestPath();
    
    auto check_authentication = [&](const userver::server::http::HttpRequest& req) {
        const auto& auth_header = req.GetHeader("authorization");
        return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
    };

    if (request.GetMethod() == userver::server::http::HttpMethod::kPost && path == "/patients") {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        auto json = userver::formats::json::FromString(request.RequestBody());

        // mock data - `Patient Service` should be called
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kCreated);
        userver::formats::json::ValueBuilder builder;
        builder["id"] = "id_patient_" + std::to_string(std::time(nullptr) % 1000000);
        builder["first_name"] = json["first_name"].As<std::string>();
        builder["last_name"] = json["last_name"].As<std::string>();
        builder["patronymic"] = json["patronymic"].As<std::string>("");
        builder["birth_date"] = json["birth_date"].As<std::string>();
        builder["phone"] = json["phone"].As<std::string>("");
        builder["registered_at"] = std::time(nullptr);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/patients/search") == 0) {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }
        auto fio = request.GetArg("fio");
        if (fio.empty()) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kBadRequest);
            userver::formats::json::ValueBuilder error_builder;
            error_builder["error"] = "fio parameter required";
            return userver::formats::json::ToString(error_builder.ExtractValue());
        }
        
        // mockd data - patient search
        std::vector<std::vector<std::string>> mock_patients = {
            {"id_patient_001", "Peter", "Sidorov", "Ivanovich", "1980-05-15", "+1-999-123-45-67"}
        };
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder array(userver::formats::json::Type::kArray);

        for (const auto& patient : mock_patients) {
            std::string full_name = patient[1] + " " + patient[2];
            std::string lower_full_name = full_name;
            std::transform(lower_full_name.begin(), lower_full_name.end(), lower_full_name.begin(), ::tolower);
            std::string lower_fio = fio;
            std::transform(lower_fio.begin(), lower_fio.end(), lower_fio.begin(), ::tolower);
            bool match = false;

            if (lower_full_name.find(lower_fio) != std::string::npos) match = true;
            if (patient[1] == "Peter" && lower_fio == "petr") match = true;
            
            if (match) {
                userver::formats::json::ValueBuilder patient_obj;
                patient_obj["id"] = patient[0];
                patient_obj["first_name"] = patient[1];
                patient_obj["last_name"] = patient[2];
                patient_obj["patronymic"] = patient[3];
                patient_obj["birth_date"] = patient[4];
                patient_obj["phone"] = patient[5];
                patient_obj["registered_at"] = std::time(nullptr);
                array.PushBack(patient_obj.ExtractValue());
            }
        }
        return userver::formats::json::ToString(array.ExtractValue());
    }

    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/patients/") == 0 && path.find("/history") != std::string::npos) {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        std::string patient_id = path.substr(std::string("/patients/").size());
        auto pos = patient_id.find("/history");
        if (pos != std::string::npos) {
            patient_id = patient_id.substr(0, pos);
        }

        // redirect to record_handler by returning mock history
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder arr(userver::formats::json::Type::kArray);

        userver::formats::json::ValueBuilder record1;
        record1["code"] = "REC-001-001";
        record1["patient_id"] = patient_id;
        record1["created_by"] = "doctor_user";
        record1["title"] = "Initial checkup";
        record1["description"] = "Patient is healthy";
        record1["created_at"] = std::time(nullptr) - 86400;
        arr.PushBack(record1.ExtractValue());

        userver::formats::json::ValueBuilder record2;
        record2["code"] = "REC-002-002";
        record2["patient_id"] = patient_id;
        record2["created_by"] = "doctor_user";
        record2["title"] = "Follow-up visit";
        record2["description"] = "Condition improved";
        record2["created_at"] = std::time(nullptr);
        arr.PushBack(record2.ExtractValue());

        return userver::formats::json::ToString(arr.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/patients/") == 0 && path.find("/history") == std::string::npos) {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        std::string patient_id = path.substr(9);
        
        // mock data - is patient exists
        if (patient_id.find("patient") != std::string::npos || patient_id == "id_patient_001") {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
            userver::formats::json::ValueBuilder builder;
            builder["id"] = patient_id;
            builder["first_name"] = "Peter";
            builder["last_name"] = "Sidorov";
            builder["patronymic"] = "Ivanovich";
            builder["birth_date"] = "1980-05-15";
            builder["phone"] = "+1-999-123-45-67";
            builder["registered_at"] = std::time(nullptr);
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
        userver::formats::json::ValueBuilder error_builder;
        error_builder["error"] = "Patient not found";
        return userver::formats::json::ToString(error_builder.ExtractValue());
    }
    
    request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
    return R"({"error": "Not found"})";
}

}  // namespace handlers
