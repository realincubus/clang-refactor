set(LLVM_LINK_COMPONENTS support)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

add_clang_library(refactorCore
  ReplacementHandling.cpp
  Transforms.cpp
  Transform.cpp
  IncludeExcludeInfo.cpp
  PerfSupport.cpp
  IncludeDirectives.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangLex
  clangTooling
)

