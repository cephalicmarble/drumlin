# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/amsc/data/pleg/drumlin

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/amsc/data/pleg/drumlin

# Include any dependencies generated for this target.
include CMakeFiles/apptest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/apptest.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/apptest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/apptest.dir/flags.make

CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o: CMakeFiles/apptest.dir/flags.make
CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o: tests/apptest/apptest.cpp
CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o: CMakeFiles/apptest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/amsc/data/pleg/drumlin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o -MF CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o.d -o CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o -c /home/amsc/data/pleg/drumlin/tests/apptest/apptest.cpp

CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/amsc/data/pleg/drumlin/tests/apptest/apptest.cpp > CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.i

CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/amsc/data/pleg/drumlin/tests/apptest/apptest.cpp -o CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.s

# Object files for target apptest
apptest_OBJECTS = \
"CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o"

# External object files for target apptest
apptest_EXTERNAL_OBJECTS =

apptest: CMakeFiles/apptest.dir/tests/apptest/apptest.cpp.o
apptest: CMakeFiles/apptest.dir/build.make
apptest: /usr/lib/libboost_system.so.1.76.0
apptest: /usr/lib/libboost_date_time.so.1.76.0
apptest: /usr/lib/libboost_log.so.1.76.0
apptest: /usr/lib/libboost_program_options.so.1.76.0
apptest: /usr/lib/libboost_regex.so.1.76.0
apptest: /usr/lib/libboost_chrono.so.1.76.0
apptest: /usr/lib/libboost_filesystem.so.1.76.0
apptest: /usr/lib/libboost_atomic.so.1.76.0
apptest: /usr/lib/libboost_thread.so.1.76.0
apptest: CMakeFiles/apptest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/amsc/data/pleg/drumlin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable apptest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/apptest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/apptest.dir/build: apptest
.PHONY : CMakeFiles/apptest.dir/build

CMakeFiles/apptest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/apptest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/apptest.dir/clean

CMakeFiles/apptest.dir/depend:
	cd /home/amsc/data/pleg/drumlin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/amsc/data/pleg/drumlin /home/amsc/data/pleg/drumlin /home/amsc/data/pleg/drumlin /home/amsc/data/pleg/drumlin /home/amsc/data/pleg/drumlin/CMakeFiles/apptest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/apptest.dir/depend
