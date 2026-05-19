#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <userver/formats/json/value_builder.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/urabbitmq/client.hpp>
#include <userver/yaml_config/schema.hpp>

using namespace std;

namespace components {
    class EventPublisher final : public userver::components::ComponentBase {
    public:
        static constexpr string_view kName = "event-publisher";

        EventPublisher(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& context);

        ~EventPublisher() = default;

        static userver::yaml_config::Schema GetStaticConfigSchema();

        void PublishEvent(string_view event_type,
                         const userver::formats::json::Value& payload);

    private:
        string exchange_name_;
        shared_ptr<userver::urabbitmq::Client> rabbitmq_client_;
    };
}  // namespace components