#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
#----------------------------------------------------------------------
#
# Mersenne Twister : a pseudorandom number generator
#
#----------------------------------------------------------------------
# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# library
set(LIBMT19937 "mt19937")

# make test program (default: false)
set(BUILD_TEST_EXEC FALSE)
set(MT19937_TEST_EXEC "mtTest")

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
  set(CMAKE_C_COMPILER   gcc)
endif(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
  AND ${PLATFORM} STREQUAL "cygwin")

# compile options
# build type = Debug (defualt)
message(STATUS "configuring for ${CMAKE_BUILD_TYPE} ...")

# options for release build
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wno-sign-compare")

# options for windows native application
# #compile on MinGW
set(CMAKE_C_FLAGS_WINNATIVE   "-O3 -Wno-sign-compare -mwindows -static-libgcc")

# options for debug build
set(CMAKE_C_FLAGS_DEBUG   "-g -Wall -Wno-sign-compare")

#----------------------------------------------------------------------
# program
set(MT19937_HDRS
  mt19937ar.h
  )

set(MT19937_SRCS
  mt19937ar.c
  )

set(MT19937_TEST_SRCS
  mtTest.c
  )

#----------------------------------------------------------------------
# build library
add_library(${LIBMT19937} STATIC ${MT19937_SRCS})

# build test program
if(BUILD_TEST_EXEC)
  add_executable(${MT19937_TEST_EXEC} ${MT19937_TEST_SRCS})
  target_link_libraries(${MT19937_TEST_EXEC} ${LIBMT19937})
endif(BUILD_TEST_EXEC)

#----------------------------------------------------------------------
# clean target
set(CLEAN_TARGETS
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/*.a
  )

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_TARGETS}"
  )

#----------------------------------------------------------------------
# clear cache for next execution
unset(CMAKE_BUILD_TYPE CACHE)

