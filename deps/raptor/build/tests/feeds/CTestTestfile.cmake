# CMake generated Testfile for 
# Source directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds
# Build directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/feeds
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(feeds.test04.atom:run "/usr/bin/cmake" "-P" "test-feeds.test04.atom.cmake")
set_tests_properties(feeds.test04.atom:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;31;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test04.atom:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test04-result.ttl" "test04.ttl")
set_tests_properties(feeds.test04.atom:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;31;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test05.atom:run "/usr/bin/cmake" "-P" "test-feeds.test05.atom.cmake")
set_tests_properties(feeds.test05.atom:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;37;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test05.atom:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test05-result.ttl" "test05.ttl")
set_tests_properties(feeds.test05.atom:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;37;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test01.ttl:run "/usr/bin/cmake" "-P" "test-feeds.test01.ttl.cmake")
set_tests_properties(feeds.test01.ttl:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;47;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test01.ttl:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test01-result.atom" "test01.atom")
set_tests_properties(feeds.test01.ttl:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;47;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test02.ttl:run "/usr/bin/cmake" "-P" "test-feeds.test02.ttl.cmake")
set_tests_properties(feeds.test02.ttl:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;53;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test02.ttl:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test02-result.atom" "test02.atom")
set_tests_properties(feeds.test02.ttl:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;53;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test03.ttl:run "/usr/bin/cmake" "-P" "test-feeds.test03.ttl.cmake")
set_tests_properties(feeds.test03.ttl:run PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;368;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;59;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")
add_test(feeds.test03.ttl:cmp "/usr/bin/cmake" "-E" "compare_files" "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test03-result.atom" "test03.atom")
set_tests_properties(feeds.test03.ttl:cmp PROPERTIES  _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/CMakeLists.txt;369;ADD_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;59;RAPPER_TEST;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/CMakeLists.txt;0;")