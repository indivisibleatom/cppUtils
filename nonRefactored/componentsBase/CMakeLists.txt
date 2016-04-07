file(GLOB componentsBase_SRC
  "inc/*.h"
  "src/*.cpp")

add_library (componentsBase ${componentsBase_SRC})

target_include_directories (componentsBase "${CMAKE_CURRENT_SOURCE_DIR}/inc")
