cmake_minimum_required(VERSION 3.10)
project(SurfaceToAirWeaponSystem)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# 환경 구분 변수 (기본값: Ubuntu 환경)
option(BUILD_FOR_PETALINUX "Build for Petalinux (ARM)" OFF)

if(BUILD_FOR_PETALINUX)
    # Petalinux(ARM) 빌드 환경
    message(STATUS "Building for Petalinux ARM environment")

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm)

    set(CMAKE_C_COMPILER arm-xilinx-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER arm-xilinx-linux-gnueabi-g++)

    set(CMAKE_BUILD_TYPE Debug)  # 기본 빌드 타입을 Debug로 설정
    set(CMAKE_CXX_FLAGS_DEBUG "-g")  # -g 플래그로 디버깅 정보 포함
else()
    # Ubuntu(x86) 빌드 환경
    message(STATUS "Building for Ubuntu x86 environment")
    
    # 기본적으로 g++ 사용
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Simulator
    ${CMAKE_SOURCE_DIR}/Config
    ${CMAKE_SOURCE_DIR}/UDPCommunicate
    ${CMAKE_SOURCE_DIR}/Mock
    ${CMAKE_SOURCE_DIR}/Mock/info
)

# Add source files
set(SOURCES
    main.cpp
    Simulator/Simulator.cpp
    UDPCommunicate/LSRecvUDPManager.cpp
    UDPCommunicate/MFRSendUDPManager.cpp
    Mock/MockTarget.cpp
    Mock/MockTargetManager.cpp
    Mock/MockMissileManager.cpp
    Mock/MockMissile.cpp
    Config/Config.cpp
)

# Add header files
set(HEADERS
    Simulator/Simulator.h
    UDPCommunicate/LSRecvUDPManager.h
    UDPCommunicate/MFRSendUDPManager.h
    Mock/MockTargetManager.h
    Mock/MockTarget.h
    Mock/MockMissileManager.h
    Mock/info/MissileInfo.h
    Config/Config.h
)

# Create the executable
add_executable(SurfaceToAirWeaponSystem ${SOURCES} ${HEADERS})

# Add threading support
find_package(Threads REQUIRED)
target_link_libraries(SurfaceToAirWeaponSystem Threads::Threads)