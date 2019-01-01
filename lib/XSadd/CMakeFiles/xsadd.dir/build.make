# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# Include any dependencies generated for this target.
include lib/XSadd/CMakeFiles/xsadd.dir/depend.make

# Include the progress variables for this target.
include lib/XSadd/CMakeFiles/xsadd.dir/progress.make

# Include the compile flags for this target's objects.
include lib/XSadd/CMakeFiles/xsadd.dir/flags.make

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o: lib/XSadd/CMakeFiles/xsadd.dir/flags.make
lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o: lib/XSadd/xsadd.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/y_ushimaru/ParMates/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o"
	cd /home/y_ushimaru/ParMates/lib/XSadd && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/xsadd.dir/xsadd.c.o   -c /home/y_ushimaru/ParMates/lib/XSadd/xsadd.c

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/xsadd.dir/xsadd.c.i"
	cd /home/y_ushimaru/ParMates/lib/XSadd && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/y_ushimaru/ParMates/lib/XSadd/xsadd.c > CMakeFiles/xsadd.dir/xsadd.c.i

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/xsadd.dir/xsadd.c.s"
	cd /home/y_ushimaru/ParMates/lib/XSadd && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/y_ushimaru/ParMates/lib/XSadd/xsadd.c -o CMakeFiles/xsadd.dir/xsadd.c.s

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.requires:

.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.requires

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.provides: lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.requires
	$(MAKE) -f lib/XSadd/CMakeFiles/xsadd.dir/build.make lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.provides.build
.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.provides

lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.provides.build: lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o


# Object files for target xsadd
xsadd_OBJECTS = \
"CMakeFiles/xsadd.dir/xsadd.c.o"

# External object files for target xsadd
xsadd_EXTERNAL_OBJECTS =

lib/XSadd/libxsadd.a: lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o
lib/XSadd/libxsadd.a: lib/XSadd/CMakeFiles/xsadd.dir/build.make
lib/XSadd/libxsadd.a: lib/XSadd/CMakeFiles/xsadd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/y_ushimaru/ParMates/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libxsadd.a"
	cd /home/y_ushimaru/ParMates/lib/XSadd && $(CMAKE_COMMAND) -P CMakeFiles/xsadd.dir/cmake_clean_target.cmake
	cd /home/y_ushimaru/ParMates/lib/XSadd && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/xsadd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/XSadd/CMakeFiles/xsadd.dir/build: lib/XSadd/libxsadd.a

.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/build

lib/XSadd/CMakeFiles/xsadd.dir/requires: lib/XSadd/CMakeFiles/xsadd.dir/xsadd.c.o.requires

.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/requires

lib/XSadd/CMakeFiles/xsadd.dir/clean:
	cd /home/y_ushimaru/ParMates/lib/XSadd && $(CMAKE_COMMAND) -P CMakeFiles/xsadd.dir/cmake_clean.cmake
.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/clean

lib/XSadd/CMakeFiles/xsadd.dir/depend:
	cd /home/y_ushimaru/ParMates && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/y_ushimaru/ParMates /home/y_ushimaru/ParMates/lib/XSadd /home/y_ushimaru/ParMates /home/y_ushimaru/ParMates/lib/XSadd /home/y_ushimaru/ParMates/lib/XSadd/CMakeFiles/xsadd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/XSadd/CMakeFiles/xsadd.dir/depend
