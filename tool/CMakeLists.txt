set(LLVM_LINK_COMPONENTS support)

set (ClangModernizeSources
  ClangModernize.cpp
  )

# FIXME: Lib-ify the transforms to simplify the build rules.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

# For each transform subdirectory.
#file(GLOB_RECURSE LoopConvertSources "../LoopConvert/*.cpp")
#list(APPEND ClangModernizeSources ${LoopConvertSources})

#file(GLOB_RECURSE UseNullptrSources "../UseNullptr/*.cpp")
#list(APPEND ClangModernizeSources ${UseNullptrSources})

#file(GLOB_RECURSE UseAutoSources "../UseAuto/*.cpp")
#list(APPEND ClangModernizeSources ${UseAutoSources})

#file(GLOB_RECURSE AddOverrideSources "../AddOverride/*.cpp")
#list(APPEND ClangModernizeSources ${AddOverrideSources})

#file(GLOB_RECURSE PassByValueSources "../PassByValue/*.cpp")
#list(APPEND ClangModernizeSources ${PassByValueSources})

#file(GLOB_RECURSE ReplaceAutoPtrSources "../ReplaceAutoPtr/*.cpp")
#list(APPEND ClangModernizeSources ${ReplaceAutoPtrSources})

#file(GLOB_RECURSE RemoveUnusedParameterSources "../RemoveUnusedParameter/*.cpp")
#list(APPEND ClangModernizeSources ${RemoveUnusedParameterSources})

file(GLOB_RECURSE UsePowerOperatorSources "../UsePowerOperator/*.cpp")
list(APPEND ClangModernizeSources ${UsePowerOperatorSources})

file(GLOB_RECURSE UseCompoundAssignOperatorSources "../UseCompoundAssignOperator/*.cpp")
list(APPEND ClangModernizeSources ${UseCompoundAssignOperatorSources})

file(GLOB_RECURSE UseUnaryOperatorSources "../UseUnaryOperator/*.cpp")
list(APPEND ClangModernizeSources ${UseUnaryOperatorSources})

file(GLOB_RECURSE RemoveIdentityOperationsSources "../RemoveIdentityOperations/*.cpp")
list(APPEND ClangModernizeSources ${RemoveIdentityOperationsSources})

file(GLOB_RECURSE UseHypotSources "../UseHypot/*.cpp")
list(APPEND ClangModernizeSources ${UseHypotSources})

#file(GLOB_RECURSE UseExpMinusOneSources "../UseExpMinusOne/*.cpp")
#list(APPEND ClangModernizeSources ${UseExpMinusOneSources})

#file(GLOB_RECURSE UseFMASources "../UseFusedMultiplyAdd/*.cpp")
#list(APPEND ClangModernizeSources ${UseFMASources})

file(GLOB_RECURSE UseMathConstantsSources "../UseMathConstants/*.cpp")
list(APPEND ClangModernizeSources ${UseMathConstantsSources})

file(GLOB_RECURSE UseRoundSources "../UseRound/*.cpp")
list(APPEND ClangModernizeSources ${UseRoundSources})

file(GLOB_RECURSE RepairBrokenNullCheckSources "../RepairBrokenNullCheck/*.cpp")
list(APPEND ClangModernizeSources ${RepairBrokenNullCheckSources})

file(GLOB_RECURSE CollapseIfStatementSources "../CollapsIfStatement/*.cpp")
list(APPEND ClangModernizeSources ${CollapseIfStatementSources})

file(GLOB_RECURSE ExtractMethodSources "../ExtractMethod/*.cpp")
list(APPEND ClangModernizeSources ${ExtractMethodSources})

file(GLOB_RECURSE RenameVariableSources "../RenameVariable/*.cpp")
list(APPEND ClangModernizeSources ${RenameVariableSources})

file(GLOB_RECURSE UseVectorSources "../UseVector/*.cpp")
list(APPEND ClangModernizeSources ${UseVectorSources})

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

