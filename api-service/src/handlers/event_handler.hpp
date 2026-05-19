#pragma once
#include <string>
#include <userver/storages/mongo/pool.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_context.hpp>

using namespace std;

namespace handlers {
    class EventHandler final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr string_view kName = "event-handler";

        EventHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);

        string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                       userver::server::request::RequestContext&) const override;

    private:
        userver::storages::mongo::PoolPtr mongo_pool_;
    };
}  // namespace handlers