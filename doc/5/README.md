# 使用Docker+CMake为ARM交叉编译C/C++

并不局限于ARM和C/C++，都可作为参考。

## 交叉编译(arm)

编译源码到arm可执行的二进制文件，可选方案有: 

* golang、rust等语言可以方便的支持交叉编译

* clang  
它本身是可交叉编译的架构。指定--target指定目标平台即可，但是资料较少，编译过程的头文件和库、最终的链接、c runtime估计还是会用GNU的，应该麻烦不少。没有尝试过也不过多讨论。

* 各种arm-linux-gcc  
这是本文讨论的方式，也是目前最稳定和使用最广的，推荐使用linaro系列交叉编译工具: [https://www.linaro.org/](https://www.linaro.org/)

## 使用Docker而不是虚拟机

配置交叉编译的环境比较繁琐：虚拟机、OS版本、交叉编译器种类和版本、功能验证等一系列问题要解决。

使用Docker可以配置好这些环境，保证环境的统一性，也方便编译环境复用。最终我们可以做到只需要一条命令，即可在任何有Docker环境的机器下编译出ARM(当然也可以是其他的)二进制文件。

* Docker的安装和使用: [https://blog.csdn.net/u012456479/article/details/102571737](https://blog.csdn.net/u012456479/article/details/102571737)

* CMake交叉编译: [https://blog.csdn.net/u012456479/article/details/102462241](https://blog.csdn.net/u012456479/article/details/102462241)

## Docker交叉编译(以arm-aarch64为例)

注: 直接看代码见最后链接。

推荐使用[https://hub.docker.com/u/rocproject](https://hub.docker.com/u/rocproject)组织的各种镜像，内部使用linaro系列交叉编译工具。

* 配置cmake支持交叉编译

新建cmake文件夹，并新建文件CrossARM.docker.cmake:

```cmake
set(CMAKE_SYSTEM_NAME   Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_DIR       /opt/toolchain)
set(CMAKE_C_COMPILER    ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
```

修改CMakeLists.txt，在project前如下设置

```cmake
# cross
if(CROSS_ARM)
    message(compile for arm now!!!)
    if(IS_DOCKER)
        message("using docker build...")
        include(cmake/CrossARM.docker.cmake)
    else()
        include(cmake/CrossARM.cmake)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -s")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/output/${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
```

* shell脚本 用于docker自动执行cmake

新建docker文件夹，并新建文件build.sh:

```bash
#!/usr/bin/env bash

BUILD_DIR=cmake-build-arm-release-docker
mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
cmake -DCROSS_ARM=ON -DCMAKE_BUILD_TYPE=Release -DIS_DOCKER=ON ..
make app -j8
```

* docker-compose.yml

其中，build服务用于构建，app服务用于模拟arm运行测试。

```yaml
version: "3"
services:
  build:
    container_name: docker_cross_demo_build
    image: rocproject/cross-aarch64-linux-gnu:gcc-7.4
    volumes:
      - .:/app
    working_dir: /app
    command: sh docker/build.sh
    tty: true

  app:
    container_name: docker_cross_demo_run
    image: arm64v8/busybox:1.31.0
    volumes:
      - ./output/arm-Release:/app
    working_dir: /app
    command: ./app
    tty: true
```

执行构建：

```bash
docker-compose up build
```

将使用`rocproject/cross-aarch64-linux-gnu:gcc-7.4`镜像生成容器执行构建，生成二进制目录在`output`。

模拟arm运行：

```bash
docker-compose up app
```

将使用`arm64v8/busybox:1.31.0`模拟(qemu)一个arm64设备执行。

## Links

* 完整的示例代码: [https://github.com/shuai132/blog_code/tree/master/doc/5/docker_cross_demo](https://github.com/shuai132/blog_code/tree/master/doc/5/docker_cross_demo)
