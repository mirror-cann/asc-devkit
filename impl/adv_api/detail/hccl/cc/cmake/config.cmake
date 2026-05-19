# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(DEFAULT_BUILD_TYPE "Release")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the build type: Release/Debug" FORCE)
endif()

if(CUSTOM_ASCEND_CANN_PACKAGE_PATH)
    set(ASCEND_CANN_PACKAGE_PATH  ${CUSTOM_ASCEND_CANN_PACKAGE_PATH})
elseif(DEFINED ENV{ASCEND_HOME_PATH})
    set(ASCEND_CANN_PACKAGE_PATH  $ENV{ASCEND_HOME_PATH})
elseif(DEFINED ENV{ASCEND_OPP_PATH})
    get_filename_component(ASCEND_CANN_PACKAGE_PATH "$ENV{ASCEND_OPP_PATH}/.." ABSOLUTE)
else()
    set(ASCEND_CANN_PACKAGE_PATH  "/usr/local/Ascend/ascend-toolkit/latest")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(HCCL_HOST_ARCH_PREFIX x86_64-linux)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|arm")
    set(HCCL_HOST_ARCH_PREFIX aarch64-linux)
else()
    message(FATAL_ERROR "Unsupported host architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

set(HCCL_HOST_LIB_DIRS
    ${ASCEND_CANN_PACKAGE_PATH}/lib64
    ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/lib64
)

set(HCCL_HOST_DEVLIB_DIRS
    ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/devlib
    ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/devlib/device
)

set(HCCL_DEVICE_LIB_DIRS
    ${ASCEND_CANN_PACKAGE_PATH}/devlib/device
)

set(ASCEND_MOCKCPP_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# if (NOT EXISTS "${ASCEND_CANN_PACKAGE_PATH}")
#     message(FATAL_ERROR "${ASCEND_CANN_PACKAGE_PATH} does not exist, please install the cann package and set environment variables.")
# endif()

# if (NOT EXISTS "${THIRD_PARTY_NLOHMANN_PATH}")
#     message(FATAL_ERROR "${THIRD_PARTY_NLOHMANN_PATH} does not exist, please check the setting of THIRD_PARTY_NLOHMANN_PATH.")
# endif()

set(ASCEND_SDK_PACKAGE_PATH "${ASCEND_CANN_PACKAGE_PATH}")
if (NOT EXISTS "${ASCEND_CANN_PACKAGE_PATH}/opensdk")
    # 设置社区包sdk安装位置
    set(ASCEND_SDK_PACKAGE_PATH "${ASCEND_CANN_PACKAGE_PATH}/../../latest")
endif()

#execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/check_version_compatiable.sh
#                             ${ASCEND_CANN_PACKAGE_PATH}
#                             hccl
#                             ${CMAKE_CURRENT_SOURCE_DIR}/version.info
#    RESULT_VARIABLE result
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#    OUTPUT_VARIABLE CANN_VERSION
#    )

#if (result)
#    message(FATAL_ERROR "${CANN_VERSION}")
#else()
#     string(TOLOWER ${CANN_VERSION} CANN_VERSION)
#endif()

if (CMAKE_INSTALL_PREFIX STREQUAL /usr/local)
    set(CMAKE_INSTALL_PREFIX     "${HCCL_CC_DIR}/output"  CACHE STRING "path for install()" FORCE)
endif ()

set(HI_PYTHON                     "python3"                       CACHE   STRING   "python executor")

message(STATUS "config.cmake KERNEL_MODE=${KERNEL_MODE} BUILD_OPEN_PROJECT=${BUILD_OPEN_PROJECT}")
if(BUILD_OPEN_PROJECT AND KERNEL_MODE)
    set(PRODUCT_SIDE                  device)
else()
    set(PRODUCT_SIDE                  host)
endif()

if(PRODUCT_SIDE STREQUAL "device")
    set(HCCL_PRODUCT_LIB_DIRS ${HCCL_DEVICE_LIB_DIRS})
    set(HCCL_PRODUCT_DEVLIB_DIRS ${HCCL_DEVICE_LIB_DIRS})
    set(HCCL_C_SEC_LINK -lc_sec)
    set(HCCL_MMPA_LINK -lmmpa)
    set(HCCL_UNIFIED_DLOG_LINK -lunified_dlog)
else()
    set(HCCL_PRODUCT_LIB_DIRS ${HCCL_HOST_LIB_DIRS})
    set(HCCL_PRODUCT_DEVLIB_DIRS ${HCCL_HOST_DEVLIB_DIRS})
    set(HCCL_C_SEC_LINK c_sec)
    set(HCCL_MMPA_LINK mmpa)
    set(HCCL_UNIFIED_DLOG_LINK unified_dlog)
endif()

set(INSTALL_LIBRARY_DIR ${CMAKE_SYSTEM_PROCESSOR}-linux/lib64)
set(INSTALL_AICPU_KERNEL_JSON_DIR opp/built-in/op_impl/aicpu/config)
set(INSTALL_DEVICE_TAR_DIR opp/built-in/op_impl/aicpu/kernel)
set(INSTALL_PKG_INCLUDE_DIR ${CMAKE_SYSTEM_PROCESSOR}-linux/pkg_inc)
set(INSTALL_CCL_KERNEL_JSON_DIR opp/built-in/op_impl/aicpu/config)
set(INSTALL_DPU_KERNEL_JSON_DIR opp/built-in/op_impl/dpu/config)
if (ENABLE_TEST)
    set(CMAKE_SKIP_RPATH FALSE)
else ()
    set(CMAKE_SKIP_RPATH TRUE)
endif ()
