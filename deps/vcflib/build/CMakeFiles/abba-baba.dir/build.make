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
include CMakeFiles/abba-baba.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/abba-baba.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/abba-baba.dir/flags.make

CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o: CMakeFiles/abba-baba.dir/flags.make
CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o: ../src/abba-baba.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o -c /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/abba-baba.cpp

CMakeFiles/abba-baba.dir/src/abba-baba.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/abba-baba.dir/src/abba-baba.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/abba-baba.cpp > CMakeFiles/abba-baba.dir/src/abba-baba.cpp.i

CMakeFiles/abba-baba.dir/src/abba-baba.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/abba-baba.dir/src/abba-baba.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/src/abba-baba.cpp -o CMakeFiles/abba-baba.dir/src/abba-baba.cpp.s

# Object files for target abba-baba
abba__baba_OBJECTS = \
"CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o"

# External object files for target abba-baba
abba__baba_EXTERNAL_OBJECTS =

abba-baba: CMakeFiles/abba-baba.dir/src/abba-baba.cpp.o
abba-baba: CMakeFiles/abba-baba.dir/build.make
abba-baba: libvcflib.a
abba-baba: /usr/lib/x86_64-linux-gnu/libz.so
abba-baba: /usr/lib/x86_64-linux-gnu/liblzma.so
abba-baba: /usr/lib/x86_64-linux-gnu/libbz2.so
abba-baba: CMakeFiles/abba-baba.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable abba-baba"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/abba-baba.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/abba-baba.dir/build: abba-baba

.PHONY : CMakeFiles/abba-baba.dir/build

CMakeFiles/abba-baba.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/abba-baba.dir/cmake_clean.cmake
.PHONY : CMakeFiles/abba-baba.dir/clean

CMakeFiles/abba-baba.dir/depend:
	cd /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build/CMakeFiles/abba-baba.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/abba-baba.dir/depend
