#set(LLVM_LINK_COMPONENTS support)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Og -DCLANG_ENABLE_ARCMT -DCLANG_ENABLE_OBJC_REWRITER -DCLANG_ENABLE_STATIC_ANALYZER -DGTEST_HAS_RTTI=0 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti -fPIC -ffunction-sections -fdata-sections -fvisibility-inlines-hidden -fno-common" )

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
)


add_library( refactorCore STATIC
  ReplacementHandling.cpp
  Transforms.cpp
  Transform.cpp
  IncludeExcludeInfo.cpp
  PerfSupport.cpp
  IncludeDirectives.cpp
)

#target_link_libraries( refactorCore 
#  LLVMSupport
#  clangAST
#  clangASTMatchers
#  clangBasic
#  clangFrontend
#  clangLex
#  clangTooling
#)


