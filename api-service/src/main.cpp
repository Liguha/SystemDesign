#include <userver/components/component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/components/common_component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <optional>

#include "handlers/auth_handler.hpp"
#include "handlers/user_handler.hpp"
#include "handlers/patient_handler.hpp"
#include "handlers/record_handler.hpp"

int main(int argc, const char* const* argv) {
    auto component_list = userver::components::MinimalServerComponentList()
        .Append<handlers::AuthHandler>()
        .Append<handlers::UserHandler>()
        .Append<handlers::PatientHandler>()
        .Append<handlers::RecordHandler>();

    std::string config_path = "/app/config/config.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    userver::components::Run(
        config_path,
        std::optional<std::string>{},
        std::optional<std::string>{},
        component_list);
    return 0;
}
