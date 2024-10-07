cmake_minimum_required(VERSION 3.15)
project(Map2model_cpp)

option(DISABLE_LTO OFF)
set(CMAKE_CXX_STANDARD 11)

SET(SRCS src/map2model.cpp
         src/parameters_utils.cpp
         src/parameters_reader.cpp
         src/converter_utils.cpp
         src/intersector.cpp
         src/data_reader.cpp
         src/converter.cpp
         src/graph.cpp
         src/graph_utils.cpp
         src/local_topology_analyzer.cpp
         src/topology_analyzer.cpp
         src/clipper.cpp
         src/svgbuilder.cpp
)

SET(SRCS_INCLUDE include)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Create the executable first
# add_executable(map2model ${SRCS})

# Add Python include directories and libraries after creating the target
pybind11_add_module(map2model src/map2model.cpp)

target_include_directories(map2model PUBLIC ${SRCS_INCLUDE} ${Python_INCLUDE_DIRS})
target_link_libraries(map2model PRIVATE ${Python_LIBRARIES})

# Compiler flags
if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

if (CMAKE_COMPILER_IS_GNUCC)
    SET(GCC_FLAGS -m64 -mfpmath=sse -O3 -ffast-math -march=native -funroll-loops -Wall -Wno-unknown-pragmas -Wextra)
    if (NOT DISABLE_LTO)
        SET(GCC_FLAGS ${GCC_FLAGS} -flto)
    endif()
    target_compile_options(map2model PRIVATE ${GCC_FLAGS})
endif()

if (MSVC)
    target_compile_options(map2model PRIVATE /W4)
endif()
install(TARGETS map2model DESTINATION ${CMAKE_INSTALL_PREFIX}/map2model)
