# CMake generated Testfile for 
# Source directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib
# Build directory: /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(RunTests "make")
set_tests_properties(RunTests PROPERTIES  WORKING_DIRECTORY "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/test" _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;295;add_test;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;0;")
add_test(vcf2tsv "python3" "-m" "doctest" "-o" "NORMALIZE_WHITESPACE" "-o" "REPORT_UDIFF" "pytest/vcf2tsv.md")
set_tests_properties(vcf2tsv PROPERTIES  WORKING_DIRECTORY "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/test" _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;310;add_test;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;334;add_pydoctest;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;0;")
add_test(vcfnulldotslashdot "python3" "-m" "doctest" "-o" "NORMALIZE_WHITESPACE" "-o" "REPORT_UDIFF" "pytest/vcfnulldotslashdot.md")
set_tests_properties(vcfnulldotslashdot PROPERTIES  WORKING_DIRECTORY "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/test" _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;310;add_test;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;335;add_pydoctest;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;0;")
add_test(doc/vcfintersect "python3" "-m" "doctest" "-o" "NORMALIZE_WHITESPACE" "-o" "REPORT_UDIFF" "../doc/vcfintersect.md")
set_tests_properties(doc/vcfintersect PROPERTIES  WORKING_DIRECTORY "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/test" _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;318;add_test;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;336;add_doctest;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;0;")
add_test(doc/vcffilter "python3" "-m" "doctest" "-o" "NORMALIZE_WHITESPACE" "-o" "REPORT_UDIFF" "../doc/vcffilter.md")
set_tests_properties(doc/vcffilter PROPERTIES  WORKING_DIRECTORY "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/test" _BACKTRACE_TRIPLES "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;318;add_test;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;337;add_doctest;/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/vcflib/CMakeLists.txt;0;")
