cmake_minimum_required(VERSION 3.15)
project(Map2model_cpp)

option(DISABLE_LTO OFF)
set(CMAKE_CXX_STANDARD 11)

SET(SRCS src/local_topology_analyzer.cpp
         src/topology_analyzer.cpp
         src/parameters_utils.cpp
         src/parameters_reader.cpp
         src/converter_utils.cpp
         src/intersector.cpp
         src/data_reader.cpp
         src/converter.cpp
         src/graph.cpp
         src/graph_utils.cpp
         
         src/clipper.cpp
         src/svgbuilder.cpp
                  src/map2model.cpp

)

SET(SRCS_INCLUDE include)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Create the executable first
# add_executable(map2model  ${SRCS})

# Add Python include directories and libraries after creating the target
pybind11_add_module(map2model ${SRCS})

target_include_directories(map2model PUBLIC ${SRCS_INCLUDE} ${Python_INCLUDE_DIRS})
target_link_libraries(map2model PRIVATE ${Python_LIBRARIES})


install(TARGETS map2model DESTINATION ${CMAKE_INSTALL_PREFIX}) #if its jsut c++ wrapper, install directly to site packages otherwise need to use a subdir
