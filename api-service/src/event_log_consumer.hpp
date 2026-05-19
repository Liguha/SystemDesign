#pragma once

#include <string>
#include <userver/components/component_context.hpp>
#include <userver/components/component_config.hpp>
#include <userver/urabbitmq/consumer_component_base.hpp>
#include <userver/storages/mongo/pool.hpp>
#include <userver/yaml_config/schema.hpp>

using namespace std;

namespace components {

class EventLogConsumer final : public userver::urabbitmq::ConsumerComponentBase {
public:
    static constexpr string_view kName = "event-log-consumer";

    EventLogConsumer(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);

    static userver::yaml_config::Schema GetStaticConfigSchema();

protected:
    void Process(string message) override;

private:
    userver::storages::mongo::PoolPtr mongo_pool_;
    string exchange_name_;
};

}  // namespace components