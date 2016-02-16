cmake_minimum_required(VERSION 3.3)

file(GLOB geomUtils_SRC
        "inc/*.h"
        "src/*.cpp")

add_library (geomUtils STATIC ${geomUtils_SRC})

target_include_directories (geomUtils PRIVATE 
        "inc"
        "${cppUtils_SOURCE_DIR}/utils/inc"
)

include(CMakePrintHelpers)
cmake_print_properties(TARGETS geomUtils PROPERTIES INCLUDE_DIRECTORIES)

