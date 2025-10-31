cmake_minimum_required(VERSION 3.15)
project(FluidSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP with macOS-specific handling
if(APPLE)
    # Try to find Homebrew's libomp installation
    execute_process(
        COMMAND brew --prefix libomp
        OUTPUT_VARIABLE LIBOMP_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(LIBOMP_PREFIX)
        message(STATUS "Found Homebrew libomp at: ${LIBOMP_PREFIX}")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY "${LIBOMP_PREFIX}/lib/libomp.dylib")
        
        # Set library search path
        link_directories(${LIBOMP_PREFIX}/lib)
        include_directories(${LIBOMP_PREFIX}/include)
    endif()
endif()

find_package(OpenMP)

# Find VTK library
find_package(VTK REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    IOLegacy
    IOXML
)

if(VTK_FOUND)
    message(STATUS "VTK found: ${VTK_VERSION}")
else()
    message(FATAL_ERROR "VTK not found. Install with: brew install vtk")
endif()

# Add executable
add_executable(fluid_sim
    src/main.cpp
    src/FluidSolver.cpp
    src/VTKWriter.cpp
)

# Link VTK libraries
target_link_libraries(fluid_sim PRIVATE ${VTK_LIBRARIES})
vtk_module_autoinit(TARGETS fluid_sim MODULES ${VTK_LIBRARIES})

# Link OpenMP if available
if(OpenMP_CXX_FOUND)
    target_link_libraries(fluid_sim PRIVATE OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP enabled - parallel execution active")
    message(STATUS "OpenMP flags: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found - simulation will run sequentially")
    message(STATUS "To install OpenMP on macOS: brew install libomp")
    message(STATUS "Then rebuild: rm -rf build && mkdir build && cd build && cmake .. && make")
endif()

# Compiler flags for optimization
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
