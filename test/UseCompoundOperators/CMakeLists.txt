set(LLVM_LINK_COMPONENTS support)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${ClangReplaceLocation}
  )
SET( CMAKE_EXPORT_COMPILE_COMMANDS on )

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y" )

add_refactor_unittest( UseCompoundOperatorsTest 
    UseCompoundAssignOperator.cpp
    ../../UseCompoundAssignOperator/UseCompoundAssignOperator.cpp
    ../../UseCompoundAssignOperator/UseCompoundAssignOperatorMatchers.cpp 
    ../../UseCompoundAssignOperator/UseCompoundAssignOperatorActions.cpp 
)

TARGET_LINK_LIBRARIES( UseCompoundOperatorsTest
    clangAST
    clangASTMatchers
    clangBasic
    clangFormat
    clangFrontend
    clangLex
    clangTooling
    refactorCore
    gtest
    gtest_main
)

