set(LLVM_LINK_COMPONENTS support)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

add_clang_library(refactorCore
  ReplacementHandling.cpp
  Transforms.cpp
  Transform.cpp
  IncludeExcludeInfo.cpp
  PerfSupport.cpp
  IncludeDirectives.cpp
  )
target_link_libraries(refactorCore
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangLex
  clangTooling
  )
