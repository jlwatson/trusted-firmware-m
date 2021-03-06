#-------------------------------------------------------------------------------
# Copyright (c) 2018-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file holds information of a specific build configuration of this project.

#Include board specific config (CPU, etc...), select platform specific build
#system settings file
if(NOT DEFINED TARGET_PLATFORM)
	message(FATAL_ERROR "ERROR: TARGET_PLATFORM is not set in command line")
elseif(${TARGET_PLATFORM} STREQUAL "AN521")
	set(PLATFORM_CMAKE_FILE "${CMAKE_CURRENT_LIST_DIR}/platform/ext/Mps2AN521.cmake")
else()
	message(FATAL_ERROR "ERROR: Target \"${TARGET_PLATFORM}\" is not supported.")
endif()

##These variables select how the projects are built. Each project will set
#various project specific settings (e.g. what files to build, macro
#definitions) based on these.
set (REGRESSION False)
set (CORE_TEST False)
set (CORE_IPC True)

# TF-M isolation level: 1..3
set (TFM_LVL 2)

#BL2 bootloader(MCUBoot) related settings
if(NOT DEFINED BL2)
	set(BL2 True)
endif()

if(NOT DEFINED MCUBOOT_NO_SWAP)
	set(MCUBOOT_NO_SWAP False)
endif()

if(NOT DEFINED MCUBOOT_RAM_LOADING)
	set(MCUBOOT_RAM_LOADING False)
endif()

include ("${CMAKE_CURRENT_LIST_DIR}/CommonConfig.cmake")
