cmake_minimum_required(VERSION 3.2)

project (cppUtils)

if (WIN32)
  SET(BOOST_ROOT "$ENV{DEV}/ext/boost/boost_1_55_0")
endif (WIN32)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.54.0 REQUIRED COMPONENTS)

#Some global includes
#Include boost for the world
if (Boost_FOUND)
    include_directories("${Boost_INCLUDE_DIRS}")
endif ()

#Some global flags
set (CMAKE_CXX_STANDARD 11)

add_subdirectory (utils)
add_subdirectory (geomComponents)
