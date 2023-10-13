# CMake generated Testfile for 
# Source directory: /home/roma/Vertexing/tests
# Build directory: /home/roma/Vertexing/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vector_gtest "vector_algorithms_test")
set_tests_properties(vector_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/roma/Vertexing/tests/CMakeLists.txt;25;add_test;/home/roma/Vertexing/tests/CMakeLists.txt;0;")
add_test(vertex_coords_gtest "vertex_coords_test")
set_tests_properties(vertex_coords_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/roma/Vertexing/tests/CMakeLists.txt;26;add_test;/home/roma/Vertexing/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
