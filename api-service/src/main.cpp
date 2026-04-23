#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>             
#include <userver/storages/mongo/component.hpp>          
#include <userver/utils/daemon_run.hpp>         

#include "handlers/auth_handler.hpp"
#include "handlers/user_handler.hpp"     
#include "handlers/patient_handler.hpp"              
#include "handlers/record_handler.hpp"

int main(int argc, char* argv[]) {
    const auto component_list = userver::components::MinimalServerComponentList()
        .Append<userver::clients::dns::Component>()
        .Append<userver::components::Mongo>("mongo-medical")
        .Append<handlers::AuthHandler>()
        .Append<handlers::UserHandler>()
        .Append<handlers::PatientHandler>()
        .Append<handlers::RecordHandler>();      

    return userver::utils::DaemonMain(argc, argv, component_list);       
}    