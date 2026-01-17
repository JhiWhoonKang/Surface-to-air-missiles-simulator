#!/bin/bash

# 기본값: Ubuntu 빌드
BUILD_FOR_PETALINUX=0

# 옵션 파싱
while getopts "p" opt; do
    case "$opt" in
        p) BUILD_FOR_PETALINUX=1 ;; # -p 옵션이 주어지면 Petalinux 빌드 활성화
        *) echo "Usage: $0 [-p] (Use -p for Petalinux build)"; exit 1 ;;
    esac
done

# 빌드 환경 설정
if [ "$BUILD_FOR_PETALINUX" -eq 1 ]; then
    echo "Building for Petalinux (ARM)..."
    CMAKE_FLAGS="-DBUILD_FOR_PETALINUX=ON"
else
    echo "Building for Ubuntu (x86_64)..."
    CMAKE_FLAGS=""
fi

# 기존 build 디렉토리 삭제 후 새로 생성
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

echo "Creating build directory..."
mkdir build

# 프로토파일 생성
echo "Generating protobuf files..."
$GENERATE_SCRIPT
if [ $? -ne 0 ]; then
    echo "Error: Failed to generate protobuf files!"
    exit 1
fi

# 빌드 수행
echo "Starting build process..."
pushd build > /dev/null

cmake ../ $CMAKE_FLAGS
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed!"
    popd > /dev/null
    exit 1
fi

make -j 8 all
if [ $? -ne 0 ]; then
    echo "Error: Build failed!"
    popd > /dev/null
    exit 1
fi

popd > /dev/null
echo "Build process completed successfully!"
