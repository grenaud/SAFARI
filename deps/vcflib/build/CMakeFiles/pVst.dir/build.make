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
include CMakeFiles/pVst.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/pVst.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pVst.dir/flags.make

CMakeFiles/pVst.dir/src/pVst.cpp.o: CMakeFiles/pVst.dir/flags.make
CMakeFiles/pVst.dir/src/pVst.cpp.o: ../src/pVst.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/pVst.dir/src/pVst.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/pVst.dir/src/pVst.cpp.o -c /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/pVst.cpp

CMakeFiles/pVst.dir/src/pVst.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pVst.dir/src/pVst.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/pVst.cpp > CMakeFiles/pVst.dir/src/pVst.cpp.i

CMakeFiles/pVst.dir/src/pVst.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pVst.dir/src/pVst.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/pVst.cpp -o CMakeFiles/pVst.dir/src/pVst.cpp.s

# Object files for target pVst
pVst_OBJECTS = \
"CMakeFiles/pVst.dir/src/pVst.cpp.o"

# External object files for target pVst
pVst_EXTERNAL_OBJECTS =

pVst: CMakeFiles/pVst.dir/src/pVst.cpp.o
pVst: CMakeFiles/pVst.dir/build.make
pVst: libvcflib.a
pVst: /usr/lib/x86_64-linux-gnu/libz.so
pVst: /usr/lib/x86_64-linux-gnu/liblzma.so
pVst: /usr/lib/x86_64-linux-gnu/libbz2.so
pVst: CMakeFiles/pVst.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable pVst"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pVst.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pVst.dir/build: pVst

.PHONY : CMakeFiles/pVst.dir/build

CMakeFiles/pVst.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pVst.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pVst.dir/clean

CMakeFiles/pVst.dir/depend:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles/pVst.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pVst.dir/depend

