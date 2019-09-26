# CMake遍历目录下文件 为每一个cpp文件生成target

cmake基础用法请自行搜索

## Talk is cheap, show me the code.

很多时候，我们在写很多小例子的时候（比如博客测试代码、作业练习）。
我们常常要为每一个main.cpp写构建脚本，这显得有点low了。

假设我们想为类似`./src/*/*.cpp`目录结构的每一个cpp文件生成target，可以这么做：
```cmake
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
```

当然了，只适用于特殊场景，你可以添加更详细的规则。

## 题外话
诸如Python的pip，Java的Gradle、Maven，对包管理和构建提供了非常方便的支持。

C++包管理是一个痛点，这是由于平台、操作系统、硬件、abi、编译参数、各种多样性造成的，因为每一种排列组合编译出来的二进制文件都基本不相兼容。
连构建工具都乏善可陈。

CMake还是一个比较好的工具，毕竟是目前C++主流且使用最广泛的构建工具，没有之一！
