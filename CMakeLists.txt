cmake_minimum_required(VERSION 3.3)

project (cppUtils)

#Include boost for the world
SET(BOOST_ROOT "$ENV{DEV}/ext/boost/boost_1_55_0")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.55.0 REQUIRED COMPONENTS) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
endif()

add_subdirectory (utils)
add_subdirectory (geomUtils)
