#include "event_handler.hpp"
#include "../mongo_client.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/http/status_code.hpp>
#include <userver/formats/json/value.hpp>

using namespace std;

namespace handlers {
    using HttpMethod = userver::server::http::HttpMethod;
    using HttpRequest = userver::server::http::HttpRequest;
    using StatusCode = userver::http::StatusCode;
    using JsonBuilder = userver::formats::json::ValueBuilder;

    EventHandler::EventHandler(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& context)
        : HttpHandlerBase(config, context),
          mongo_pool_(context.FindComponent<userver::components::Mongo>("mongo-medical").GetPool()) {}

    string EventHandler::HandleRequestThrow(const HttpRequest& request,
                                                 userver::server::request::RequestContext&) const {
        if (request.GetMethod() != HttpMethod::kGet) {
            request.GetHttpResponse().SetStatus(StatusCode::kMethodNotAllowed);
            return R"({"error": "Method not allowed"})";
        }

        const auto& auth_header = request.GetHeader("authorization");
        if (auth_header.empty() || auth_header.rfind("Bearer ", 0) != 0) {
            request.GetHttpResponse().SetStatus(StatusCode::kUnauthorized);
            return R"({"error": "Unauthorized"})";
        }

        auto event_type = request.GetArg("type");
        string filter;
        if (event_type.empty()) {
            filter = R"({})";
        } else {
            filter = string(R"({"event_type":")") + event_type + R"("})";
        }

        MongoClient mongo(mongo_pool_);
        auto records = mongo.FindMany("event_logs", filter);

        JsonBuilder array(userver::formats::json::Type::kArray);
        for (const auto& doc : records) {
            array.PushBack(userver::formats::json::FromString(doc));
        }

        request.GetHttpResponse().SetStatus(StatusCode::kOk);
        return userver::formats::json::ToString(array.ExtractValue());
    }
}