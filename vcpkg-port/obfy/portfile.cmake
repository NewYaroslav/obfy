vcpkg_cmake_configure(
    SOURCE_PATH ${CURRENT_PORT_DIR}/../..
    OPTIONS -DOBFY_BUILD_EXAMPLES=OFF -DOBFY_BUILD_TESTS=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_get_vars(cmake_vars_file)
include(${cmake_vars_file})

if(NOT VCPKG_TARGET_CMAKE_INSTALL_LIBDIR)
    set(VCPKG_TARGET_CMAKE_INSTALL_LIBDIR lib)
endif()

vcpkg_cmake_config_fixup(CONFIG_PATH "${VCPKG_TARGET_CMAKE_INSTALL_LIBDIR}/cmake/obfy")

vcpkg_install_copyright(FILE_LIST "${CURRENT_PORT_DIR}/../../LICENSE")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug" "${CURRENT_PACKAGES_DIR}/${VCPKG_TARGET_CMAKE_INSTALL_LIBDIR}")

