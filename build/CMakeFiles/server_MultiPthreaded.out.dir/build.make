# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/networkProgramming

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/networkProgramming/build

# Include any dependencies generated for this target.
include CMakeFiles/server_MultiPthreaded.out.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/server_MultiPthreaded.out.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server_MultiPthreaded.out.dir/flags.make

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o: CMakeFiles/server_MultiPthreaded.out.dir/flags.make
CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o: ../src/server_MultiPthreaded.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/networkProgramming/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o   -c /home/networkProgramming/src/server_MultiPthreaded.c

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/networkProgramming/src/server_MultiPthreaded.c > CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.i

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/networkProgramming/src/server_MultiPthreaded.c -o CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.s

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.requires:

.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.requires

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.provides: CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.requires
	$(MAKE) -f CMakeFiles/server_MultiPthreaded.out.dir/build.make CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.provides.build
.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.provides

CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.provides.build: CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o


# Object files for target server_MultiPthreaded.out
server_MultiPthreaded_out_OBJECTS = \
"CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o"

# External object files for target server_MultiPthreaded.out
server_MultiPthreaded_out_EXTERNAL_OBJECTS =

../server_MultiPthreaded.out: CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o
../server_MultiPthreaded.out: CMakeFiles/server_MultiPthreaded.out.dir/build.make
../server_MultiPthreaded.out: CMakeFiles/server_MultiPthreaded.out.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/networkProgramming/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../server_MultiPthreaded.out"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_MultiPthreaded.out.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server_MultiPthreaded.out.dir/build: ../server_MultiPthreaded.out

.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/build

CMakeFiles/server_MultiPthreaded.out.dir/requires: CMakeFiles/server_MultiPthreaded.out.dir/src/server_MultiPthreaded.c.o.requires

.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/requires

CMakeFiles/server_MultiPthreaded.out.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server_MultiPthreaded.out.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/clean

CMakeFiles/server_MultiPthreaded.out.dir/depend:
	cd /home/networkProgramming/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/networkProgramming /home/networkProgramming /home/networkProgramming/build /home/networkProgramming/build /home/networkProgramming/build/CMakeFiles/server_MultiPthreaded.out.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server_MultiPthreaded.out.dir/depend

