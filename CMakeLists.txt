cmake_minimum_required(VERSION 3.21)
project(ScaleBalancer VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Execution source
add_executable(scaleblancer src/scaleblancer.cpp)
target_include_directories(scaleblancer 
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>
)

if(BUILD_TESTING)
    # CTEST argumes Must be set before including CTest framework.
    set(CMAKE_CTEST_ARGUMENTS "--output-on-failure" "--output-junit" "junit.xml")
    find_package(Catch2 3 REQUIRED)
    include(Catch)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()


