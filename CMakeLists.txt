cmake_minimum_required(VERSION 3.1)

project(blog_code)

# common
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall)

# all cpp files
file(GLOB_RECURSE CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/src/*/*.cpp)
# generate target for every cpp file
foreach(v ${CPP_SRC_LIST})
    # file relative path from src/
    string(REGEX MATCH "src/.*" relative_path ${v})
    # delete string "src/"
    string(REGEX REPLACE "src/" "" target_name ${relative_path})
    # rename '/' and '.' to '_'
    string(REGEX REPLACE "/|\\." "_" target_name ${target_name})

    add_executable(${target_name}_cpp ${v})
endforeach()

# lua target
file(GLOB_RECURSE LUA_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/src/*/*.lua)
foreach(v ${LUA_SRC_LIST})
    string(REGEX MATCH "src/.*" relative_path ${v})
    string(REGEX REPLACE "src/" "" target_name ${relative_path})
    string(REGEX REPLACE "/|\\." "_" target_name ${target_name})

    add_custom_target(${target_name}_lua lua ${v})
endforeach()
