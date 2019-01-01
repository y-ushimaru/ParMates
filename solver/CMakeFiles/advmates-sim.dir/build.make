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
CMAKE_SOURCE_DIR = /home/y_ushimaru/ParMates/solver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/y_ushimaru/ParMates/solver

# Include any dependencies generated for this target.
include CMakeFiles/advmates-sim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/advmates-sim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/advmates-sim.dir/flags.make

CMakeFiles/advmates-sim.dir/mainSim.cpp.o: CMakeFiles/advmates-sim.dir/flags.make
CMakeFiles/advmates-sim.dir/mainSim.cpp.o: mainSim.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/y_ushimaru/ParMates/solver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/advmates-sim.dir/mainSim.cpp.o"
	mpicxx   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/advmates-sim.dir/mainSim.cpp.o -c /home/y_ushimaru/ParMates/solver/mainSim.cpp

CMakeFiles/advmates-sim.dir/mainSim.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/advmates-sim.dir/mainSim.cpp.i"
	mpicxx  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/y_ushimaru/ParMates/solver/mainSim.cpp > CMakeFiles/advmates-sim.dir/mainSim.cpp.i

CMakeFiles/advmates-sim.dir/mainSim.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/advmates-sim.dir/mainSim.cpp.s"
	mpicxx  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/y_ushimaru/ParMates/solver/mainSim.cpp -o CMakeFiles/advmates-sim.dir/mainSim.cpp.s

CMakeFiles/advmates-sim.dir/mainSim.cpp.o.requires:

.PHONY : CMakeFiles/advmates-sim.dir/mainSim.cpp.o.requires

CMakeFiles/advmates-sim.dir/mainSim.cpp.o.provides: CMakeFiles/advmates-sim.dir/mainSim.cpp.o.requires
	$(MAKE) -f CMakeFiles/advmates-sim.dir/build.make CMakeFiles/advmates-sim.dir/mainSim.cpp.o.provides.build
.PHONY : CMakeFiles/advmates-sim.dir/mainSim.cpp.o.provides

CMakeFiles/advmates-sim.dir/mainSim.cpp.o.provides.build: CMakeFiles/advmates-sim.dir/mainSim.cpp.o


# Object files for target advmates-sim
advmates__sim_OBJECTS = \
"CMakeFiles/advmates-sim.dir/mainSim.cpp.o"

# External object files for target advmates-sim
advmates__sim_EXTERNAL_OBJECTS =

advmates-sim: CMakeFiles/advmates-sim.dir/mainSim.cpp.o
advmates-sim: CMakeFiles/advmates-sim.dir/build.make
advmates-sim: /usr/lib64/libz.so
advmates-sim: libmates.a
advmates-sim: libmatesGL.a
advmates-sim: libmates-util.a
advmates-sim: /usr/lib64/libGLU.so
advmates-sim: /usr/lib64/libGL.so
advmates-sim: /usr/lib64/libglib-2.0.so
advmates-sim: /usr/lib64/libgobject-2.0.so
advmates-sim: /usr/lib64/libatk-1.0.so
advmates-sim: /usr/lib64/libgio-2.0.so
advmates-sim: /usr/lib64/libgthread-2.0.so
advmates-sim: /usr/lib64/libgmodule-2.0.so
advmates-sim: /usr/lib64/libgdk_pixbuf-2.0.so
advmates-sim: /usr/lib64/libcairo.so
advmates-sim: /usr/lib64/libpango-1.0.so
advmates-sim: /usr/lib64/libpangocairo-1.0.so
advmates-sim: /usr/lib64/libpangoft2-1.0.so
advmates-sim: /usr/lib64/libpangoxft-1.0.so
advmates-sim: /usr/lib64/libgdk-x11-2.0.so
advmates-sim: /usr/lib64/libgtk-x11-2.0.so
advmates-sim: /usr/lib64/libX11.so
advmates-sim: /usr/lib64/libXext.so
advmates-sim: /usr/lib64/libz.so
advmates-sim: CMakeFiles/advmates-sim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/y_ushimaru/ParMates/solver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable advmates-sim"
	cd /home/y_ushimaru/ParMates/lib/mt19937 && make
	cd /home/y_ushimaru/ParMates/lib/autogl && make
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/advmates-sim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/advmates-sim.dir/build: advmates-sim

.PHONY : CMakeFiles/advmates-sim.dir/build

# Object files for target advmates-sim
advmates__sim_OBJECTS = \
"CMakeFiles/advmates-sim.dir/mainSim.cpp.o"

# External object files for target advmates-sim
advmates__sim_EXTERNAL_OBJECTS =

CMakeFiles/CMakeRelink.dir/advmates-sim: CMakeFiles/advmates-sim.dir/mainSim.cpp.o
CMakeFiles/CMakeRelink.dir/advmates-sim: CMakeFiles/advmates-sim.dir/build.make
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libz.so
CMakeFiles/CMakeRelink.dir/advmates-sim: libmates.a
CMakeFiles/CMakeRelink.dir/advmates-sim: libmatesGL.a
CMakeFiles/CMakeRelink.dir/advmates-sim: libmates-util.a
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libGLU.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libGL.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libglib-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgobject-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libatk-1.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgio-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgthread-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgmodule-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgdk_pixbuf-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libcairo.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libpango-1.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libpangocairo-1.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libpangoft2-1.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libpangoxft-1.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgdk-x11-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libgtk-x11-2.0.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libX11.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libXext.so
CMakeFiles/CMakeRelink.dir/advmates-sim: /usr/lib64/libz.so
CMakeFiles/CMakeRelink.dir/advmates-sim: CMakeFiles/advmates-sim.dir/relink.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/y_ushimaru/ParMates/solver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable CMakeFiles/CMakeRelink.dir/advmates-sim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/advmates-sim.dir/relink.txt --verbose=$(VERBOSE)

# Rule to relink during preinstall.
CMakeFiles/advmates-sim.dir/preinstall: CMakeFiles/CMakeRelink.dir/advmates-sim

.PHONY : CMakeFiles/advmates-sim.dir/preinstall

CMakeFiles/advmates-sim.dir/requires: CMakeFiles/advmates-sim.dir/mainSim.cpp.o.requires

.PHONY : CMakeFiles/advmates-sim.dir/requires

CMakeFiles/advmates-sim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/advmates-sim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/advmates-sim.dir/clean

CMakeFiles/advmates-sim.dir/depend:
	cd /home/y_ushimaru/ParMates/solver && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/y_ushimaru/ParMates/solver /home/y_ushimaru/ParMates/solver /home/y_ushimaru/ParMates/solver /home/y_ushimaru/ParMates/solver /home/y_ushimaru/ParMates/solver/CMakeFiles/advmates-sim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/advmates-sim.dir/depend
