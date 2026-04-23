cmake_minimum_required(VERSION 3.14)
project(medical-api-gateway CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
  
find_package(userver REQUIRED COMPONENTS core mongo)
find_package(PostgreSQL REQUIRED)

add_executable(medical-api-gateway
    src/main.cpp
    src/db_utils.cpp
    src/mongo_client.cpp
    src/handlers/auth_handler.cpp
    src/handlers/user_handler.cpp
    src/handlers/patient_handler.cpp
    src/handlers/record_handler.cpp
)
 
target_link_libraries(medical-api-gateway
    userver::core
    userver::mongo
    ${PostgreSQL_LIBRARIES}
)

target_include_directories(medical-api-gateway PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${PostgreSQL_INCLUDE_DIRS}
)