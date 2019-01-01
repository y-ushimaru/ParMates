#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
# cmake version
cmake_minimum_required(VERSION 2.6)

#----------------------------------------------------------------------
# project
project(AdvMATES)
set(serial "0.7b")

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

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

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#----------------------------------------------------------------------
# install directory
if(NOT DEFINED INSTALL_DIR)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/ADVENTURE")
else(NOT DEFINED INSTALL_DIR)
  set(CMAKE_INSTALL_PREFIX "${INSTALL_DIR}")
endif(NOT DEFINED INSTALL_DIR)
set(INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#----------------------------------------------------------------------
# duplicate custom targets
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

#----------------------------------------------------------------------
# subdirectories
# - build libraries before solvers
# - build solvers before pre modules
if(${PLATFORM} STREQUAL "novis")
  subdirs(
    lib/mt19937 lib/XSadd
    solver solver/tram solver/pedestrian
    doc
    )
else(${PLATFORM} STREQUAL "novis")
  subdirs(
    lib/autogl lib/mt19937 lib/XSadd
    solver solver/tram solver/pedestrian
    doc
    )
endif(${PLATFORM} STREQUAL "novis")

#----------------------------------------------------------------------
# clear cache for next execution
unset(PLATFORM CACHE)
unset(CMAKE_BUILD_TYPE CACHE)
unset(INSTALL_DIR CACHE)
