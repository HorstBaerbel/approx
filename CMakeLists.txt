cmake_minimum_required(VERSION 3.1.0)

project(approx)

#-------------------------------------------------------------------------------
# set up compiler flags and defines

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") # multi-processor compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP") # multi-processor compilation
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    # check for proper G++ version
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 8.)
            message(SEND_ERROR "You need at least G++ 8.x to compile approx!")
    endif()
    # set up compiler flags for GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # support C++14
    # make sure we have proper optimizations in release mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ffast-math -fno-aggressive-loop-optimizations")
endif()

#-------------------------------------------------------------------------------
# define targets

LIST(APPEND APPROX_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/cxxopts/include
    ${CMAKE_SOURCE_DIR}/sciplot
)

LIST(APPEND APPROX_SOURCES
    main.cpp
)

LIST(APPEND APPROX_LIBRARIES
    stdc++fs
)

#-------------------------------------------------------------------------------
# define targets

add_executable(approx ${APPROX_SOURCES})
target_include_directories(approx PRIVATE ${APPROX_INCLUDE_DIRECTORIES})
target_link_libraries(approx ${APPROX_LIBRARIES})
