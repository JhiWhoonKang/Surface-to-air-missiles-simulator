cmake_minimum_required(VERSION 3.10)
project(MFR)

# 실행 파일을 상위 bin 디렉토리에 저장
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../bin)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 환경 구분 변수 (기본값: Ubuntu 환경)
option(BUILD_FOR_PETALINUX "Build for Petalinux (ARM)" OFF)

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

    # debug
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
else()
    # Ubuntu(x86) 빌드 환경
    message(STATUS "Building for Ubuntu x86 environment")
    
    # 기본적으로 g++ 사용
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/MFR
    ${CMAKE_SOURCE_DIR}/info
    ${CMAKE_SOURCE_DIR}/Logger
    ${CMAKE_SOURCE_DIR}/Config
    ${CMAKE_SOURCE_DIR}/CommManager
    ${CMAKE_SOURCE_DIR}/StepMotorController
)

# Add source files
set(SOURCES
    main.cpp
    MFR/Mfr.cpp
    Logger/logger.cpp
    Config/MfrConfig.cpp
    CommManager/MfrLcCommManager.cpp
    CommManager/MfrSimCommManager.cpp
    StepMotorController/StepMotorController.cpp
)

# Add header files
set(HEADERS
    MFR/Mfr.h
    info/PacketProtocol.h
    Logger/logger.h
    Config/MfrConfig.h
    CommManager/IReceiver.h
    CommManager/MfrLcCommManager.h
    CommManager/MfrSimCommManager.h
    StepMotorController/StepMotorController.h
)

# Create the executable
add_executable(MFR ${SOURCES} ${HEADERS})

# Add threading support
find_package(Threads REQUIRED)
target_link_libraries(MFR Threads::Threads)