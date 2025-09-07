cmake_minimum_required(VERSION 3.10)
project(duw-collector VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Find dependencies with fallback strategies
find_package(PkgConfig QUIET)

# Find CURL
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CURL libcurl)
endif()

if(NOT CURL_FOUND)
    find_package(CURL REQUIRED)
    set(CURL_LIBRARIES ${CURL_LIBRARIES})
    set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
endif()

# Find SQLite3
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SQLITE3 sqlite3)
endif()

if(NOT SQLITE3_FOUND)
    find_package(SQLite3 REQUIRED)
    set(SQLITE3_LIBRARIES ${SQLite3_LIBRARIES})
    set(SQLITE3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIRS})
endif()

# Find nlohmann/json with fallback to FetchContent
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    message(STATUS "nlohmann/json not found locally, fetching from GitHub...")
    
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )
    
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Find spdlog with fallback to FetchContent
find_package(spdlog QUIET)

if(NOT spdlog_FOUND)
    include(FetchContent)
    message(STATUS "spdlog not found locally, fetching from GitHub...")
    
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_SHALLOW TRUE
    )
    
    FetchContent_MakeAvailable(spdlog)
endif()

# Add executable
add_executable(duw-collector
    src/app/main.cc
    src/core/collector.cc
    src/services/http_client.cc
    src/services/env_service.cc
    src/services/database_service.cc
    src/services/github_service.cc
    src/data/db_connection.cc
)

# Set target properties
set_target_properties(duw-collector PROPERTIES
    OUTPUT_NAME duw-collector
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Link libraries
target_link_libraries(duw-collector 
    PRIVATE 
    ${CURL_LIBRARIES} 
    ${SQLITE3_LIBRARIES} 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# Include directories
target_include_directories(duw-collector 
    PRIVATE 
    ${CURL_INCLUDE_DIRS} 
    ${SQLITE3_INCLUDE_DIRS}
    src
)

# Compile definitions and options
target_compile_options(duw-collector 
    PRIVATE 
    ${CURL_CFLAGS_OTHER} 
    ${SQLITE3_CFLAGS_OTHER}
)

# Enable compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Print configuration summary
message(STATUS "=== DUW Collector Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CURL found: ${CURL_FOUND} (${CURL_VERSION})")
message(STATUS "SQLite3 found: ${SQLITE3_FOUND} (${SQLITE3_VERSION})")
message(STATUS "nlohmann/json found: ${nlohmann_json_FOUND}")
message(STATUS "spdlog found: ${spdlog_FOUND}")
message(STATUS "==========================================")