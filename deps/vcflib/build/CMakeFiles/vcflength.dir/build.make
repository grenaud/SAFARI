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
include CMakeFiles/vcflength.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vcflength.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vcflength.dir/flags.make

CMakeFiles/vcflength.dir/src/vcflength.cpp.o: CMakeFiles/vcflength.dir/flags.make
CMakeFiles/vcflength.dir/src/vcflength.cpp.o: ../src/vcflength.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/vcflength.dir/src/vcflength.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vcflength.dir/src/vcflength.cpp.o -c /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcflength.cpp

CMakeFiles/vcflength.dir/src/vcflength.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcflength.dir/src/vcflength.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcflength.cpp > CMakeFiles/vcflength.dir/src/vcflength.cpp.i

CMakeFiles/vcflength.dir/src/vcflength.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcflength.dir/src/vcflength.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/vcflength.cpp -o CMakeFiles/vcflength.dir/src/vcflength.cpp.s

# Object files for target vcflength
vcflength_OBJECTS = \
"CMakeFiles/vcflength.dir/src/vcflength.cpp.o"

# External object files for target vcflength
vcflength_EXTERNAL_OBJECTS =

vcflength: CMakeFiles/vcflength.dir/src/vcflength.cpp.o
vcflength: CMakeFiles/vcflength.dir/build.make
vcflength: libvcflib.a
vcflength: /usr/lib/x86_64-linux-gnu/libz.so
vcflength: /usr/lib/x86_64-linux-gnu/liblzma.so
vcflength: /usr/lib/x86_64-linux-gnu/libbz2.so
vcflength: CMakeFiles/vcflength.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vcflength"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vcflength.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vcflength.dir/build: vcflength

.PHONY : CMakeFiles/vcflength.dir/build

CMakeFiles/vcflength.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vcflength.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vcflength.dir/clean

CMakeFiles/vcflength.dir/depend:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles/vcflength.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vcflength.dir/depend

