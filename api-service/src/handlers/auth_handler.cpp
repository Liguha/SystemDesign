#include "auth_handler.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>
#include <ctime>

namespace handlers {

AuthHandler::AuthHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string AuthHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kPost) {
        auto json = userver::formats::json::FromString(request.RequestBody());
        std::string login = json["login"].As<std::string>();
        std::string password = json["password"].As<std::string>();
        
        // mock data - `Identity Service` should be called
        if (login == "doctor_user" && password == "doc123") {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
            userver::formats::json::ValueBuilder builder;
            builder["token"] = "token_mock_doctor_" + std::to_string(std::time(nullptr));
            builder["user_id"] = "id_doctor_001";
            builder["user_login"] = "doctor_user";
            builder["user_role"] = "doctor";
            builder["expires_at"] = std::time(nullptr) + 86400;
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        if (login == "admin_user" && password == "admin123") {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
            userver::formats::json::ValueBuilder builder;
            builder["token"] = "token_mock_admin_" + std::to_string(std::time(nullptr));
            builder["user_id"] = "id_admin_001";
            builder["user_login"] = "admin_user";
            builder["user_role"] = "admin";
            builder["expires_at"] = std::time(nullptr) + 86400;
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
        userver::formats::json::ValueBuilder error_builder;
        error_builder["error"] = "Invalid credentials";
        return userver::formats::json::ToString(error_builder.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kDelete) {
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder builder;
        builder["success"] = true;
        builder["message"] = "Logged out successfully";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    request.GetHttpResponse().SetStatus(userver::http::StatusCode::kMethodNotAllowed);
    return R"({"error": "Method not allowed"})";
}

}  // namespace handlers
