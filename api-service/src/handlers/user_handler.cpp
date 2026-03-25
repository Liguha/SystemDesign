#include "user_handler.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cctype>

namespace handlers {

UserHandler::UserHandler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string UserHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    const auto& path = request.GetRequestPath();
    
    auto check_authentication = [&](const userver::server::http::HttpRequest& req) {
        const auto& auth_header = req.GetHeader("authorization");
        return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
    };
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kPost && path == "/users") {
        if (!check_authentication(request)) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";}
        
        auto json = userver::formats::json::FromString(request.RequestBody());
        
        std::string login = json["login"].As<std::string>();
        
        // мокнутые данные - проверка на дубликат. В будущем будет вызов User Service
        if (login == "doctor_user" || login == "admin_user") {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kConflict);
            userver::formats::json::ValueBuilder error_builder;
            error_builder["error"] = "User already exists";
            return userver::formats::json::ToString(error_builder.ExtractValue());
        }
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kCreated);
        userver::formats::json::ValueBuilder builder;
        builder["id"] = "id_" + login;
        builder["login"] = login;
        builder["first_name"] = json["first_name"].As<std::string>();
        builder["last_name"] = json["last_name"].As<std::string>();
        builder["role"] = json["role"].As<std::string>("doctor");
        builder["created_at"] = std::time(nullptr);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/users/search") == 0) {
        auto mask = request.GetArg("mask");
        if (mask.empty()) {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kBadRequest);
            userver::formats::json::ValueBuilder error_builder;
            error_builder["error"] = "mask parameter required";
            return userver::formats::json::ToString(error_builder.ExtractValue());
        }
        
        // mock data for user search
        std::vector<std::tuple<std::string, std::string, std::string>> mock_users = {
            {"doctor_user", "Ivan", "Petrov"},
            {"admin_user", "Admin", "System"}
        };
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
        userver::formats::json::ValueBuilder array(userver::formats::json::Type::kArray);
        
        auto lower = [](const std::string &s) {
            std::string r = s;
            std::transform(r.begin(), r.end(), r.begin(), ::tolower);
            return r;
        };
        
        std::string lower_mask = lower(mask);
        
        for (const auto& user : mock_users) {
            const auto &login = std::get<0>(user);
            const auto &first_name = std::get<1>(user);
            const auto &last_name = std::get<2>(user);
            std::string full_name = first_name + " " + last_name;
            
            bool match = false;
            if (lower(login).find(lower_mask) != std::string::npos) match = true;
            if (lower(full_name).find(lower_mask) != std::string::npos) match = true;
            
            if (match) {
                userver::formats::json::ValueBuilder user_obj;
                user_obj["id"] = "id_" + login;
                user_obj["login"] = login;
                user_obj["first_name"] = first_name;
                user_obj["last_name"] = last_name;
                user_obj["role"] = (login == "doctor_user") ? "doctor" : "admin";
                array.PushBack(user_obj.ExtractValue());
            }
        }
        return userver::formats::json::ToString(array.ExtractValue());
    }
    
    if (request.GetMethod() == userver::server::http::HttpMethod::kGet && path.find("/users/") == 0) {
        std::string login = path.substr(7);
        
        // mock data - return user if exists
        if (login == "doctor_user" || login == "admin_user") {
            request.GetHttpResponse().SetStatus(userver::http::StatusCode::kOk);
            userver::formats::json::ValueBuilder builder;
            builder["id"] = "id_" + login;
            builder["login"] = login;
            builder["first_name"] = (login == "doctor_user") ? "Ivan" : "Admin";
            builder["last_name"] = (login == "doctor_user") ? "Petrov" : "System";
            builder["role"] = (login == "doctor_user") ? "doctor" : "admin";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
        userver::formats::json::ValueBuilder error_builder;
        error_builder["error"] = "User not found";
        return userver::formats::json::ToString(error_builder.ExtractValue());
    }
    
    request.GetHttpResponse().SetStatus(userver::http::StatusCode::kNotFound);
    return R"({"error": "Not found"})";
}

}  // namespace handlers
