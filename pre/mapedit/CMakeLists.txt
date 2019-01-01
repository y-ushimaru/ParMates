#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################
#----------------------------------------------------------------------
#
# Map Editor for ADVENTURE_Mates
# (MATES : Multi-Agent-based Traffic and Environment Simulator)
#
#----------------------------------------------------------------------
# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message(STATUS "*** cmake: ${CMAKE_CURRENT_SOURCE_DIR} ***")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# make mapedit (default: true)
set(BUILD_MAPEDIT TRUE)
set(MATES_MAPEDIT "advmates-mapedit")

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
set(CMAKE_CXX_FLAGS_RELEASE   "-O3 -Wno-sign-compare")

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
set(MAPEDIT_SRCS
  app.cpp
  fileAccess.cpp
  intsec.cpp
  intsecDialog.cpp
  main.cpp
  mainwindow.cpp
  map.cpp
  mapframe.cpp
  road.cpp
  roadDialog.cpp
  setupDialog.cpp
  shMem.cpp
  testDataDialog.cpp
  )

# moc header
set(MAPEDIT_MOC_HDRS
  intsecDialog.h
  mapframe.h
  mainwindow.h
  roadDialog.h
  setupDialog.h
  testDataDialog.h
  )

# ui
set(MAPEDIT_UIS
  intsecDialog.ui
  mainwindow.ui
  roadDialog.ui
  setupDialog.ui
  testDataDialog.ui
  )

# resource file
set(MAPEDIT_QRCS
  mapedit.qrc
  )

#----------------------------------------------------------------------
#
# Qt5Widgets
#
# find_package(Qt5Widgets)
# message(STATUS "Finding Qt5Widgets ... ${Qt5Widgets_FOUND}")
# if(Qt5Widgets_FOUND)
#   include_directories(${Qt5Widgets_INCLUDE_DIRS})
#   add_definitions(${Qt5Widgets_DEFINITIONS})
#   add_definitions("-DUSE_QT5")
#   set(CMAKE_CXX_FLAGS
#     "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
#   qt5_wrap_cpp(MOC_FILES ${MAPEDIT_MOC_HDRS})
#   qt5_wrap_ui(UI_FILES ${MAPEDIT_UIS})
#   qt5_add_resources(QRC_FILES ${MAPEDIT_QRCS})

# else(Qt5Widgets_FOUND)
#
# Qt4
#
find_package(Qt4)
message(STATUS "Finding Qt4... ${QT4_FOUND}")
if(QT4_FOUND)
  add_definitions(-DUSE_QT4)
  qt4_wrap_cpp(MOC_FILES ${MAPEDIT_MOC_HDRS})
  qt4_wrap_ui(UI_FILES ${MAPEDIT_UIS})
  qt4_add_resources(QRC_FILES ${MAPEDIT_QRCS})
  include(${QT_USE_FILE})
else(QT4_FOUND)
  message(STATUS "Try to use qmake-qt4")
  set(QT_QMAKE_EXECUTABLE "qmake-qt4")
  find_package(Qt4)
  message(STATUS "Finding Qt4... ${QT4_FOUND}")
  if(QT4_FOUND)
    add_definitions(-DUSE_QT4)
    qt4_wrap_cpp(MOC_FILES ${MAPEDIT_MOC_HDRS})
    qt4_wrap_ui(UI_FILES ${MAPEDIT_UIS})
    qt4_add_resources(QRC_FILES ${MAPEDIT_QRCS})
    include(${QT_USE_FILE})
  else(QT4_FOUND)
    set(BUILD_MAP2MATES FALSE)
  endif(QT4_FOUND)
endif(QT4_FOUND)
#endif(Qt5Widgets_FOUND)

#----------------------------------------------------------------------
# build advmates-mapedit
if(BUILD_MAPEDIT)
    add_executable(${MATES_MAPEDIT}
      ${MAPEDIT_SRCS}
      ${MOC_FILES}
      ${UI_FILES}
      ${QRC_FILES})
    target_link_libraries(${MATES_MAPEDIT} mates-util)
    if(Qt5Widgets_FOUND)
      target_link_libraries(${MATES_MAPEDIT}
        ${Qt5Widgets_LIBRARIES}
        )
    elseif(QT4_FOUND)
      target_link_libraries(${MATES_MAPEDIT}
        ${QT_LIBRARIES}
        )
    endif(Qt5Widgets_FOUND)
endif(BUILD_MAPEDIT)

#----------------------------------------------------------------------
# install advmates-mapedit
install(TARGETS ${MATES_MAPEDIT} DESTINATION bin)


#----------------------------------------------------------------------
# clean target
set(CLEAN_TARGETS
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
  )
foreach(MOC_FILE ${MOC_FILES})
  set(CLEAN_TARGETS ${CLEAN_TARGETS} ${MOC_FILE})
endforeach(MOC_FILE ${MOC_FILES})
foreach(UI_FILE ${UI_FILES})
  set(CLEAN_TARGETS ${CLEAN_TARGETS} ${UI_FILE})
endforeach(UI_FILE ${UI_FILES})
foreach(MAPEDIT_QRC ${MAPEDIT_QRCS})
  set(CLEAN_TARGETS ${CLEAN_TARGETS} ${MAPEDIT_QRC}.depends)
endforeach(MAPEDIT_QRC ${MAPEDIT_QRCS})
foreach(QRC_FILE ${QRC_FILES})
  set(CLEAN_TARGETS ${CLEAN_TARGETS} ${QRC_FILE})
endforeach(QRC_FILE ${QRC_FILES})

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
