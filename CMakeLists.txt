include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${ClangReplaceLocation}
  )
SET( CMAKE_EXPORT_COMPILE_COMMANDS on )


add_subdirectory(tool)
add_subdirectory(Core)


