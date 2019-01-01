#####################################################
# Copyright (C) 2015 ADVENTURE Project
# All Rights Reserved
#####################################################
#----------------------------------------------------------------------
#
# XORSHIFT-ADD(xsadd) : a pseudorandom number generator
#
#----------------------------------------------------------------------
# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# library
set(LIBXSADD "xsadd")

# test program
set(XSADD_TEST_EXEC "test_xsadd")

#----------------------------------------------------------------------
# platform detection
if(NOT DEFINED PLATFORM)
  set(PLATFORM ${CMAKE_SYSTEM_NAME})
endif(NOT DEFINED PLATFORM)
string(TOLOWER "${PLATFORM}" PLATFORM)

if(${PLATFORM} STREQUAL "windows")
  if(${CYGWIN})
    set(PLATFORM "cygwin")
  elseif(${MINGW})
    set(PLATFORM "mingw")
  endif(${CYGWIN})
endif(${PLATFORM} STREQUAL "windows")

message(STATUS "PLATFORM: ${PLATFORM}")

#----------------------------------------------------------------------
# build type detection
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release")
endif(NOT CMAKE_BUILD_TYPE)
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

if(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
    AND NOT ${PLATFORM} STREQUAL "cygwin"
    AND NOT ${PLATFORM} STREQUAL "mingw")
  message(WARNING
    "winnative mode is only valid on cygwin or mingw platform")
  set(CMAKE_BUILD_TYPE "release")
endif(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
  AND NOT ${PLATFORM} STREQUAL "cygwin"
  AND NOT ${PLATFORM} STREQUAL "mingw")

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#----------------------------------------------------------------------
# compiler
if(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
    AND ${PLATFORM} STREQUAL "cygwin")
  set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
else(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
    AND ${PLATFORM} STREQUAL "cygwin")
  set(CMAKE_C_COMPILER gcc)
endif(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
  AND ${PLATFORM} STREQUAL "cygwin")

# compile options
# build type = Debug (defualt)
message(STATUS "configuring for ${CMAKE_BUILD_TYPE} ...")

# options for release build
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wno-sign-compare -std=c99")

# options for windows native application
# #compile on MinGW
set(CMAKE_C_FLAGS_WINNATIVE   "-O3 -Wno-sign-compare -mwindows -static-libgcc -std=c99")

# options for debug build
set(CMAKE_C_FLAGS_DEBUG   "-DDEBUG=1 -O0 -g -Wall -Wno-sign-compare -std=c99")

#----------------------------------------------------------------------
# program
set(XSADD_HDRS
  xsadd.h
  )

set(XSADD_SRCS
  xsadd.c
  )

set(XSADD_TEST_SRCS
  test_xsadd.c
  )

#----------------------------------------------------------------------
# build library
include_directories(.)
add_library(${LIBXSADD} STATIC ${XSADD_SRCS})

# build test program
add_custom_target(testexec)

add_executable(${XSADD_TEST_EXEC} EXCLUDE_FROM_ALL
  ${XSADD_TEST_SRCS})
target_link_libraries(${XSADD_TEST_EXEC} ${LIBXSADD})
add_dependencies(testexec ${XSADD_TEST_EXEC}) 

# build documentation

# The following variables will be defined:
#   DOXYGEN_FOUND
#   DOXYGEN_DOT_FOUND
#   DOXYGEN_EXECUTABLE
find_package(Doxygen)
message(STATUS "Finding doxygen ... ${DOXYGEN_FOUND}")
if(DOXYGEN_FOUND)
  add_custom_target(dox
    COMMAND ${DOXYGEN_EXECUTABLE} doxygen.cfg
    WORKING_DIRECTORY .
    )
endif(DOXYGEN_FOUND)

#----------------------------------------------------------------------
# clean target
set(CLEAN_TARGETS
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/html
  )

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_TARGETS}"
  )

#----------------------------------------------------------------------
# clear cache for next execution
unset(CMAKE_BUILD_TYPE CACHE)
