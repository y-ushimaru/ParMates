#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message(STATUS "*** cmake: ${CMAKE_CURRENT_SOURCE_DIR} ***")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

#----------------------------------------------------------------------
# install directory
unset(CMAKE_INSTALL_PREFIX CACHE)
unset(INSTALL_PREFIX CACHE)
if(INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
else(INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/ADVENTURE")
endif(INSTALL_PREFIX)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#----------------------------------------------------------------------
# install (copy) manuals
set(OSM2MATES_MANUAL_JP osm2mates-manual-jp)

install(FILES ${OSM2MATES_MANUAL_JP}.pdf DESTINATION doc/AdvMates)
