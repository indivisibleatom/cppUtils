file(GLOB geomComponents_SRC
        "inc/*.h"
        "src/*.cpp")

add_library (geomComponents STATIC ${geomComponents_SRC})

target_include_directories (geomComponents PRIVATE
        "inc"
        "${cppUtils_SOURCE_DIR}/utils/inc"
        "${cppUtils_SOURCE_DIR}/renderUtils" #TODO msati3: Remove this
                                             #dependency
)

include(CMakePrintHelpers)
cmake_print_properties(TARGETS geomComponents PROPERTIES INCLUDE_DIRECTORIES)

