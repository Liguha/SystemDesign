#pragma once
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_context.hpp>
#include <string>

namespace handlers {
    class RecordHandler final : public userver::server::handlers::HttpHandlerBase {        
    public:     
        static constexpr std::string_view kName = "record-handler";       

        RecordHandler(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

        std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext&) const override;
    };
}  // namespace handlers    
          