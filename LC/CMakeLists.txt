cmake_minimum_required(VERSION 3.10)
project(LC_Project)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

set(CMAKE_CXX_STANDARD 17)

if(BUILD_FOR_PETALINUX)
    # Petalinux(ARM) 빌드 환경
    message(STATUS "Building for Petalinux ARM environment")

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm)

    set(CMAKE_C_COMPILER arm-xilinx-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER arm-xilinx-linux-gnueabi-g++)
else()
    # Ubuntu(x86) 빌드 환경
    message(STATUS "Building for Ubuntu x86 environment")
    
    # 기본적으로 g++ 사용
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Define include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/common
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/ecc
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/ls
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/mfr
    ${CMAKE_CURRENT_SOURCE_DIR}/status
    ${CMAKE_CURRENT_SOURCE_DIR}/inih
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# Define the executable and its source files
add_executable(lc
    main.cpp

    Config/LCConfig.cpp

    # Core components
    core/LCManager.cpp
    core/StatusLoader.cpp
    core/timeTrans.cpp
    # Communication modules
    comm/ecc/TcpECC.cpp
    comm/ls/SerialLS.cpp
    comm/mfr/TcpMFR.cpp
    core/LCCommandHandler.cpp

    # Common utilities
    comm/common/MessageParser.cpp
    comm/common/Serializer.cpp

    # External library
    inih/ini.c
)

# 스레드 라이브러리 연결
find_package(Threads REQUIRED)
target_link_libraries(lc Threads::Threads)