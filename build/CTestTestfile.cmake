# CMake generated Testfile for 
# Source directory: /home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo
# Build directory: /home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(graph_test "/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/build/graph_test.t")
set_tests_properties(graph_test PROPERTIES  _BACKTRACE_TRIPLES "/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/CMakeLists.txt;98;add_test;/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/CMakeLists.txt;0;")
add_test(unit_tests "/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/build/unit_tests.t")
set_tests_properties(unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/CMakeLists.txt;104;add_test;/home/giuseppe/Scrivania/Giuseppe/università/triennale/primo_anno/programmazione/razzo/CMakeLists.txt;0;")
subdirs("_deps/sfml-build")
