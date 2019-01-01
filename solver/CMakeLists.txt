#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
#----------------------------------------------------------------------
#
# ADVENTURE_Mates
# (MATES : Multi-Agent-based Traffic and Environment Simulator)
#
#----------------------------------------------------------------------

# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

#----------------------------------------------------------------------
# platform detection
if(NOT DEFINED PLATFORM)
  set(PLATFORM ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER "${PLATFORM}" PLATFORM)

if(${PLATFORM} STREQUAL "windows")
  if(${CYGWIN})
    set(PLATFORM "cygwin")
  elseif(${MINGW})
    set(PLATFORM "mingw")
  endif()
endif()

message(STATUS "PLATFORM: ${PLATFORM}")

#----------------------------------------------------------------------
# buildtype detection
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release")
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

if(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
    AND NOT ${PLATFORM} STREQUAL "cygwin"
    AND NOT ${PLATFORM} STREQUAL "mingw")
  message(WARNING
    "winnative mode is only valid on cygwin or mingw platform")
  set(PLATFORM "mingw")
  set(CMAKE_BUILD_TYPE "release")
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#----------------------------------------------------------------------
# install directory
if(NOT DEFINED INSTALL_DIR)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/ADVENTURE")
else()
  set(CMAKE_INSTALL_PREFIX "${INSTALL_DIR}")
endif()
set(INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#----------------------------------------------------------------------
# libraries
set(LIBMATESUTIL "mates-util")
set(LIBMATES     "mates")
set(LIBMATESGL   "matesGL")

# make advmates-calc (default: true)
set(BUILD_MATES_CALC TRUE)
set(MATES_CALC "advmates-calc")

# make advmates-sim (default: true)
set(BUILD_MATES_SIM TRUE)
set(MATES_SIM "advmates-sim")
if(${PLATFORM} STREQUAL "novis")
  set(BUILD_MATES_SIM FALSE)
endif()

# using "advautogl-config" (defalut: false)
set(USE_AUTOGL_CONFIG FALSE)

# using OpenMP (default: true) (without Cygwin, MinGW)
option(USE_OPENMP "Using OpenMP" OFF)
if(${PLATFORM} STREQUAL "cygwin"
    OR ${PLATFORM} STREQUAL "mingw")
  set(USE_OPENMP OFF)
endif()

#----------------------------------------------------------------------
# compiler
if(${CMAKE_BUILD_TYPE} STREQUAL "winnative"
    AND ${PLATFORM} STREQUAL "cygwin")
  set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
  set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
else()
  set(CMAKE_C_COMPILER   mpicc)
  set(CMAKE_CXX_COMPILER mpicxx)
endif()

# compile options

# check c++11 compiler option
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
  message(STATUS "Please use a different C++ compiler.")
endif()

# build type = Release (defualt)
message(STATUS "configuring for ${CMAKE_BUILD_TYPE} ...")

# options for release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wno-sign-compare")
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wno-sign-compare")

# options for windows native application
set(CMAKE_CXX_FLAGS_WINNATIVE "-O3 -Wno-sign-compare -mwindows -static-libgcc -static-libstdc++")
set(CMAKE_C_FLAGS_WINNATIVE   "-O3 -Wno-sign-compare -mwindows -static-libgcc")

# options for debug build
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg -Wall -Wno-sign-compare")
set(CMAKE_C_FLAGS_DEBUG   "-g -pg -Wall -Wno-sign-compare")

#----------------------------------------------------------------------
# include bundled libraries

# AdvMates global settings
set(MATES_HEADER_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(MATES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

# AutoGL settings
set(AUTOGL_HEADER_DIR  "${MATES_LIBRARY_DIR}/autogl")
set(AUTOGL_LIBRARY_DIR "${MATES_LIBRARY_DIR}/autogl")

# for use advautogl-config
if(AUTOGL_CONFIG)
  execute_process(
    COMMAND
    sh ${AUTOGL_LIBRARY_DIR}/advautogl-config
    OUTPUT_VARIABLE
    AUTOGL_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# MT19937 (pseudo random number generator) settings
set(MT19937_HEADER_DIR  "${MATES_LIBRARY_DIR}/mt19937")
set(MT19937_LIBRARY_DIR "${MATES_LIBRARY_DIR}/mt19937")

# XSadd (pseudo random  number generator) settings
set(XSADD_HEADER_DIR  "${MATES_LIBRARY_DIR}/XSadd")
set(XSADD_LIBRARY_DIR "${MATES_LIBRARY_DIR}/XSadd")

#----------------------------------------------------------------------
# simulation settings

option(INCLUDE_VEHICLES "Define INCLUDE_VEHICLES macro" ON)
if(INCLUDE_VEHICLES)
  add_definitions("-DINCLUDE_VEHICLES")
endif()

option(INCLUDE_TRAMS "Define INCLUDE_TRAMS macro" OFF)
if(INCLUDE_TRAMS)
  add_definitions("-DINCLUDE_TRAMS")
endif()

option(INCLUDE_PEDESTRIANS "Define INCLUDE_PEDESTRIANS macro" OFF)
if(INCLUDE_PEDESTRIANS)
  add_definitions("-DINCLUDE_PEDESTRIANS")
endif()

option(GENERATE_VEHICLE_VELOCITY_ZERO
  "Define GENERATE_VEHICLE_VELOCITY_ZERO macro" OFF)
if(GENERATE_VEHICLE_VELOCITY_ZERO)
  add_definitions("-DGENERATE_VEHICLE_VELOCITY_ZERO")
endif()

option(UNIQUE_NETWORK
  "Define UNIQUE_NETWORK macro" ON)
if(UNIQUE_NETWORK)
  add_definitions("-DUNIQUE_NETWORK")
endif()

option(MEASURE_TIME "Define MEASURE_TIME macro" ON)
if(MEASURE_TIME)
  add_definitions("-DMEASURE_TIME")
endif()

option(DEBUG "Define DEBUG macro" OFF)
if(DEBUG)
  add_definitions("-DDEBUG")
endif()
#----------------------------------------------------------------------
# find packages

# zlib
# The following variables will be defined:
#  ZLIB_FOUND
#  ZLIB_INCLUDE_DIRS
#  ZLIB_LIBRARIES
find_package(ZLIB)
message(STATUS "Finding zlib ... ${ZLIB_FOUND}")
if(ZLIB_FOUND)
  add_definitions("-DUSE_ZLIB")
  link_libraries(${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)

#**********************************************************************
if(NOT USE_AUTOGL_CONFIG AND BUILD_MATES_SIM)

  #********************************************************************
  # for PLATFORM LINUX
  #
  if(${PLATFORM} STREQUAL "linux")
    # OpenGL
    # The following variables will be defined:
    #   OPENGL_FOUND
    #   OPENGL_INCLUDE_DIR
    #   OPENGL_LIBRARIES
    find_package(OpenGL)
    message(STATUS "Finding OpenGL ... ${OPENGL_FOUND}")
    if(NOT OPENGL_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

    # OpenCV
    # The following variables will be defined:
    #   OpenCV_FOUND
    #   OpenCV_INCLUDE_DIRS
    #   OpenCV_LIBS
    find_package(OpenCV)
    message(STATUS "Finding OpenCV ... ${OpenCV_FOUND}")
    if(OpenCV_FOUND)
      add_definitions("-DUSE_OPENCV")
    endif()

    # GTK2
    # The following variables will be defined:
    #   GTK2_FOUND
    #   GTK2_INCLUDE_DIRS
    #   GTK2_LIBRARIES
    find_package(GTK2)
    message(STATUS "Finding GTK2 ... ${GTK2_FOUND}")
    if(NOT GTK2_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

    # X11
    # The folloing variables will be defined:
    #   X11_FOUND
    #   X11_INCLUDE_DIR
    #   X11_LIBRARIES
    find_package(X11)
    message(STATUS "Finding X11 ... ${X11_FOUND}")
    if(NOT X11_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

  endif()

  #********************************************************************
  # for PLATFORM Unix
  # ... but not tested enough
  #
  if(${PLATFORM} STREQUAL "unix")
    # OpenGL
    # The following variables will be defined:
    #   OPENGL_FOUND
    #   OPENGL_INCLUDE_DIR
    #   OPENGL_LIBRARIES
    find_package(OpenGL)
    message(STATUS "Finding OpenGL ... ${OPENGL_FOUND}")
    if(NOT OPENGL_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

    # OpenCV
    # The following variables will be defined:
    #   OpenCV_FOUND
    #   OpenCV_INCLUDE_DIRS
    #   OpenCV_LIBS
    find_package(OpenCV)
    message(STATUS "Finding OpenCV ... ${OpenCV_FOUND}")
    if(OpenCV_FOUND)
      add_definitions("-DUSE_OPENCV")
    endif()

    # Motif
    # The folloing variables will be defined:
    #   MOTIF_FOUND
    #   MOTIF_INCLUDE_DIR
    #   MOTIF_LIBRARIES
    find_package(Motif)
    message(STATUS "Finding Motif ... ${MOTIF_FOUND}")
    if(NOT MOTIF_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

    # X11
    # The folloing variables will be defined:
    #   X11_FOUND
    #   X11_INCLUDE_DIR
    #   X11_LIBRARIES
    find_package(X11)
    message(STATUS "Finding X11 ... ${X11_FOUND}")
    if(NOT X11_FOUND)
      set(BUILD_MATES_SIM FALSE)
    endif()

  endif()

  #********************************************************************
  # for PLATFORM Darwin (OSX)
  # ... but not tested at all
  #
  if(${PLATFORM} STREQUAL "darwin")
    # OpenGL
    # The following variables will be defined:
    #   OPENGL_FOUND
    #   OPENGL_INCLUDE_DIR
    #   OPENGL_LIBRARIES
    find_package(OpenGL)
    message(STATUS "Finding OpenGL ... ${OPENGL_FOUND}")
    if(OPENGL_FOUND)
    else()
      set(BUILD_MATES_SIM FALSE)
    endif()

    # OpenCV
    # The following variables will be defined:
    #   OpenCV_FOUND
    #   OpenCV_INCLUDE_DIRS
    #   OpenCV_LIBS
    find_package(OpenCV)
    message(STATUS "Finding OpenCV ... ${OpenCV_FOUND}")
    if(OpenCV_FOUND)
      add_definitions("-DUSE_OPENCV")
    endif()

    # GLUT
    # The following variables will be defined:
    #   GLUT_FOUND
    #   GLUT_INCLUDE_DIR
    #   GLUT_LIBRARIES
    find_package(GLUT)
    message(STATUS "Finding glut ... ${GLUT_FOUND}")
    if(GLUT_FOUND)
    else()
      set(BUILD_MATES_SIM FALSE)
    endif()

    # GTK2
    # The following variables will be defined:
    #   GTK2_FOUND
    #   GTK2_INCLUDE_DIRS
    #   GTK2_LIBRARIES
    find_package(GTK2)
    message(STATUS "Finding GTK2 ... ${GTK2_FOUND}")
    if(GTK2_FOUND)
      include_directories(${GTK2_INCLUDE_DIRS})
    else()
      set(BUILD_MATES_SIM FALSE)
    endif()

    # X11
    # The following variables will be defined:
    #   X11_FOUND
    #   X11_INCLUDE_DIR
    #   X11_LIBRARY_DIR
    #   X11_LIBRARIES
    find_package(X11)
    message(STATUS "Finding X11 ... ${X11_FOUND}")
    if(X11_FOUND)
      include_directories(${X11_INCLUDE_DIR})
    else()
      set(BUILD_MATES_SIM FALSE)
    endif()

  endif()

  #********************************************************************
  # for PLATFORM Cygwin on Windows
  #
  if(${PLATFORM} STREQUAL "cygwin")
    set(CMAKE_LEGACY_CYGWIN_WIN32 TRUE)
    set(CMAKE_C_FLAGS_RELEASE
      "${CMAKE_C_FLAGS_RELEASE} -DUSE_OPENGL32")
    set(CMAKE_CXX_FLAGS_RELEASE
      "${CMAKE_CXX_FLAGS_RELEASE} -DUSE_OPENGL32")
    set(CMAKE_C_FLAGS_DEBUG
      "${CMAKE_C_FLAGS_DEBUG} -DUSE_OPENGL32")
    set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_OPENGL32")

    # OpenCV
    # The following variables will be defined:
    #   OpenCV_FOUND
    #   OpenCV_INCLUDE_DIRS
    #   OpenCV_LIBS
    find_package(OpenCV)
    message(STATUS "Finding OpenCV ... ${OpenCV_FOUND}")
    if(OpenCV_FOUND)
      add_definitions("-DUSE_OPENCV")
    endif()

    if(${CMAKE_BUILD_TYPE} STREQUAL "winnative")
      add_definitions("-DUSE_MINGW")
      remove_definitions("-DUSE_OPENCV")
    endif()

  endif()

  #********************************************************************
  # for PLATFORM MinGW on Windows
  # ... but not tested enough
  #
  if(${PLATFORM} STREQUAL "mingw")

    # OpenGL
    find_package(OpenGL)
    message(STATUS "Finding OpenGL ... ${OPENGL_FOUND}")

    # OpenCV
    # The following variables will be defined:
    #   OpenCV_FOUND
    #   OpenCV_INCLUDE_DIRS
    #   OpenCV_LIBS
    find_package(OpenCV)
    message(STATUS "Finding OpenCV ... ${OpenCV_FOUND}")
    if(OpenCV_FOUND)
      add_definitions("-DUSE_OPENCV")
    endif()

    add_definitions("-DUSE_MINGW")

  endif()

  #********************************************************************
  # for PLATFORM software emulation, off-line rendering
  #
  if(${PLATFORM} STREQUAL "software")
  endif()

endif()

if(USE_OPENMP)
  #********************************************************************
  # OpenMP
  # The following variables will be defined:
  #   OPENMP_FOUND
  #   OpenMP_C_FLAGS
  #   OpenMP_CXX_FLAGS
  find_package(OpenMP)
  message(STATUS "Finding OpenMP ... ${OPENMP_FOUND}")
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  endif()
endif()

# include path
include_directories(${AUTOGL_HEADER_DIR})
include_directories(${MT19937_HEADER_DIR})
include_directories(${XSADD_HEADER_DIR})

# library path
link_directories(${AUTOGL_LIBRARY_DIR})
link_directories(${MT19937_LIBRARY_DIR})
link_directories(${XSADD_LIBRARY_DIR})
if(${PLATFORM} STREQUAL "darwin")
  link_directories(${X11_LIBRARY_DIR})
endif(${PLATFORM} STREQUAL "darwin")

#----------------------------------------------------------------------
# source
set(MATES_UTIL_HDRS
  AmuLineSegment.h
  AmuConverter.h
  AmuMatrix.h
  AmuMatrix2D.h
  AmuMatrix3D.h
  AmuVector.h
  AmuInterval.h
  AmuPoint.h
  AmuPriorityQueue.h
  AmuStringOperator.h
  )

set(MATES_UTIL_SRCS
  AmuLineSegment.cpp
  AmuConverter.cpp
  AmuMatrix2D.cpp
  AmuMatrix3D.cpp
  AmuVector.cpp
  AmuInterval.cpp
  AmuPoint.cpp
  AmuStringOperator.cpp
  )

set(MATES_COMMON_SRCS
  AppMates.cpp
  Blinker.cpp
  Border.cpp
  Clocker.cpp
  Connector.cpp
  ConvoyMonitorIO.cpp
  Detector.cpp
  DetectorIO.cpp
  DetectorUnit.cpp
  OD.cpp
  GeneratingTable.cpp
  GeneratingTableCell.cpp
  GenerateVehicleController.cpp
  GenerateVehicleCounterIO.cpp
  GenerateVehicleIO.cpp
  GVInitializer.cpp
  GVManager.cpp
  FileManager.cpp
  Intersection.cpp
  IntersectionBuilder.cpp
  Lane.cpp
  LaneBundle.cpp
  LaneInIntersection.cpp
  LaneInSection.cpp
  LocalLaneRoute.cpp
  LocalLaneRouter.cpp
  NodeAStar.cpp
  ObjManager.cpp
  ODNode.cpp
  ODNodeGroup.cpp
  Parallel.cpp
  Partitioner.cpp
  PFAStar.cpp
  PFAStarHierarchy.cpp
  PFBase.cpp
  PFDijkstra.cpp
  PFLogger.cpp
  Random.cpp
  RelativeDirectionTable.cpp
  RoadEntity.cpp
  RoadMap.cpp
  RoadMapBuilder.cpp
  Route.cpp
  RouteIO.cpp
  RouteComponent.cpp
  RouteComponentKeyBase.cpp
  RouteComponentKeyHierarchy.cpp
  RouteStorage.cpp
  RoutingLink.cpp
  RoutingManager.cpp
  RoutingNetwork.cpp
  RoutingNetworkBuilder.cpp
  RoutingNode.cpp
  ScheduleManager.cpp
  Section.cpp
  SectionBuilder.cpp
  Signal.cpp
  SignalAspect.cpp
  SignalColor.cpp
  SignalControlData.cpp
  SignalControlDataSet.cpp
  SignalIO.cpp
  Simulator.cpp
  SpeedLimitCell.cpp
  SubIntersection.cpp
  SubSection.cpp
  TimeManager.cpp
  Tree.cpp
  Vehicle.cpp
  VehicleAccelDeterminer.cpp
  VehicleFamily.cpp
  VehicleFamilyIO.cpp
  VehicleFamilyManager.cpp
  VehicleIO.cpp
  VehicleLaneShifter.cpp
  VehicleRecognizer.cpp
  VehicleRunner.cpp
  VirtualLeader.cpp
  VirtualOccupant.cpp
  pedestrian/GeneratePedestrianController.cpp
  pedestrian/GeneratePedestrianIO.cpp
  pedestrian/IntersectionPedExt.cpp
  pedestrian/ObjManagerPedExt.cpp
  pedestrian/Pedestrian.cpp
  pedestrian/SimulatorPedExt.cpp
  pedestrian/VehiclePedExt.cpp
  pedestrian/Zebra.cpp
  tram/BorderTram.cpp
  tram/IntersectionTramExt.cpp
  tram/ObjManager.cpp
  tram/ODNodeTramExt.cpp
  tram/SectionTramExt.cpp
  tram/TramLaneInIntersection.cpp
  tram/TramLaneInSection.cpp
  tram/TramLine.cpp
  tram/TramLineManager.cpp
  tram/VehicleTram.cpp
  )

set(MATES_CALC_SRCS
  AppCalc.cpp
  )

set(MATES_SIM_SRCS
  AppSim.cpp
  Drawer.cpp
  GLColor.cpp
  Visualizer.cpp
  VisualizerDrawer.cpp
  VisualizerPanel.cpp
  VisualizerSimulationController.cpp
  VisualizerUserManipulation.cpp
  VisualizerViewController.cpp
  autogl_mates.c
  pedestrian/DrawerPedExt.cpp
  )

set(MATES_CALC_MAIN_SRC mainCalc.cpp)
set(MATES_SIM_MAIN_SRC  mainSim.cpp)

#----------------------------------------------------------------------
# build libraries
add_library(${LIBMATES} STATIC ${MATES_COMMON_SRCS})

if(BUILD_MATES_SIM)
  add_library(${LIBMATESGL} STATIC ${MATES_SIM_SRCS})
endif()

add_library(${LIBMATESUTIL} STATIC ${MATES_UTIL_SRCS})
add_custom_command(TARGET ${LIBMATESUTIL}
  POST_BUILD
  COMMAND mkdir -p ${MATES_HEADER_DIR}
  COMMAND cp -f  ${MATES_UTIL_HDRS} ${MATES_HEADER_DIR}
  COMMAND ln -fs ${CMAKE_CURRENT_SOURCE_DIR}/lib${LIBMATESUTIL}.a
  ${MATES_LIBRARY_DIR}/lib${LIBMATESUTIL}.a
  )

#----------------------------------------------------------------------
# build advmates-calc
if(BUILD_MATES_CALC)
  add_executable(${MATES_CALC}
    ${MATES_CALC_SRCS} ${MATES_CALC_MAIN_SRC})
  add_custom_command(TARGET ${MATES_CALC}
    PRE_LINK
    COMMAND make
    WORKING_DIRECTORY ${MT19937_LIBRARY_DIR})
  add_custom_command(TARGET ${MATES_CALC}
    PRE_LINK
    COMMAND make
    WORKING_DIRECTORY ${XSADD_LIBRARY_DIR})
  target_link_libraries(${MATES_CALC}
    ${LIBMATES} ${LIBMATESUTIL})
  target_link_libraries(${MATES_CALC}
    mt19937 xsadd m)
endif()

#----------------------------------------------------------------------
# install advmates-calc
install(TARGETS ${MATES_CALC} DESTINATION bin)

#----------------------------------------------------------------------
# build advmates-sim
if(BUILD_MATES_SIM)
  add_executable(${MATES_SIM}
    ${MATES_SIM_MAIN_SRC})
  add_custom_command(TARGET ${MATES_SIM}
    PRE_LINK
    COMMAND make
    WORKING_DIRECTORY ${MT19937_LIBRARY_DIR})
  add_custom_command(TARGET ${MATES_CALC}
    PRE_LINK
    COMMAND make
    WORKING_DIRECTORY ${XSADD_LIBRARY_DIR})
  add_custom_command(TARGET ${MATES_SIM}
    PRE_LINK
    COMMAND make
    WORKING_DIRECTORY ${AUTOGL_LIBRARY_DIR})
  target_link_libraries(${MATES_SIM}
    ${LIBMATES} ${LIBMATESGL} ${LIBMATESUTIL})
  target_link_libraries(${MATES_SIM} mt19937 xsadd autogl m)

  if(USE_AUTOGL_CONFIG)
    target_link_libraries(${MATES_SIM}
      ${AUTOGL_LIBS}
      )

  else()
    if(${PLATFORM} STREQUAL "linux")
      target_link_libraries(${MATES_SIM}
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBS}
        ${GTK2_LIBRARIES}
        ${X11_LIBRARIES}
        )

    elseif(${PLATFORM} STREQUAL "unix")
      target_link_libraries(${MATES_SIM}
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBS}
        ${MOTIF_LIBRARIES}
        ${X11_Xt_LIB}
        ${X11_Xmu_LIB}
        ${X11_Xext_LIB}
        ${X11_SM_LIB}
        ${X11_ICE_LIB}
        GLw
        )

    elseif(${PLATFORM} STREQUAL "darwin")
      set(EXTRA_LIBS GL X11 GLU glut)
      target_link_libraries(${MATES_SIM}
        ${GTK2_LIBRARIES}
        ${EXTRA_LIBS}
        )

    elseif(${PLATFORM} STREQUAL "cygwin")
      target_link_libraries(${MATES_SIM}
        glu32
        opengl32
        user32
        gdi32
        ${OpenCV_LIBS}
        )

    elseif(${PLATFORM} STREQUAL "mingw")
      target_link_libraries(${MATES_SIM}
        glu32
        opengl32
        user32
        gdi32
        ${OpenCV_LIBS}
        )

    elseif(${PLATFORM} STREQUAL "software")
      # nothing to do

    endif()
  endif()
endif()

#----------------------------------------------------------------------
# install advmates-sim
if(BUILD_MATES_SIM)
  install(TARGETS ${MATES_SIM} DESTINATION bin)
endif()

#----------------------------------------------------------------------
# clean target
set(CLEAN_TARGETS
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
  ${MATES_LIBRARY_DIR}/lib${LIBMATESUTIL}.a
  )

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_TARGETS}"
  )

#----------------------------------------------------------------------
# for flymake
get_directory_property(FM_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
foreach(FM_INCLUDE_DIRECTORY ${FM_INCLUDE_DIRECTORIES})
  set(FM_INCLUDES_TMP ${FM_INCLUDES_TMP} -I${FM_INCLUDE_DIRECTORY})
endforeach()
set(FM_INCLUDES ${FM_INCLUDES_TMP} CACHE STRING
  "Include directories for flymake")

get_directory_property(FM_DEFINITIONS COMPILE_DEFINITIONS)
foreach(FM_DEFINITION ${FM_DEFINITIONS})
  set(FM_FLAGS_TMP ${FM_FLAGS_TMP} -D${FM_DEFINITION})
endforeach()
if (DEFINED CMAKE_CXX_FLAGS)
  string(REPLACE " " ";" CXX_FLAGS_REP ${CMAKE_CXX_FLAGS})
  set(FM_FLAGS_TMP "${FM_FLAGS_TMP}${CXX_FLAGS_REP}")
endif()

set(FM_FLAGS ${FM_FLAGS_TMP} CACHE STRING
  "Compile flags for flymake")

add_custom_target(check-syntax
  ${CMAKE_CXX_COMPILER} ${FM_FLAGS} ${FM_INCLUDES}
  -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
  )

#----------------------------------------------------------------------
# for doxygen
set(MATES_DOXYGEN_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/../doc/doxygen")

# The following variables will be defined:
#   DOXYGEN_FOUND
#   DOXYGEN_DOT_FOUND
#   DOXYGEN_EXECUTABLE
find_package(Doxygen)
message(STATUS "Finding doxygen ... ${DOXYGEN_FOUND}")
if(DOXYGEN_FOUND)
  if(DOXYGEN_DOT_FOUND)
    set(MATES_DOXYGEN_CONF "${MATES_DOXYGEN_DIR}/advmates-dox")
  else()
    set(MATES_DOXYGEN_CONF "${MATES_DOXYGEN_DIR}/advmates-dox-min")
  endif()
  add_custom_target(dox
    COMMAND ${DOXYGEN_EXECUTABLE} ${MATES_DOXYGEN_CONF}
    WORKING_DIRECTORY ${MATES_DOXYGEN_DIR}
    )
endif()

#----------------------------------------------------------------------
# print status
message(STATUS "cmake status:")
message(STATUS "\tUSE_OPENMP                     : ${USE_OPENMP}")
message(STATUS "\tINCLUDE_VEHICLES               : ${INCLUDE_VEHICLES}")
message(STATUS "\tINCLUDE_TRAMS                  : ${INCLUDE_TRAMS}")
message(STATUS "\tINCLUDE_PEDESTRIANS            : ${INCLUDE_PEDESTRIANS}")
message(STATUS "\tGENERATE_VEHICLE_VELOCITY_ZERO : ${GENERATE_VEHICLE_VELOCITY_ZERO}")
message(STATUS "\tUNIQUE_NETWORK                 : ${UNIQUE_NETWORK}")
message(STATUS "\tMEASURE_TIME                   : ${MEASURE_TIME}")
message(STATUS "\tDEBUG                          : ${DEBUG}")
message(STATUS "FM_FLAGS : ${FM_FLAGS}")
message(STATUS "FM_INCLUDES : ${FM_INCLUDES}")

#----------------------------------------------------------------------
# clear cache for next execution
unset(PLATFORM CACHE)
unset(CMAKE_BUILD_TYPE CACHE)
unset(INSTALL_DIR CACHE)
