cmake_minimum_required(VERSION 3.16)
project(DsTauVertexing LANGUAGES CUDA CXX)
add_definitions(-O3)

add_compile_options(-mavx2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CUDA_COMPILER /usr/local/cuda-10.0/bin/nvcc) #make it dynamic
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_CUDA_STANDARD 11)

add_executable(DsTauVertexing src/main/MainClass.cpp
 src/main/AppLogic.cpp
 src/vertex_search/VertexSearcher.cpp
 src/vertex_processing/VertexProcessor.cpp
 src/detector/DetectorVolume.cpp
 src/downloaders/FedraDownloader.cpp)

find_package(ROOT REQUIRED COMPONENTS RIO Net)
if(ROOT_FOUND)
  MESSAGE(STATUS "Found ROOT framework, OK.")
else (ROOT_FOUND)
  MESSAGE(STATUS "ERROR: Could not find ROOT framework or its components.")
endif(ROOT_FOUND)

target_include_directories(DsTauVertexing PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

target_link_libraries(DsTauVertexing PUBLIC ROOT::Core ROOT::Hist ROOT::RIO ROOT::Net
 ROOT::Physics ROOT::Tree ROOT::TreeViewer ROOT::Minuit ROOT::TMVA)

include(CTest)
enable_testing()
add_subdirectory(tests)

# ROOT_GENERATE_DICTIONARY(fedra_dict 
# src/downloaders/EdbSegP.h 
# # src/downloaders/EdbTrack2D.h 
# # src/downloaders/EdbPoint2D.h
# # src/downloaders/EdbPoint.h
# # src/downloaders/EdbAngle2D.h
# # src/downloaders/EdbID.h
# src/downloaders/MyClass.h #test object
# MODULE DsTauVertexing LINKDEF LinkDef.h)