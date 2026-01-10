cmake_minimum_required(VERSION 3.14)
project(LauncherSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BUILD_FOR_PETALINUX)
    # Petalinux(ARM) 빌드 환경
    # 컴파일러 경고 레벨 설정
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

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


# 소스 모음
set(SOURCES
    main.cpp
    LS.cpp
    config/ConfigParser.cpp
    comm_SIM/LSToSimCommManager.cpp
    comm_LC/serial/LCToLSCommManager.cpp
    comm_LC/udp/LCToLSCommUDPManager.cpp
    comm_LC/LCToLSCommFactory.cpp
    statusManager/LSStatusManager.cpp
    statusManager/motorManager/LSMotorManager.cpp
)

# 실행 파일 생성
add_executable(launcher ${SOURCES})

# include 경로 설정
target_include_directories(launcher PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/config
    ${PROJECT_SOURCE_DIR}/comm_SIM
    ${PROJECT_SOURCE_DIR}/comm_LC
    ${PROJECT_SOURCE_DIR}/comm_LC/udp
    ${PROJECT_SOURCE_DIR}/comm_LC/serial
    ${PROJECT_SOURCE_DIR}/statusManager
    ${PROJECT_SOURCE_DIR}/statusManager/motorManager
    ${PROJECT_SOURCE_DIR}/statusManager/motorManager/testMotor
)
