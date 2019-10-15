cmake_minimum_required(VERSION 3.1)

# options
option(CROSS_ARM "compile for arm" OFF)
option(IS_DOCKER "is using docker" OFF)

# cross
if(CROSS_ARM)
    message("compile for arm now!!!")
    if(IS_DOCKER)
        message("using docker build...")
        include(cmake/CrossARM.docker.cmake)
    else()
        include(cmake/CrossARM.cmake)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "-static -s")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/output/${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})

project(app)

add_executable(app main.cpp)

# docker
add_custom_target(docker_corss_build docker-compose up build)
add_custom_target(docker_corss_run   docker-compose up app)
add_custom_target(docker_corss_build_and_run docker-compose up build && docker-compose up app)
