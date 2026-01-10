cmake_minimum_required(VERSION 3.10)
project(RadarSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BUILD_FOR_PETALINUX)
    # Petalinux(ARM) 빌드 환경
    message(STATUS "Building for Petalinux ARM environment")

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm)

    set(CMAKE_C_COMPILER arm-xilinx-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER arm-xilinx-linux-gnueabi-g++)

    # DCMAKE BUILD_FOR_PETALINUX
    add_definitions(-DBUILD_FOR_PETALINUX)
else()
    # Ubuntu(x86) 빌드 환경
    message(STATUS "Building for Ubuntu x86 environment")
    
    # 기본적으로 g++ 사용
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_BUILD_TYPE Debug)  # 기본 빌드 타입을 Debug로 설정
    set(CMAKE_CXX_FLAGS_DEBUG "-g")  # -g 플래그로 디버깅 정보 포함
endif()

# 소스 파일들
set(SOURCES
    main.cpp
    Mfr.cpp
    MfrLcCommManager.cpp
    MfrSimCommManager.cpp
    StepMotorController.cpp
    config/MfrConfig.cpp
    Logger/logger.cpp
)

# 실행 파일 생성
add_executable(radar ${SOURCES})

# include 디렉토리 추가
target_include_directories(radar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/MFR.ini
    COPYONLY
)