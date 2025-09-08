#!/usr/bin/env bash
set -euo pipefail

CMAKE_FLAGS=(-DCMAKE_BUILD_TYPE=Release)

if [ -n "${CXX_STANDARD:-}" ]; then
  CMAKE_FLAGS+=("-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
fi

if [ -n "${VCPKG_ROOT:-}" ]; then
  INSTALL_OPTS=()
  if [ -n "${VCPKG_OVERLAY_PORTS:-}" ]; then
    INSTALL_OPTS+=("--overlay-ports=${VCPKG_OVERLAY_PORTS}")
  fi
  TRIPLET=x64-linux-dynamic
  "${VCPKG_ROOT}/vcpkg" install obfy boost-test --triplet ${TRIPLET} "${INSTALL_OPTS[@]}"
  CMAKE_FLAGS+=("-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=${TRIPLET}")
fi

cmake -S . -B build "${CMAKE_FLAGS[@]}"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure

