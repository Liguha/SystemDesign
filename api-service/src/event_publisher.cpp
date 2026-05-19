#include "event_publisher.hpp"
#include <chrono>
#include <random>
#include <sstream>
#include <cstring>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/urabbitmq/client.hpp>
#include <userver/urabbitmq/component.hpp>
#include <userver/engine/deadline.hpp>
#include <userver/urabbitmq/channel.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

using namespace std;

namespace components {
    using JsonBuilder = userver::formats::json::ValueBuilder;
    using JsonValue = userver::formats::json::Value;

    string GenerateEventId() {
        random_device rd;
        mt19937_64 gen(rd());
        uint64_t val = gen();
        ostringstream ss;
        ss << "evt_" << hex << val << "_" << dec << time(nullptr);
        return ss.str();
    }

    string GetCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        tm* tm_info = gmtime(&time_t);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm_info);

        string result(buf);
        result += '.';
        char ms_buf[4];
        snprintf(ms_buf, sizeof(ms_buf), "%03ld", ms.count());
        result += ms_buf;
        result += "Z";
        return result;
    }

    EventPublisher::EventPublisher(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& context)
        : userver::components::ComponentBase(config, context),
          exchange_name_(config["exchange_name"].As<string>("medical.events")),
          rabbitmq_client_(context.FindComponent<userver::components::RabbitMQ>(config["rabbit_name"].As<string>("rabbitmq")).GetClient()) {
    }

    void EventPublisher::PublishEvent(string_view event_type,
                                      const JsonValue& payload) {
        JsonBuilder event_builder;
        event_builder["event_type"] = event_type;
        event_builder["event_id"] = GenerateEventId();
        event_builder["timestamp"] = GetCurrentTimestamp();
        event_builder["source"] = "api-service";
        event_builder["version"] = "1.0";
        event_builder["payload"] = payload;

        auto event_json = event_builder.ExtractValue();
        string event_str = userver::formats::json::ToString(event_json);

        try {
            auto deadline = userver::engine::Deadline::FromDuration(chrono::milliseconds{200});
            userver::urabbitmq::Exchange ex(exchange_name_);
            rabbitmq_client_->DeclareExchange(ex, userver::urabbitmq::Exchange::Type::kTopic, deadline);
            auto channel = rabbitmq_client_->GetChannel(deadline);
            string routing_key = string("event.") + string(event_type);
            channel.Publish(ex, routing_key, event_str, deadline);
            
            LOG_INFO() << "Event published: " << event_type << " (id: " << event_json["event_id"].As<string>() << ")";
        } catch (const exception& e) {
            LOG_ERROR() << "Failed to publish event: " << e.what();
        }
    }

    userver::yaml_config::Schema EventPublisher::GetStaticConfigSchema() {
        return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
            type: object
            description: EventPublisher component config
            additionalProperties: false
            properties:
                exchange_name:
                    type: string
                    description: RabbitMQ exchange name
                    defaultDescription: medical.events
                rabbit_name:
                    type: string
                    description: RabbitMQ component name
                    defaultDescription: rabbitmq
        )");
    }
}  // namespace components