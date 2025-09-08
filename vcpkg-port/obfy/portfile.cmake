vcpkg_cmake_configure(
    SOURCE_PATH ${CURRENT_PORT_DIR}/../..
    OPTIONS -DOBFY_BUILD_EXAMPLES=OFF -DOBFY_BUILD_TESTS=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/obfy)

vcpkg_install_copyright(FILE_LIST "${CURRENT_PORT_DIR}/../../LICENSE")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug" "${CURRENT_PACKAGES_DIR}/lib")

