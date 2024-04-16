# CMake generated Testfile for 
# Source directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig
# Build directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/trig
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(trig.example1:run "/usr/bin/cmake" "-P" "test-trig.example1.cmake")
set_tests_properties(trig.example1:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;7;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.example1:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/example1.out" "example1.res")
set_tests_properties(trig.example1:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;7;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.example2:run "/usr/bin/cmake" "-P" "test-trig.example2.cmake")
set_tests_properties(trig.example2:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;13;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.example2:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/example2.out" "example2.res")
set_tests_properties(trig.example2:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;13;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.example3:run "/usr/bin/cmake" "-P" "test-trig.example3.cmake")
set_tests_properties(trig.example3:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;19;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.example3:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/example3.out" "example3.res")
set_tests_properties(trig.example3:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;19;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.bug370:run "/usr/bin/cmake" "-P" "test-trig.bug370.cmake")
set_tests_properties(trig.bug370:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;25;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.bug370:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/bug370.out" "bug370.res")
set_tests_properties(trig.bug370:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;25;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.bug451:run "/usr/bin/cmake" "-P" "test-trig.bug451.cmake")
set_tests_properties(trig.bug451:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;31;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
add_test(trig.bug451:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/bug451.out" "bug451.res")
set_tests_properties(trig.bug451:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;31;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/CMakeLists.txt;0;")
