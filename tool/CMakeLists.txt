set(LLVM_LINK_COMPONENTS support)

set (ClangModernizeSources
  ClangModernize.cpp
  )

# FIXME: Lib-ify the transforms to simplify the build rules.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Og" )

file(GLOB_RECURSE NestedIndexRenamerSources "../Transformations/NestedIndexRenamer/*.cpp")
list(APPEND ClangModernizeSources ${NestedIndexRenamerSources})

add_clang_executable(clang-refactor
  ${ClangModernizeSources}
  )


add_dependencies(clang-refactor
  clang-headers clang-apply-replacements
)

target_link_libraries(clang-refactor
  clangAST
  clangASTMatchers
  clangBasic
  clangFormat
  clangFrontend
  clangLex
  clangTooling
  refactorCore
  )

install(TARGETS clang-refactor
  RUNTIME DESTINATION bin)

add_clang_library( refactorApp
  ${ClangModernizeSources}

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangLex
  clangTooling
  refactorCore
)

add_dependencies(refactorApp
  clang-headers clang-apply-replacements
)


