file(GLOB utils_SRC
  "inc/*.h"
  "src/*.cpp")

add_custom_target(
  generateHeaders COMMAND python ${PROJECT_SOURCE_DIR}/tools/enumGen.py
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/colors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/colors.config COLORS
  )

add_library (utils STATIC ${utils_SRC})
add_dependencies(utils generateHeaders)

target_include_directories (utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
