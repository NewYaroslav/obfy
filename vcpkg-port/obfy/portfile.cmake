vcpkg_cmake_configure(
    SOURCE_PATH ${CURRENT_PORT_DIR}/../..
    OPTIONS -DOBFY_BUILD_EXAMPLES=OFF -DOBFY_BUILD_TESTS=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/obfy)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

