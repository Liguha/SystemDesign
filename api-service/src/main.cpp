#include <optional>
#include <string>
#include "handlers/patient_handler.hpp"
#include "handlers/auth_handler.hpp"
#include "handlers/record_handler.hpp"
#include "handlers/user_handler.hpp"
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/components/common_component_list.hpp>

using namespace std;    

int main(int argc, const char* const* argv) {
    auto component_list = userver::components::MinimalServerComponentList()
        .Append<handlers::AuthHandler>()
        .Append<handlers::UserHandler>()
        .Append<handlers::PatientHandler>()
        .Append<handlers::RecordHandler>();

    string config_path = "/app/config/config.yaml";    

    userver::components::Run(
        config_path,
        optional<string>{},     
        optional<string>{},  
        component_list);
    return 0;
}
