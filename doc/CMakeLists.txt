#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

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
# install (copy) manuals
set(MATES_MANUAL_JP AdvMates-manual-jp)

install(FILES ${MATES_MANUAL_JP}.pdf DESTINATION doc/AdvMates)

#----------------------------------------------------------------------
# clear cache for next execution
unset(INSTALL_DIR CACHE)
