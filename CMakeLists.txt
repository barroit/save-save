# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(MAJOR_VERSION  0)
set(PATCH_VERSION  1)
set(TWEAK_VERSION  0)
set(SAVESAVE_VERSION ${MAJOR_VERSION}.${PATCH_VERSION}.${TWEAK_VERSION})

include($ENV{ROOT}/scripts/kconfig.cmake)

if (CONFIG_CROSS_COMPILING OR NOT $ENV{HOST} STREQUAL linux)
  set(BUILD windows)
else()
  set(BUILD linux)
endif()

if (CONFIG_CROSS_COMPILING)
  set(CMAKE_C_COMPILER ${CONFIG_COMPILER_PREFIX}${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_COMPILER_PREFIX}${CONFIG_CXX_NAME})
elseif(NOT $ENV{HOST} STREQUAL linux) # Windows
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_CXX_NAME})
else() # Linux
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
endif()

# Let us handle flags
set(CMAKE_BUILD_TYPE "")

if (CONFIG_CROSS_COMPILING)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 22H2)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS true)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CONFIG_COMPILER_WARNING_ALL)
add_compile_options(-Wall)
endif()

if (CONFIG_COMPILER_WARNING_EXTRA)
add_compile_options(-Wextra)
endif()

set_choiced_value(CONFIG_COMPILER_DEBUG)
add_compile_options(-g${CONFIG_COMPILER_DEBUG})

set_choiced_value(CONFIG_COMPILER_OPTIMIZE)
add_compile_options(-O${CONFIG_COMPILER_OPTIMIZE})

# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include $ENV{AUTOCONF}")
add_compile_options("SHELL:-include $ENV{ROOT}/include/compat.h")

list(APPEND languages C)

if (BUILD STREQUAL windows)
list(APPEND languages CXX)
endif()

project(savesave VERSION ${SAVESAVE_VERSION} LANGUAGES ${languages})

include($ENV{ROOT}/scripts/genheader.cmake)

configure_file($ENV{ROOT}/savesave.manifest.in $ENV{ROOT}/savesave.manifest)

include_directories($ENV{ROOT}/include)

if (BUILD STREQUAL linux)
  file(GLOB platspec_src LIST_DIRECTORIES false uni/*.c)
  set(mainfile $ENV{ROOT}/entry/savesave.c)
else()
  file(GLOB platspec_src LIST_DIRECTORIES false win/*.cpp)
  set(mainfile $ENV{ROOT}/entry/savesave.cpp)
endif()
add_library(platspec OBJECT ${platspec_src})

file(GLOB platfree_src LIST_DIRECTORIES false platfree/*.c)
add_library(platfree OBJECT ${platfree_src})

if (BUILD STREQUAL linux)
  add_executable(savesave ${mainfile})
else()
  add_executable(savesave WIN32 ${mainfile} $ENV{ROOT}/savesave.rc)
endif()
target_link_libraries(savesave platfree platspec)

if (BUILD STREQUAL windows)
target_link_libraries(savesave ws2_32 comctl32)
endif()

# For the best portable
if (CONFIG_CROSS_COMPILING)
target_link_options(savesave PRIVATE -static-libstdc++)
endif()

add_compile_definitions(_GNU_SOURCE)

if (BUILD STREQUAL windows)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7)
  if (NOT CONFIG_CROSS_COMPILING) # clang
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  endif()
endif()
