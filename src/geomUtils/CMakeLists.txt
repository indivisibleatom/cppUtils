file(GLOB geomUtils_SRC
  "src/*.cpp")

add_library (geomUtils STATIC ${geomUtils_SRC})

target_include_directories (geomUtils PUBLIC "${PROJECT_SOURCE_DIR}/inc/geomUtils")
