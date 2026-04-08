#include <string>
#include <ctime>
#include "db_utils.hpp"
#include "auth_handler.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json/value.hpp>  
#include <userver/server/http/http_response.hpp>
#include <userver/http/status_code.hpp>

using namespace std;
  
namespace handlers {
    using StatusCode = userver::http::StatusCode;
    using HttpMethod = userver::server::http::HttpMethod; 
    using HttpRequest = userver::server::http::HttpRequest;
    using JsonBuilder = userver::formats::json::ValueBuilder;  
    using HttpHandlerBase = userver::server::handlers::HttpHandlerBase;

    AuthHandler::AuthHandler(const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& context)  
        : HttpHandlerBase(config, context) {}

    string AuthHandler::HandleRequestThrow(
        const HttpRequest& request,
        userver::server::request::RequestContext&) const {  
        
        if (request.GetMethod() == HttpMethod::kPost) {
            auto json = userver::formats::json::FromString(request.RequestBody());
            string login = json["login"].As<string>(); 
            string password = json["password"].As<string>();
            try {  
                PgClient pg;
                const char* values[2] = {login.c_str(), password.c_str()};      
                auto result = pg.ExecParams("SELECT id, role FROM users WHERE login = $1 AND password = $2",2, values);   
                if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK || PQntuples(result.get()) != 1) {
                    request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
                    JsonBuilder error_builder;
                    error_builder["error"] = "Invalid credentials";
                    return userver::formats::json::ToString(error_builder.ExtractValue());
                }
                string user_id = PQgetvalue(result.get(), 0, 0); 
                string role = PQgetvalue(result.get(), 0, 1);  
                request.GetHttpResponse().SetStatus(StatusCode::kOk); 
                JsonBuilder builder;  
                builder["token"] = "token_" + user_id + "_" + to_string(time(nullptr));
                builder["user_id"] = user_id;
                builder["user_login"] = login; 
                builder["user_role"] = role;  
                builder["expires_at"] = time(nullptr) + 86400;
                return userver::formats::json::ToString(builder.ExtractValue());      
            } catch (const exception& ex) {
                request.GetHttpResponse().SetStatus(StatusCode::kInternalServerError);
                JsonBuilder error_builder;   
                error_builder["error"] = ex.what(); 
                return userver::formats::json::ToString(error_builder.ExtractValue());  
            }  
        }  
        if (request.GetMethod() == HttpMethod::kDelete) {
            request.GetHttpResponse().SetStatus(StatusCode::kOk);
            JsonBuilder builder;
            builder["success"] = true;  
            builder["message"] = "Logged out successfully";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        request.GetHttpResponse().SetStatus(StatusCode::kMethodNotAllowed);
        return R"({"error": "Method not allowed"})"; 
    }
}  // namespace handlers