# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_SOURCE_DIR = /home/dawid/Public/studia/rownolegle

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dawid/Public/studia/rownolegle/build

# Include any dependencies generated for this target.
include CMakeFiles/rownolegle.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/rownolegle.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/rownolegle.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rownolegle.dir/flags.make

CMakeFiles/rownolegle.dir/src/main.cpp.o: CMakeFiles/rownolegle.dir/flags.make
CMakeFiles/rownolegle.dir/src/main.cpp.o: /home/dawid/Public/studia/rownolegle/src/main.cpp
CMakeFiles/rownolegle.dir/src/main.cpp.o: CMakeFiles/rownolegle.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dawid/Public/studia/rownolegle/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rownolegle.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/rownolegle.dir/src/main.cpp.o -MF CMakeFiles/rownolegle.dir/src/main.cpp.o.d -o CMakeFiles/rownolegle.dir/src/main.cpp.o -c /home/dawid/Public/studia/rownolegle/src/main.cpp

CMakeFiles/rownolegle.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/rownolegle.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dawid/Public/studia/rownolegle/src/main.cpp > CMakeFiles/rownolegle.dir/src/main.cpp.i

CMakeFiles/rownolegle.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/rownolegle.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dawid/Public/studia/rownolegle/src/main.cpp -o CMakeFiles/rownolegle.dir/src/main.cpp.s

CMakeFiles/rownolegle.dir/src/algorithm.cpp.o: CMakeFiles/rownolegle.dir/flags.make
CMakeFiles/rownolegle.dir/src/algorithm.cpp.o: /home/dawid/Public/studia/rownolegle/src/algorithm.cpp
CMakeFiles/rownolegle.dir/src/algorithm.cpp.o: CMakeFiles/rownolegle.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dawid/Public/studia/rownolegle/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/rownolegle.dir/src/algorithm.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/rownolegle.dir/src/algorithm.cpp.o -MF CMakeFiles/rownolegle.dir/src/algorithm.cpp.o.d -o CMakeFiles/rownolegle.dir/src/algorithm.cpp.o -c /home/dawid/Public/studia/rownolegle/src/algorithm.cpp

CMakeFiles/rownolegle.dir/src/algorithm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/rownolegle.dir/src/algorithm.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dawid/Public/studia/rownolegle/src/algorithm.cpp > CMakeFiles/rownolegle.dir/src/algorithm.cpp.i

CMakeFiles/rownolegle.dir/src/algorithm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/rownolegle.dir/src/algorithm.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dawid/Public/studia/rownolegle/src/algorithm.cpp -o CMakeFiles/rownolegle.dir/src/algorithm.cpp.s

CMakeFiles/rownolegle.dir/src/database_queries.cpp.o: CMakeFiles/rownolegle.dir/flags.make
CMakeFiles/rownolegle.dir/src/database_queries.cpp.o: /home/dawid/Public/studia/rownolegle/src/database_queries.cpp
CMakeFiles/rownolegle.dir/src/database_queries.cpp.o: CMakeFiles/rownolegle.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dawid/Public/studia/rownolegle/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/rownolegle.dir/src/database_queries.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/rownolegle.dir/src/database_queries.cpp.o -MF CMakeFiles/rownolegle.dir/src/database_queries.cpp.o.d -o CMakeFiles/rownolegle.dir/src/database_queries.cpp.o -c /home/dawid/Public/studia/rownolegle/src/database_queries.cpp

CMakeFiles/rownolegle.dir/src/database_queries.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/rownolegle.dir/src/database_queries.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dawid/Public/studia/rownolegle/src/database_queries.cpp > CMakeFiles/rownolegle.dir/src/database_queries.cpp.i

CMakeFiles/rownolegle.dir/src/database_queries.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/rownolegle.dir/src/database_queries.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dawid/Public/studia/rownolegle/src/database_queries.cpp -o CMakeFiles/rownolegle.dir/src/database_queries.cpp.s

# Object files for target rownolegle
rownolegle_OBJECTS = \
"CMakeFiles/rownolegle.dir/src/main.cpp.o" \
"CMakeFiles/rownolegle.dir/src/algorithm.cpp.o" \
"CMakeFiles/rownolegle.dir/src/database_queries.cpp.o"

# External object files for target rownolegle
rownolegle_EXTERNAL_OBJECTS =

rownolegle: CMakeFiles/rownolegle.dir/src/main.cpp.o
rownolegle: CMakeFiles/rownolegle.dir/src/algorithm.cpp.o
rownolegle: CMakeFiles/rownolegle.dir/src/database_queries.cpp.o
rownolegle: CMakeFiles/rownolegle.dir/build.make
rownolegle: /usr/lib64/libcurl.so
rownolegle: CMakeFiles/rownolegle.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/dawid/Public/studia/rownolegle/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable rownolegle"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rownolegle.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/rownolegle.dir/build: rownolegle
.PHONY : CMakeFiles/rownolegle.dir/build

CMakeFiles/rownolegle.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rownolegle.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rownolegle.dir/clean

CMakeFiles/rownolegle.dir/depend:
	cd /home/dawid/Public/studia/rownolegle/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dawid/Public/studia/rownolegle /home/dawid/Public/studia/rownolegle /home/dawid/Public/studia/rownolegle/build /home/dawid/Public/studia/rownolegle/build /home/dawid/Public/studia/rownolegle/build/CMakeFiles/rownolegle.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/rownolegle.dir/depend

