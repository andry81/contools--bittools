set(ENV_FILENAME "Environment.cmake")
if ("${ENV_ROOT}" STREQUAL "")
    if (EXISTS "${PROJECT_SOURCE_DIR}/${ENV_FILENAME}")
        message(STATUS "(*) Using local Env file")
        set(ENV_ROOT "${PROJECT_SOURCE_DIR}")
    elseif (DEFINED ENV{ENV_ROOT})
        message(STATUS "(*) Using Env path from env variable")
        set(ENV_ROOT "$ENV{ENV_ROOT}")
    endif()
endif()
if (NOT "${ENV_ROOT}" STREQUAL "")
    set(ENV_FILE "${ENV_ROOT}/${ENV_FILENAME}")
    get_filename_component(ENV_FILE "${ENV_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()
if (EXISTS "${ENV_FILE}")
    message(STATUS "(*) Environment description: ${ENV_FILE}")
    get_filename_component(ENV_ROOT "${ENV_FILE}" DIRECTORY)
    set(ENV_ROOT "${ENV_ROOT}" CACHE PATH "${ENV_FILENAME} directory")
    include("${ENV_FILE}")
else()
    if (NOT "${ENV_ROOT}" STREQUAL "") # some malformed user defined path
        message(FATAL_ERROR "Invalid ${ENV_FILENAME} directory: ${ENV_ROOT}")
    else()
        unset(ENV_ROOT CACHE) # just command to ignore any env and user separate vars
    endif()
endif()
