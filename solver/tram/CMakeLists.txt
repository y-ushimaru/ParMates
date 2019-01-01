#####################################################
# Copyright (C) 2014 ADVENTURE Project
# All Rights Reserved
#####################################################

#**********************************************************************
# This configuration is only for creating check-syntax target
#**********************************************************************

# cmake version
cmake_minimum_required(VERSION 2.6)

# start cmake
message("***** cmake: ${CMAKE_CURRENT_SOURCE_DIR} *****")
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# load cache
load_cache(../../)
load_cache(../)

# compiler
set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

#----------------------------------------------------------------------
# for flymake
add_custom_target(check-syntax
  ${CMAKE_CXX_COMPILER} ${FM_FLAGS} ${FM_INCLUDES}
  -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
  )
