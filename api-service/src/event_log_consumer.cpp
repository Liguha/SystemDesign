#include "event_log_consumer.hpp"
#include "mongo_client.hpp"
#include <utility>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/urabbitmq/client.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/urabbitmq/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/engine/deadline.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/urabbitmq/typedefs.hpp>

using namespace std;

namespace components {
    using JsonValue = userver::formats::json::Value;
    using JsonBuilder = userver::formats::json::ValueBuilder;

    EventLogConsumer::EventLogConsumer(const userver::components::ComponentConfig& config,
                                       const userver::components::ComponentContext& context)
        : userver::urabbitmq::ConsumerComponentBase(config, context),
          mongo_pool_(context.FindComponent<userver::components::Mongo>("mongo-medical").GetPool()),
          exchange_name_(config["exchange_name"].As<string>("medical.events")) {
        try {
            auto& rabbit = context.FindComponent<userver::components::RabbitMQ>(config["rabbit_name"].As<string>("rabbitmq"));
            auto client = rabbit.GetClient();
            const string queue_name = config["queue"].As<string>("medical.events.log");
            const string routing_key = config["routing_key"].As<string>("event.#");

            auto deadline = userver::engine::Deadline::FromDuration(chrono::seconds(2));
            userver::urabbitmq::Exchange ex{exchange_name_};
            const userver::urabbitmq::Queue queue(queue_name);

            client->DeclareExchange(ex, userver::urabbitmq::Exchange::Type::kTopic, deadline);
            client->DeclareQueue(queue, deadline);
            client->BindQueue(ex, queue, routing_key, deadline);
        } catch (const exception& e) {
            LOG_ERROR() << "EventLogConsumer initialization failed: " << e.what();
            throw;
        }
    }

    void EventLogConsumer::Process(string message) {
        try {
            auto event_json = userver::formats::json::FromString(message);
            MongoClient mongo(mongo_pool_);
            mongo.InsertOne("event_logs", userver::formats::json::ToString(event_json));
            LOG_INFO() << "Event log saved: " << event_json["event_type"].As<string>();
        } catch (const exception& e) {
            LOG_ERROR() << "Failed to process event message: " << e.what();
            throw;
        }
    }

    userver::yaml_config::Schema EventLogConsumer::GetStaticConfigSchema() {
        return userver::yaml_config::MergeSchemas<userver::urabbitmq::ConsumerComponentBase>(R"(
            type: object
            description: EventLogConsumer component config
            additionalProperties: false
            properties:
                exchange_name:
                    type: string
                    description: RabbitMQ exchange name
                    defaultDescription: medical.events
                routing_key:
                    type: string
                    description: Routing key to bind
                    defaultDescription: event.#
        )");
    }

}  // namespace components