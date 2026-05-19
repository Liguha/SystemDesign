#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>             
#include <userver/storages/mongo/component.hpp>          
#include <userver/urabbitmq/component.hpp>
#include <userver/utils/daemon_run.hpp>     
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>    

#include "event_publisher.hpp"
#include "event_log_consumer.hpp"
#include "handlers/auth_handler.hpp"
#include "handlers/user_handler.hpp"     
#include "handlers/patient_handler.hpp"              
#include "handlers/record_handler.hpp"
#include "handlers/event_handler.hpp"
#include "rate_limiter.hpp"
#include "cache.hpp"

ServiceCache g_cache;   
RateLimiter g_rate_limiter(100, 60);

int main(int argc, char* argv[]) {
    const auto component_list = userver::components::MinimalServerComponentList()
        .Append<userver::components::Secdist>()
        .Append<userver::components::DefaultSecdistProvider>()
        .Append<userver::clients::dns::Component>()
        .Append<userver::components::Mongo>("mongo-medical")
        .Append<userver::components::RabbitMQ>("rabbitmq")
        .Append<components::EventPublisher>("event-publisher")
        .Append<components::EventLogConsumer>("event-log-consumer")
        .Append<handlers::AuthHandler>()
        .Append<handlers::UserHandler>()
        .Append<handlers::PatientHandler>()
        .Append<handlers::RecordHandler>()
        .Append<handlers::EventHandler>();      

    return userver::utils::DaemonMain(argc, argv, component_list);       
}    