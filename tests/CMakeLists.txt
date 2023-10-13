
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(vector_algorithms_test vector_algorithms_test.cpp)

add_executable(vertex_coords_test vertex_coords_test.cpp  
 ../src/vertex_search/VertexSearcher.cpp
 ../src/vertex_processing/VertexProcessor.cpp
 ../src/data_types/Track.hpp 
 ../src/detector/DetectorVolume.cpp)

target_link_libraries(vector_algorithms_test PRIVATE GTest::GTest ROOT::Physics)

target_link_libraries(vertex_coords_test PRIVATE GTest::GTest ROOT::Core ROOT::Hist ROOT::RIO ROOT::Net
ROOT::Physics ROOT::Tree ROOT::TreeViewer ROOT::Minuit ROOT::TMVA)


add_test(vector_gtest vector_algorithms_test)
add_test(vertex_coords_gtest vertex_coords_test)

enable_testing()