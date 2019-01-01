#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
#----------------------------------------------------------------------
#
# Map Converter for ADVENTURE_Mates
# (MATES : Multi-Agent-based Traffic and Environment Simulator)
#
#----------------------------------------------------------------------
# cmake version
cmake_minimum_required(VERSION 2.8)

# start cmake
message(STATUS "*** cmake: ${CMAKE_CURRENT_SOURCE_DIR} ***")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# make advmates-osm2mates (default: true)
set(BUILD_O2M TRUE)
set(MATES_O2M "advmates-osm2mates")

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
# compiler
set(CMAKE_CXX_COMPILER g++)

# compile options
# build type = Debug (defualt)
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "build type is not specified, set to debug build")
  set(CMAKE_BUILD_TYPE "debug")
endif(NOT CMAKE_BUILD_TYPE)
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

message(STATUS "configuring for ${CMAKE_BUILD_TYPE}...")

# options for release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wno-sign-compare")

# options for debug build
set(CMAKE_CXX_FLAGS_DEBUG   "-g -Wall -Wno-sign-compare")

#----------------------------------------------------------------------
# MATES global settings
set(MATES_HEADER_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
set(MATES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")

# include path
include_directories(${MATES_HEADER_DIR})

# library path
link_directories(${MATES_LIBRARY_DIR})

#----------------------------------------------------------------------
# source
set(O2M_SRCS
  app.cpp
  intsec.cpp
  main.cpp
  map.cpp
  merge.cpp
  matesFile.cpp
  osmFile.cpp
  road.cpp
  )

#----------------------------------------------------------------------
#
# Qt5Core
#
find_package(Qt5Core)
message(STATUS "Finding Qt5Core ... ${Qt5Core_FOUND}")
if(${Qt5Core_FOUND})
  include_directories(${Qt5Core_INCLUDE_DIRS})
  add_definitions(${Qt5Core_DEFINITIONS})
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
else(${Qt5Core_FOUND})
  #
  # Qt4
  #
  find_package(Qt4 COMPONENTS QTCORE)
  message(STATUS "Finding Qt4... ${QT4_FOUND}")
  if(${QT4_FOUND})
    include(${QT_USE_FILE})
  else(${QT4_FOUND})
    message(STATUS "Try to use qmake-qt4")
    set(QT_QMAKE_EXECUTABLE "qmake-qt4")
    find_package(Qt4 COMPONENTS QTCORE)
    message(STATUS "Finding Qt4... ${QT4_FOUND}")
    if(${QT4_FOUND})
      include(${QT_USE_FILE})
    else(${QT4_FOUND})
      set(BUILD_O2M FALSE)
    endif(${QT4_FOUND})
  endif(${QT4_FOUND})
endif(${Qt5Core_FOUND})

#----------------------------------------------------------------------
# build advmates-osm2mates
if(${BUILD_O2M})
    add_executable(${MATES_O2M} ${O2M_SRCS})
    target_link_libraries(${MATES_O2M} mates-util)
    if(${Qt5Core_FOUND})
      target_link_libraries(${MATES_O2M} ${Qt5Core_LIBRARIES})
    elseif(${QT4_FOUND})
      target_link_libraries(${MATES_O2M} ${QT_LIBRARIES})
    endif(${Qt5Core_FOUND})
endif(${BUILD_O2M})

#----------------------------------------------------------------------
# install advmates-osm2mates
install(TARGETS ${MATES_O2M} DESTINATION bin)

#----------------------------------------------------------------------
# clean target
set(CLEAN_TARGETS
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
  )

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_TARGETS}"
  )

#----------------------------------------------------------------------
# for flymake
get_directory_property(FM_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
foreach(FM_INCLUDE_DIRECTORY ${FM_INCLUDE_DIRECTORIES})
  set(FM_INCLUDES ${FM_INCLUDES} -I${FM_INCLUDE_DIRECTORY})
endforeach(FM_INCLUDE_DIRECTORY ${FM_INCLUDE_DIRECTORIES})

add_custom_target(check-syntax
  ${CMAKE_CXX_COMPILER} ${FM_INCLUDES}
  -fPIE -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
)

#----------------------------------------------------------------------
# subdirectories
subdirs(
  doc
)
