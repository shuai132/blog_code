#!/usr/bin/env bash

BUILD_DIR=cmake-build-arm-release-docker
mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
cmake -DCROSS_ARM=ON -DCMAKE_BUILD_TYPE=Release -DIS_DOCKER=ON ..
make app -j8
