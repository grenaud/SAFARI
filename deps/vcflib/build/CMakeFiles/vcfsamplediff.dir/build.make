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
CMAKE_SOURCE_DIR = /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build

# Include any dependencies generated for this target.
include CMakeFiles/vcfsamplediff.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vcfsamplediff.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vcfsamplediff.dir/flags.make

CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o: CMakeFiles/vcfsamplediff.dir/flags.make
CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o: ../src/vcfsamplediff.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o -c /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcfsamplediff.cpp

CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcfsamplediff.cpp > CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.i

CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcfsamplediff.cpp -o CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.s

# Object files for target vcfsamplediff
vcfsamplediff_OBJECTS = \
"CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o"

# External object files for target vcfsamplediff
vcfsamplediff_EXTERNAL_OBJECTS =

vcfsamplediff: CMakeFiles/vcfsamplediff.dir/src/vcfsamplediff.cpp.o
vcfsamplediff: CMakeFiles/vcfsamplediff.dir/build.make
vcfsamplediff: libvcflib.a
vcfsamplediff: /usr/lib/x86_64-linux-gnu/libz.so
vcfsamplediff: /usr/lib/x86_64-linux-gnu/liblzma.so
vcfsamplediff: /usr/lib/x86_64-linux-gnu/libbz2.so
vcfsamplediff: CMakeFiles/vcfsamplediff.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vcfsamplediff"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vcfsamplediff.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vcfsamplediff.dir/build: vcfsamplediff

.PHONY : CMakeFiles/vcfsamplediff.dir/build

CMakeFiles/vcfsamplediff.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vcfsamplediff.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vcfsamplediff.dir/clean

CMakeFiles/vcfsamplediff.dir/depend:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles/vcfsamplediff.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vcfsamplediff.dir/depend

