# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build

# Include any dependencies generated for this target.
include src/CMakeFiles/raptor_term_test.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/raptor_term_test.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/raptor_term_test.dir/flags.make

src/CMakeFiles/raptor_term_test.dir/raptor_term.c.o: src/CMakeFiles/raptor_term_test.dir/flags.make
src/CMakeFiles/raptor_term_test.dir/raptor_term.c.o: ../src/raptor_term.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/raptor_term_test.dir/raptor_term.c.o"
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/raptor_term_test.dir/raptor_term.c.o   -c /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/src/raptor_term.c

src/CMakeFiles/raptor_term_test.dir/raptor_term.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/raptor_term_test.dir/raptor_term.c.i"
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/src/raptor_term.c > CMakeFiles/raptor_term_test.dir/raptor_term.c.i

src/CMakeFiles/raptor_term_test.dir/raptor_term.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/raptor_term_test.dir/raptor_term.c.s"
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/src/raptor_term.c -o CMakeFiles/raptor_term_test.dir/raptor_term.c.s

# Object files for target raptor_term_test
raptor_term_test_OBJECTS = \
"CMakeFiles/raptor_term_test.dir/raptor_term.c.o"

# External object files for target raptor_term_test
raptor_term_test_EXTERNAL_OBJECTS =

src/raptor_term_test: src/CMakeFiles/raptor_term_test.dir/raptor_term.c.o
src/raptor_term_test: src/CMakeFiles/raptor_term_test.dir/build.make
src/raptor_term_test: src/libraptor2.a
src/raptor_term_test: /usr/lib/x86_64-linux-gnu/libxml2.so
src/raptor_term_test: src/CMakeFiles/raptor_term_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable raptor_term_test"
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/raptor_term_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/raptor_term_test.dir/build: src/raptor_term_test

.PHONY : src/CMakeFiles/raptor_term_test.dir/build

src/CMakeFiles/raptor_term_test.dir/clean:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src && $(CMAKE_COMMAND) -P CMakeFiles/raptor_term_test.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/raptor_term_test.dir/clean

src/CMakeFiles/raptor_term_test.dir/depend:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/src /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src/CMakeFiles/raptor_term_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/raptor_term_test.dir/depend

