# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# This defines default install directories like "lib"
include(GNUInstallDirs)

# Project's name
project(structures)
# We build using c++14
set(CMAKE_CXX_STANDARD 14)

# Use all standard-compliant optimizations
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

# Let cmake decide where to put the output files, allowing for out-of-tree builds.

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We are probably an external project. Don't use @rpath in Mac builds'
    # install_name fields (AKA LC_ID_DYLIB in otool -l output). Populate with
    # an absolute path instead. This will let us actually find the library when
    # we use it as a CMake external project and don't fully install it to any
    # normal lib directory.
    message("structures is root project or external_project")
    set (CMAKE_MACOSX_RPATH OFF)
else()
    # We are probably an add_subdirectory. We will expect to be in the root
    # project's lib directory, so we do want to have our non-installed
    # install_name use @rpath.
    message("structures is add_subdirectory project")
    set (CMAKE_MACOSX_RPATH ON)
endif()

# The install_name gets modified on installation to be this.
set (CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

add_library(structures_objs OBJECT
    src/stable_double.cpp
    src/suffix_tree.cpp
    src/tests.cpp
    src/union_find.cpp
)
  
set(structures_INCLUDES
  "src/include"
)

# Use the include directory when building the objects.
target_include_directories(structures_objs PUBLIC ${structures_INCLUDES})
# Build objects position-independent to allow a shared library
set_target_properties(structures_objs PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# Use the include directory when building the objects.
# It can't be picked up via dependency by the other libraries even if it's public.
target_include_directories(structures_objs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

# Build objects position-independent to allow a shared library
set_target_properties(structures_objs PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# Make static and shared versions with the same base name.
# Make sure to give them interface include directories that depending targets can use.
add_library(structures_shared SHARED $<TARGET_OBJECTS:structures_objs>)
set_target_properties(structures_shared PROPERTIES OUTPUT_NAME structures)
set_target_properties(structures_shared PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Make sure we can find our dependency libraries after installation, if we aren't installing into the system search path
set_target_properties(structures_shared PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
target_include_directories(structures_shared INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
add_library(structures_static STATIC $<TARGET_OBJECTS:structures_objs>)
set_target_properties(structures_static PROPERTIES OUTPUT_NAME structures)
target_include_directories(structures_static INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

# Set up for installability
install(TARGETS structures_shared structures_static 
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY src/include/structures
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp"
)

# Build the test executable
add_executable(test
  src/tests.cpp)
target_link_libraries(test structures_shared)
set_target_properties(test PROPERTIES OUTPUT_NAME "test")
set_target_properties(test PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
