# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/alexis/CLionProjects/fixedbigint

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/alexis/CLionProjects/fixedbigint/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/fixedbigint.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/fixedbigint.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fixedbigint.dir/flags.make

CMakeFiles/fixedbigint.dir/main.c.o: CMakeFiles/fixedbigint.dir/flags.make
CMakeFiles/fixedbigint.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/fixedbigint.dir/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fixedbigint.dir/main.c.o   -c /Users/alexis/CLionProjects/fixedbigint/main.c

CMakeFiles/fixedbigint.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fixedbigint.dir/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/alexis/CLionProjects/fixedbigint/main.c > CMakeFiles/fixedbigint.dir/main.c.i

CMakeFiles/fixedbigint.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fixedbigint.dir/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/alexis/CLionProjects/fixedbigint/main.c -o CMakeFiles/fixedbigint.dir/main.c.s

CMakeFiles/fixedbigint.dir/bi32.c.o: CMakeFiles/fixedbigint.dir/flags.make
CMakeFiles/fixedbigint.dir/bi32.c.o: ../bi32.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/fixedbigint.dir/bi32.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fixedbigint.dir/bi32.c.o   -c /Users/alexis/CLionProjects/fixedbigint/bi32.c

CMakeFiles/fixedbigint.dir/bi32.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fixedbigint.dir/bi32.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/alexis/CLionProjects/fixedbigint/bi32.c > CMakeFiles/fixedbigint.dir/bi32.c.i

CMakeFiles/fixedbigint.dir/bi32.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fixedbigint.dir/bi32.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/alexis/CLionProjects/fixedbigint/bi32.c -o CMakeFiles/fixedbigint.dir/bi32.c.s

CMakeFiles/fixedbigint.dir/lenstra.c.o: CMakeFiles/fixedbigint.dir/flags.make
CMakeFiles/fixedbigint.dir/lenstra.c.o: ../lenstra.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/fixedbigint.dir/lenstra.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fixedbigint.dir/lenstra.c.o   -c /Users/alexis/CLionProjects/fixedbigint/lenstra.c

CMakeFiles/fixedbigint.dir/lenstra.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fixedbigint.dir/lenstra.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/alexis/CLionProjects/fixedbigint/lenstra.c > CMakeFiles/fixedbigint.dir/lenstra.c.i

CMakeFiles/fixedbigint.dir/lenstra.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fixedbigint.dir/lenstra.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/alexis/CLionProjects/fixedbigint/lenstra.c -o CMakeFiles/fixedbigint.dir/lenstra.c.s

CMakeFiles/fixedbigint.dir/bi63t.c.o: CMakeFiles/fixedbigint.dir/flags.make
CMakeFiles/fixedbigint.dir/bi63t.c.o: ../bi63t.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/fixedbigint.dir/bi63t.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fixedbigint.dir/bi63t.c.o   -c /Users/alexis/CLionProjects/fixedbigint/bi63t.c

CMakeFiles/fixedbigint.dir/bi63t.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fixedbigint.dir/bi63t.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/alexis/CLionProjects/fixedbigint/bi63t.c > CMakeFiles/fixedbigint.dir/bi63t.c.i

CMakeFiles/fixedbigint.dir/bi63t.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fixedbigint.dir/bi63t.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/alexis/CLionProjects/fixedbigint/bi63t.c -o CMakeFiles/fixedbigint.dir/bi63t.c.s

# Object files for target fixedbigint
fixedbigint_OBJECTS = \
"CMakeFiles/fixedbigint.dir/main.c.o" \
"CMakeFiles/fixedbigint.dir/bi32.c.o" \
"CMakeFiles/fixedbigint.dir/lenstra.c.o" \
"CMakeFiles/fixedbigint.dir/bi63t.c.o"

# External object files for target fixedbigint
fixedbigint_EXTERNAL_OBJECTS =

fixedbigint: CMakeFiles/fixedbigint.dir/main.c.o
fixedbigint: CMakeFiles/fixedbigint.dir/bi32.c.o
fixedbigint: CMakeFiles/fixedbigint.dir/lenstra.c.o
fixedbigint: CMakeFiles/fixedbigint.dir/bi63t.c.o
fixedbigint: CMakeFiles/fixedbigint.dir/build.make
fixedbigint: CMakeFiles/fixedbigint.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable fixedbigint"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fixedbigint.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fixedbigint.dir/build: fixedbigint

.PHONY : CMakeFiles/fixedbigint.dir/build

CMakeFiles/fixedbigint.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fixedbigint.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fixedbigint.dir/clean

CMakeFiles/fixedbigint.dir/depend:
	cd /Users/alexis/CLionProjects/fixedbigint/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/alexis/CLionProjects/fixedbigint /Users/alexis/CLionProjects/fixedbigint /Users/alexis/CLionProjects/fixedbigint/cmake-build-debug /Users/alexis/CLionProjects/fixedbigint/cmake-build-debug /Users/alexis/CLionProjects/fixedbigint/cmake-build-debug/CMakeFiles/fixedbigint.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/fixedbigint.dir/depend

