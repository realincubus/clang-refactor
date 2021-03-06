#set(LLVM_LINK_COMPONENTS support)

set (ClangModernizeSources
  ClangModernize.cpp
  )

# FIXME: Lib-ify the transforms to simplify the build rules.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Og -DCLANG_ENABLE_ARCMT -DCLANG_ENABLE_OBJC_REWRITER -DCLANG_ENABLE_STATIC_ANALYZER -DGTEST_HAS_RTTI=0 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti -fPIC -ffunction-sections -fdata-sections -fvisibility-inlines-hidden -fno-common" )

file(GLOB_RECURSE UsePowerOperatorSources "../Transformations/UsePowerOperator/*.cpp")
list(APPEND ClangModernizeSources ${UsePowerOperatorSources})

file(GLOB_RECURSE UseCompoundAssignOperatorSources "../Transformations/UseCompoundAssignOperator/*.cpp")
list(APPEND ClangModernizeSources ${UseCompoundAssignOperatorSources})

file(GLOB_RECURSE UseUnaryOperatorSources "../Transformations/UseUnaryOperator/*.cpp")
list(APPEND ClangModernizeSources ${UseUnaryOperatorSources})

file(GLOB_RECURSE RemoveIdentityOperationsSources "../Transformations/RemoveIdentityOperations/*.cpp")
list(APPEND ClangModernizeSources ${RemoveIdentityOperationsSources})

file(GLOB_RECURSE UseHypotSources "../Transformations/UseHypot/*.cpp")
list(APPEND ClangModernizeSources ${UseHypotSources})

file(GLOB_RECURSE UseMathConstantsSources "../Transformations/UseMathConstants/*.cpp")
list(APPEND ClangModernizeSources ${UseMathConstantsSources})

file(GLOB_RECURSE UseRoundSources "../Transformations/UseRound/*.cpp")
list(APPEND ClangModernizeSources ${UseRoundSources})

file(GLOB_RECURSE RepairBrokenNullCheckSources "../Transformations/RepairBrokenNullCheck/*.cpp")
list(APPEND ClangModernizeSources ${RepairBrokenNullCheckSources})

file(GLOB_RECURSE CollapseIfStatementSources "../Transformations/CollapsIfStatement/*.cpp")
list(APPEND ClangModernizeSources ${CollapseIfStatementSources})

file(GLOB_RECURSE ExtractMethodSources "../Transformations/ExtractMethod/*.cpp")
list(APPEND ClangModernizeSources ${ExtractMethodSources})

file(GLOB_RECURSE RenameSources "../Transformations/Rename/*.cpp")
list(APPEND ClangModernizeSources ${RenameSources})

file(GLOB_RECURSE UseStdArraySources "../Transformations/UseStdArray/*.cpp")
list(APPEND ClangModernizeSources ${UseStdArraySources})

file(GLOB_RECURSE UseVectorSources "../Transformations/UseVector/*.cpp")
list(APPEND ClangModernizeSources ${UseVectorSources})

file(GLOB_RECURSE UseEmplaceSources "../Transformations/UseEmplace/*.cpp")
list(APPEND ClangModernizeSources ${UseEmplaceSources})

file(GLOB_RECURSE UnInvertLogicSources "../Transformations/UnInvertLogic/*.cpp")
list(APPEND ClangModernizeSources ${UnInvertLogicSources})

file(GLOB_RECURSE RemoveRedundantConditionalSources "../Transformations/RemoveRedudantConditional/*.cpp")
list(APPEND ClangModernizeSources ${RemoveRedundantConditionalSources})

file(GLOB_RECURSE UseEmptySources "../Transformations/UseEmpty/*.cpp")
list(APPEND ClangModernizeSources ${UseEmptySources})

file(GLOB_RECURSE UseRAIISources "../Transformations/UseRAII/*.cpp")
list(APPEND ClangModernizeSources ${UseRAIISources})

file(GLOB_RECURSE UseConstSources "../Transformations/UseConst/*.cpp")
list(APPEND ClangModernizeSources ${UseConstSources})

file(GLOB_RECURSE UseAlgorithmsSources "../Transformations/UseAlgorithms/*.cpp")
list(APPEND ClangModernizeSources ${UseAlgorithmsSources})

file(GLOB_RECURSE UnglobalMethodSources "../Transformations/UnglobalMethod/*.cpp")
list(APPEND ClangModernizeSources ${UnglobalMethodSources})

file(GLOB_RECURSE UseSwapSources "../Transformations/UseSwap/*.cpp")
list(APPEND ClangModernizeSources ${UseSwapSources})

file(GLOB_RECURSE UseFullyQualifiedTypesSources "../Transformations/UseFullyQualifiedTypes/*.cpp")
list(APPEND ClangModernizeSources ${UseFullyQualifiedTypesSources})

file(GLOB_RECURSE QueryMangledNameSources "../Queries/QueryMangledName/*.cpp")
list(APPEND ClangModernizeSources ${QueryMangledNameSources})

#file(GLOB_RECURSE CoreSources "../Core/*.cpp")
#list(APPEND ClangModernizeSources ${CoreSources})

# if you use a non standard directory for your libraries you have to add it
# here
#LINK_DIRECTORIES( /home/incubus/dev/llvm-clang-3.5-build/lib/ )

add_executable(clang-refactor
  ${ClangModernizeSources}
  )


#add_dependencies(clang-refactor
#  clang-headers clang-apply-replacements
#)
#    clangAST
#    clangASTMatchers
#    clangBasic
#    clangFormat
#    clangFrontend
#    clangLex
#    clangTooling
#    refactorCore
#    clangASTMatchers 
#    clangFrontend 
#    clangParse 
#    LLVMMCParser 
#    clangSerialization 
#    clangSema 
#    clangEdit 
#    clangAnalysis 
#    LLVMBitReader 
#    LLVMCore 
#    clangDriver 
#    LLVMOption 
#    clangRewrite 
#    clangAST 
#    clangLex 
#    clangBasic 
#    LLVMMC 
#    LLVMSupport 
#    rt  
#    dl  
#    tinfo  
#    pthread  
#    z 


target_link_libraries(clang-refactor
LLVMSupport
clangAST
clangASTMatchers
clangBasic
clangFormat
clangFrontend
clangLex
clangTooling
refactorCore
clangTooling
clangASTMatchers
clangFrontend
clangParse
LLVMMCParser
clangSerialization
clangSema
clangEdit
clangAnalysis
LLVMBitReader
LLVMCore
clangDriver
LLVMOption
clangRewrite
clangAST
clangLex
clangBasic
LLVMMC
LLVMSupport
rt
dl
tinfo
pthread
z
  )

#install(TARGETS clang-refactor
#  RUNTIME DESTINATION bin)

#add_clang_library( refactorApp
#  ${ClangModernizeSources}
#
#  LINK_LIBS
#  clangAST
#  clangASTMatchers
#  clangBasic
#  clangFrontend
#  clangLex
#  clangTooling
#  refactorCore
#)

#add_dependencies(refactorApp
#  clang-headers clang-apply-replacements
#)


