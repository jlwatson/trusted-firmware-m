#-------------------------------------------------------------------------------
# Copyright (c) 2018-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "crypto" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#       MBEDTLS_INSTALL_DIR - directory where mbed TLS headers and libraries can be found. Needed only when using CRYPTO_ENGINE_MBEDTLS ON.
#       TFM_ROOT_DIR        - root directory of the TF-M repository.
#Outputs:
#       Will modify include directories to make the source compile.
#       ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(CRYPTO_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED ENABLE_CRYPTO)
  message(FATAL_ERROR "Incomplete build configuration: ENABLE_CRYPTO is undefined. ")
endif()

if (NOT DEFINED CRYPTO_ENGINE_MBEDTLS)
  message(FATAL_ERROR "Incomplete build configuration: CRYPTO_ENGINE_MBEDTLS is undefined. ")
endif()

if (ENABLE_CRYPTO)
  if (CRYPTO_ENGINE_MBEDTLS)
    if (NOT DEFINED MBEDTLS_INSTALL_DIR)
      message(FATAL_ERROR "Please set MBEDTLS_INSTALL_DIR before including this file.")
    endif()
  endif()

  if (NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
  endif()

  set (CRYPTO_C_SRC "${CRYPTO_DIR}/crypto_init.c"
                    "${CRYPTO_DIR}/crypto_alloc.c"
                    "${CRYPTO_DIR}/crypto_cipher.c"
                    "${CRYPTO_DIR}/crypto_hash.c"
                    "${CRYPTO_DIR}/crypto_mac.c"
                    "${CRYPTO_DIR}/crypto_key.c"
                    "${CRYPTO_DIR}/crypto_engine.c"
                    "${CRYPTO_DIR}/crypto_aead.c"
                    "${CRYPTO_DIR}/tfm_crypto_secure_api.c"
      )

  #Append all our source files to global lists.
  list(APPEND ALL_SRC_C ${CRYPTO_C_SRC})
  unset(CRYPTO_C_SRC)

  #Setting include directories
  embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
  embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
  if (CRYPTO_ENGINE_MBEDTLS)
    embedded_include_directories(PATH ${MBEDTLS_INSTALL_DIR}/include ABSOLUTE)
  endif()

  #Inform the user about Crypto service features selected based on the Crypto service cmake flags
  message("The Crypto service compile configuration is as follows:")
  message("- CRYPTO_ENGINE_MBEDTLS: " ${CRYPTO_ENGINE_MBEDTLS})
  if (NOT DEFINED CRYPTO_KEY_STORAGE_NUM)
    message("- CRYPTO_KEY_STORAGE_NUM using default value")
  else()
    message("- CRYPTO_KEY_STORAGE_NUM: " ${CRYPTO_KEY_STORAGE_NUM})
  endif()
  if (NOT DEFINED CRYPTO_KEY_MAX_KEY_LENGTH)
    message("- CRYPTO_KEY_MAX_KEY_LENGTH using default value")
  else()
    message("- CRYPTO_KEY_MAX_KEY_LENGTH: " ${CRYPTO_KEY_MAX_KEY_LENGTH})
  endif()
  if (NOT DEFINED CRYPTO_ENGINE_BUF_SIZE)
    message("- CRYPTO_ENGINE_BUF_SIZE using default value")
  else()
    message("- CRYPTO_ENGINE_BUF_SIZE: " ${CRYPTO_ENGINE_BUF_SIZE})
  endif()

else()
  message(FATAL_ERROR "Build system currently doesn't support selectively disabling of a service.")
endif()
