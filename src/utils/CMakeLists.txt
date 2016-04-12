file(GLOB utils_SRC
  "*.cpp")

add_custom_target(
  generateHeaders COMMAND python ${PROJECT_SOURCE_DIR}/tools/enumGen.py
  ${PROJECT_SOURCE_DIR}/inc/utils/colors.h
  ${PROJECT_SOURCE_DIR}/inc/utils/colors.config COLORS
  )

add_library (utils STATIC ${utils_SRC})
add_dependencies(utils generateHeaders)

#Remove generated files. TODO msati3: Is there a way to wildcard this to CMAKE?
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  "${PROJECT_SOURCE_DIR}/inc/colors_gen.h")

target_include_directories (utils PUBLIC "${PROJECT_SOURCE_DIR}/inc/utils")
