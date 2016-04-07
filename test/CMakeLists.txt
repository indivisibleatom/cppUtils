file(GLOB GEOMUTILS_TEST_SOURCE_FILES "geomUtils/*.cpp")

add_executable(cppUtilsTest
  main.cpp
  ${GEOMUTILS_TEST_SOURCE_FILES}
  )
include_directories(${PROJECT_SOURCE_DIR}/inc)
#add_dependencies(cppUtilsTest ${PROJECT_SOURCE_DIR/src/)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(cppUtilsTest ${GTEST_LIBRARIES})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
target_link_libraries(cppUtilsTest ${CMAKE_THREAD_LIBS_INIT})

