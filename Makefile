# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/y_ushimaru/ParMates

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/y_ushimaru/ParMates

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target list_install_components
list_install_components:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Available install components are: \"Unspecified\""
.PHONY : list_install_components

# Special rule for the target list_install_components
list_install_components/fast: list_install_components

.PHONY : list_install_components/fast

# Special rule for the target install
install: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install

# Special rule for the target install
install/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install/fast

# Special rule for the target install/local
install/local: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local

# Special rule for the target install/local
install/local/fast: install/local

.PHONY : install/local/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/y_ushimaru/ParMates/CMakeFiles /home/y_ushimaru/ParMates/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/y_ushimaru/ParMates/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named autogl

# Build rule for target.
autogl: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 autogl
.PHONY : autogl

# fast build rule for target.
autogl/fast:
	$(MAKE) -f lib/autogl/CMakeFiles/autogl.dir/build.make lib/autogl/CMakeFiles/autogl.dir/build
.PHONY : autogl/fast

#=============================================================================
# Target rules for targets named mt19937

# Build rule for target.
mt19937: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 mt19937
.PHONY : mt19937

# fast build rule for target.
mt19937/fast:
	$(MAKE) -f lib/mt19937/CMakeFiles/mt19937.dir/build.make lib/mt19937/CMakeFiles/mt19937.dir/build
.PHONY : mt19937/fast

#=============================================================================
# Target rules for targets named test_xsadd

# Build rule for target.
test_xsadd: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_xsadd
.PHONY : test_xsadd

# fast build rule for target.
test_xsadd/fast:
	$(MAKE) -f lib/XSadd/CMakeFiles/test_xsadd.dir/build.make lib/XSadd/CMakeFiles/test_xsadd.dir/build
.PHONY : test_xsadd/fast

#=============================================================================
# Target rules for targets named testexec

# Build rule for target.
testexec: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 testexec
.PHONY : testexec

# fast build rule for target.
testexec/fast:
	$(MAKE) -f lib/XSadd/CMakeFiles/testexec.dir/build.make lib/XSadd/CMakeFiles/testexec.dir/build
.PHONY : testexec/fast

#=============================================================================
# Target rules for targets named xsadd

# Build rule for target.
xsadd: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 xsadd
.PHONY : xsadd

# fast build rule for target.
xsadd/fast:
	$(MAKE) -f lib/XSadd/CMakeFiles/xsadd.dir/build.make lib/XSadd/CMakeFiles/xsadd.dir/build
.PHONY : xsadd/fast

#=============================================================================
# Target rules for targets named mates

# Build rule for target.
mates: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 mates
.PHONY : mates

# fast build rule for target.
mates/fast:
	$(MAKE) -f solver/CMakeFiles/mates.dir/build.make solver/CMakeFiles/mates.dir/build
.PHONY : mates/fast

#=============================================================================
# Target rules for targets named matesGL

# Build rule for target.
matesGL: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 matesGL
.PHONY : matesGL

# fast build rule for target.
matesGL/fast:
	$(MAKE) -f solver/CMakeFiles/matesGL.dir/build.make solver/CMakeFiles/matesGL.dir/build
.PHONY : matesGL/fast

#=============================================================================
# Target rules for targets named advmates-sim

# Build rule for target.
advmates-sim: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 advmates-sim
.PHONY : advmates-sim

# fast build rule for target.
advmates-sim/fast:
	$(MAKE) -f solver/CMakeFiles/advmates-sim.dir/build.make solver/CMakeFiles/advmates-sim.dir/build
.PHONY : advmates-sim/fast

# Manual pre-install relink rule for target.
advmates-sim/preinstall:
	$(MAKE) -f solver/CMakeFiles/advmates-sim.dir/build.make solver/CMakeFiles/advmates-sim.dir/preinstall
.PHONY : advmates-sim/preinstall

#=============================================================================
# Target rules for targets named mates-util

# Build rule for target.
mates-util: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 mates-util
.PHONY : mates-util

# fast build rule for target.
mates-util/fast:
	$(MAKE) -f solver/CMakeFiles/mates-util.dir/build.make solver/CMakeFiles/mates-util.dir/build
.PHONY : mates-util/fast

#=============================================================================
# Target rules for targets named advmates-calc

# Build rule for target.
advmates-calc: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 advmates-calc
.PHONY : advmates-calc

# fast build rule for target.
advmates-calc/fast:
	$(MAKE) -f solver/CMakeFiles/advmates-calc.dir/build.make solver/CMakeFiles/advmates-calc.dir/build
.PHONY : advmates-calc/fast

# Manual pre-install relink rule for target.
advmates-calc/preinstall:
	$(MAKE) -f solver/CMakeFiles/advmates-calc.dir/build.make solver/CMakeFiles/advmates-calc.dir/preinstall
.PHONY : advmates-calc/preinstall

#=============================================================================
# Target rules for targets named check-syntax

# Build rule for target.
check-syntax: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 check-syntax
.PHONY : check-syntax

# fast build rule for target.
check-syntax/fast:
	$(MAKE) -f solver/CMakeFiles/check-syntax.dir/build.make solver/CMakeFiles/check-syntax.dir/build
.PHONY : check-syntax/fast

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... list_install_components"
	@echo "... install"
	@echo "... install/local"
	@echo "... autogl"
	@echo "... mt19937"
	@echo "... test_xsadd"
	@echo "... testexec"
	@echo "... xsadd"
	@echo "... mates"
	@echo "... matesGL"
	@echo "... advmates-sim"
	@echo "... mates-util"
	@echo "... advmates-calc"
	@echo "... check-syntax"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

