#include <vector>
#include <ctime>
#include <algorithm>
#include <string>
#include <string_view>
#include <cctype>
#include "user_handler.hpp"
#include "db_utils.hpp"
#include "../globals.hpp"
#include <userver/server/http/http_response.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/formats/json/value.hpp>

using namespace std;      

namespace handlers {
    using HttpRequest = userver::server::http::HttpRequest;
    using HttpHandlerBase = userver::server::handlers::HttpHandlerBase;
    using StatusCode = userver::http::StatusCode;
    using JsonBuilder = userver::formats::json::ValueBuilder;   
    using HttpMethod = userver::server::http::HttpMethod;

    UserHandler::UserHandler(const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& context)
        : HttpHandlerBase(config, context) {}

    static string ToLower(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    string UserHandler::HandleRequestThrow(  
        const HttpRequest& request,
        userver::server::request::RequestContext&) const {
        
        const auto& path = request.GetRequestPath();
        
        auto check_authentication = [&](const HttpRequest& req) {
            const auto& auth_header = req.GetHeader("authorization");
            return !auth_header.empty() && auth_header.rfind("Bearer ", 0) == 0;
        };
        
        if (request.GetMethod() == HttpMethod::kPost && path == "/users") {
            if (!check_authentication(request)) {
                request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                return R"({"error": "Unauthorized"})";        
            }

            string user_id = request.GetHeader("x-user-id");
            if (user_id.empty()) user_id = "anonymous";
            if (!g_rate_limiter.TryConsume("create_user:" + user_id)) {
                request.GetHttpResponse().SetStatus(StatusCode::kTooManyRequests);
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Limit"), "100");
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Remaining"), "0");
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Reset"), 
                    to_string(time(nullptr) + g_rate_limiter.GetRetryAfterSeconds("create_user:" + user_id)));
                request.GetHttpResponse().SetHeader(string_view("Retry-After"), 
                    to_string(g_rate_limiter.GetRetryAfterSeconds("create_user:" + user_id)));
                return R"({"error": "Too many requests"})";
            }
            
            auto json = userver::formats::json::FromString(request.RequestBody());
            string login = json["login"].As<string>();
            string password = json["password"].As<string>();
            string first_name = json["first_name"].As<string>();    
            string last_name = json["last_name"].As<string>();
            string role = json["role"].As<string>("doctor");
            string id = "user_" + login;
            try {
                PgClient pg;
                const char* values[6] = {id.c_str(), login.c_str(), password.c_str(), first_name.c_str(), last_name.c_str(), role.c_str()};
                auto result = pg.ExecParams(
                    "INSERT INTO users (id, login, password, first_name, last_name, role, created_at) "
                    "VALUES ($1, $2, $3, $4, $5, $6, NOW())",
                    6, values);
                if (!result || PQresultStatus(result.get()) != PGRES_COMMAND_OK) {
                    string error = result ? PQresultErrorMessage(result.get()) : pg.GetError();
                    if (error.find("duplicate key") != string::npos) {       
                        request.GetHttpResponse().SetStatus(StatusCode::kConflict);  
                        JsonBuilder error_builder;
                        error_builder["error"] = "User already exists";
                        return userver::formats::json::ToString(error_builder.ExtractValue());
                    }
                    request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                    JsonBuilder error_builder;
                    error_builder["error"] = "Database error";
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }
                g_cache.InvalidatePattern("users:search:*");
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);      
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }
            request.GetHttpResponse().SetStatus(StatusCode::kCreated);
            JsonBuilder builder;
            builder["id"] = id;
            builder["login"] = login;
            builder["first_name"] = first_name;  
            builder["last_name"] = last_name;
            builder["role"] = role;
            builder["created_at"] = time(nullptr);
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        if (request.GetMethod() == HttpMethod::kGet && path.find("/users/search") == 0) {
            auto mask = request.GetArg("mask");
            if (mask.empty()) {
                request.GetHttpResponse().SetStatus(StatusCode::kBadRequest);
                JsonBuilder error_builder;
                error_builder["error"] = "mask parameter required";
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }

            string cache_key = "users:search:" + mask;
            auto cached_result = g_cache.Get(cache_key);
            if (cached_result) {
                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                request.GetHttpResponse().SetHeader(string_view("X-Cache"), "HIT");
                return *cached_result;
            }

            try {
                PgClient pg;
                string pattern = "%" + mask + "%";
                const char* values[3] = {pattern.c_str(), pattern.c_str(), pattern.c_str()};
                auto result = pg.ExecParams(
                    "SELECT id, login, first_name, last_name, role FROM users "
                    "WHERE login ILIKE $1 OR first_name ILIKE $2 OR last_name ILIKE $3 "
                    "ORDER BY login LIMIT 50",
                    3,
                    values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK) {
                    request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                    return R"({"error": "Database error"})";
                }

                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                request.GetHttpResponse().SetHeader(string_view("X-Cache"), "MISS");
                JsonBuilder array(userver::formats::json::Type::kArray);
                int rows = PQntuples(result.get());
                for (int i = 0; i < rows; i++) {
                    JsonBuilder user_obj;
                    user_obj["id"] = PQgetvalue(result.get(), i, 0);
                    user_obj["login"] = PQgetvalue(result.get(), i, 1);  
                    user_obj["first_name"] = PQgetvalue(result.get(), i, 2);
                    user_obj["last_name"] = PQgetvalue(result.get(), i, 3);
                    user_obj["role"] = PQgetvalue(result.get(), i, 4);
                    array.PushBack(user_obj.ExtractValue());
                }
                string result_json = userver::formats::json::ToString(array.ExtractValue());
                g_cache.Set(cache_key, result_json, 600);
                return result_json;
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;
                error_builder["error"] = ex.what();
                return userver::formats::json::ToString(error_builder.ExtractValue());
            }
        }
        if (request.GetMethod() == HttpMethod::kGet && path.find("/users/") == 0) {
            string login = path.substr(7);
            string cache_key = "user:login:" + login;
            auto cached_result = g_cache.Get(cache_key);
            if (cached_result) {
                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                request.GetHttpResponse().SetHeader(string_view("X-Cache"), "HIT");
                int remaining = g_rate_limiter.GetRemainingTokens("get_user:" + login);
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Limit"), "100");
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Remaining"), to_string(remaining));
                return *cached_result;
            }

            try {
                PgClient pg;
                const char* values[1] = {login.c_str()};
                auto result = pg.ExecParams(
                    "SELECT id, login, first_name, last_name, role, EXTRACT(EPOCH FROM created_at)::BIGINT "
                    "FROM users WHERE login = $1",
                    1, values);
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK || PQntuples(result.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kNotFound);     
                    JsonBuilder error_builder;
                    error_builder["error"] = "User not found";
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }
                request.GetHttpResponse().SetStatus(StatusCode::kOk);
                request.GetHttpResponse().SetHeader(string_view("X-Cache"), "MISS");
                JsonBuilder builder;
                builder["id"] = PQgetvalue(result.get(), 0, 0);
                builder["login"] = PQgetvalue(result.get(), 0, 1);
                builder["first_name"] = PQgetvalue(result.get(), 0, 2);
                builder["last_name"] = PQgetvalue(result.get(), 0, 3);
                builder["role"] = PQgetvalue(result.get(), 0, 4);   
                builder["created_at"] = stoll(PQgetvalue(result.get(), 0, 5));
                string result_json = userver::formats::json::ToString(builder.ExtractValue());
                g_cache.Set(cache_key, result_json, 600);
                int remaining = g_rate_limiter.GetRemainingTokens("get_user:" + login);
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Limit"), "100");
                request.GetHttpResponse().SetHeader(string_view("X-RateLimit-Remaining"), to_string(remaining));
                
                return result_json;
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
