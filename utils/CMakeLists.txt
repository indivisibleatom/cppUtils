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

#Remove generated files. TODO msati3: Is there a way to wildcard this to CMAKE?
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/inc/colors_gen.h")

target_include_directories (utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
