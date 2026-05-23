# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_compile_options(-Wno-error)

# generate tiling_api.h softlink
if(NOT BUILD_OPEN_PROJECT)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/tiling/tiling_api.h
    COMMAND mkdir ${CMAKE_BINARY_DIR}/tiling
    COMMAND ln -s ${ASCENDC_INCLUDE_DIR}/adv_api/tiling_api.h
            ${CMAKE_BINARY_DIR}/tiling/tiling_api.h
    DEPENDS ${ASCENDC_INCLUDE_DIR}/adv_api/tiling_api.h)
  add_custom_target(gen_kernel_api ALL
                    DEPENDS ${CMAKE_BINARY_DIR}/tiling/tiling_api.h)
endif()

include(gen_kernel_tiling.cmake)

add_library(asc_host_headers INTERFACE)
target_include_directories(
  asc_host_headers
  INTERFACE $<BUILD_INTERFACE:${ASCENDC_DIR}/include/adv_api/hccl/internal>
)

add_library(asc_kernel_headers INTERFACE)
target_include_directories(
  asc_kernel_headers
  INTERFACE $<BUILD_INTERFACE:${ASCENDC_DIR}>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/include>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/include/simt_api>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/impl>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/basic_api>
            $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/simt_api>
)

add_library(kernel_tiling_headers INTERFACE)
add_dependencies(kernel_tiling_headers gen_kernel_tiling)
if(NOT BUILD_OPEN_PROJECT)
  add_dependencies(kernel_tiling_headers gen_kernel_api)
endif()
target_include_directories(
  kernel_tiling_headers
  INTERFACE $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/tikcpp>
            $<INSTALL_INTERFACE:include/tikcpp/tikcfw>
            $<INSTALL_INTERFACE:include/tikcpp/tikcfw/kernel_tiling>
            $<BUILD_INTERFACE:${KERNEL_TILING_DIR}>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

add_subdirectory(tiling)

if (BUILD_OPEN_PROJECT AND ENABLE_BUILD_DEVICE)
  include(ExternalProject)
  ExternalProject_Add(device_tiling
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/cmake/device
    BINARY_DIR ${CMAKE_BINARY_DIR}/device_build
    CONFIGURE_COMMAND ${CMAKE_COMMAND}
    -G ${CMAKE_GENERATOR}
    -DCANN_3RD_LIB_PATH=${CANN_3RD_LIB_PATH}
    -DBUILD_OPEN_PROJECT=${BUILD_OPEN_PROJECT}
    -DINSTALL_LIBRARY_DIR=${INSTALL_LIBRARY_DIR}/lib64/device
    -DASCEND_CANN_PACKAGE_PATH=${ASCEND_CANN_PACKAGE_PATH}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DSYS_VERSION=${SYS_VERSION}
    -DCMAKE_C_COMPILER=${ASCEND_CANN_PACKAGE_PATH}/toolkit/toolchain/hcc/bin/aarch64-target-linux-gnu-gcc
    -DCMAKE_CXX_COMPILER=${ASCEND_CANN_PACKAGE_PATH}/toolkit/toolchain/hcc/bin/aarch64-target-linux-gnu-g++
    -DPRODUCT_SIDE=device
    -DCUSTOM_SIGN_SCRIPT=${CUSTOM_SIGN_SCRIPT}
    -DENABLE_SIGN=${ENABLE_SIGN}
    -DVERSION_INFO=${VERSION_INFO}
    <SOURCE_DIR>
    BUILD_ALWAYS 1)
endif()


# copy include to asc impl
if(NOT "${DPRODUCT_SIDE}" STREQUAL "device")
  install(
    FILES ${KERNEL_TILING_HEAD}
    DESTINATION
      ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit)

  # copy detail to asc impl
  install(
    DIRECTORY detail/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/adv_api/detail
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit FILES_MATCHING
    PATTERN "*.h"
    PATTERN "CMakeLists.txt" EXCLUDE)

  # copy tiling to asc impl
  install(
    DIRECTORY tiling/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/adv_api/tiling
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit FILES_MATCHING
    PATTERN "*.h"
    PATTERN "CMakeLists.txt" EXCLUDE)

  # cp include to asc include
  install(
    DIRECTORY ${ASCENDC_INCLUDE_DIR}/adv_api/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit)

  # generate softlink
  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_headers.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_headers.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_headers.cmake COMPONENT asc-devkit)

  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_impl.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_impl.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_impl.cmake COMPONENT asc-devkit)

  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_directory.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_directory.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_directory.cmake COMPONENT asc-devkit)

  configure_file(
      ${ASCENDC_ADV_API_CMAKE_DIR}/utils_headers.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/utils_headers.cmake
      @ONLY
  )
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/utils_headers.cmake COMPONENT asc-devkit)
endif()  

if (BUILD_OPEN_PROJECT)
    add_subdirectory(detail/hccl/cc)
endif()

if(NOT BUILD_OPEN_PROJECT)
  install_package(
    PACKAGE kernel_tiling
    TARGETS kernel_tiling_headers
    FILES ${KERNEL_TILING_HEAD}
    DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include/adv_api/)
endif()
