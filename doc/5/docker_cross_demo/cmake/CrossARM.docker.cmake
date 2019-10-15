set(CMAKE_SYSTEM_NAME   Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_DIR       /opt/toolchain)
set(CMAKE_C_COMPILER    ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
