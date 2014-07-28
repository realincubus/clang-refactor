set(LLVM_LINK_COMPONENTS support)

set (ClangModernizeSources
  ClangModernize.cpp
  )

# FIXME: Lib-ify the transforms to simplify the build rules.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Og" )

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

file(GLOB_RECURSE UseStdArraySources "../UseStdArray/*.cpp")
list(APPEND ClangModernizeSources ${UseStdArraySources})

file(GLOB_RECURSE UseVectorSources "../UseVector/*.cpp")
list(APPEND ClangModernizeSources ${UseVectorSources})

file(GLOB_RECURSE UseEmplaceSources "../UseEmplace/*.cpp")
list(APPEND ClangModernizeSources ${UseEmplaceSources})

file(GLOB_RECURSE UnInvertLogicSources "../UnInvertLogic/*.cpp")
list(APPEND ClangModernizeSources ${UnInvertLogicSources})

file(GLOB_RECURSE RemoveRedundantConditionalSources "../RemoveRedudantConditional/*.cpp")
list(APPEND ClangModernizeSources ${RemoveRedundantConditionalSources})

file(GLOB_RECURSE UseEmptySources "../UseEmpty/*.cpp")
list(APPEND ClangModernizeSources ${UseEmptySources})

file(GLOB_RECURSE PullTemporariesSources "../PullTemporaries/*.cpp")
list(APPEND ClangModernizeSources ${PullTemporariesSources})

file(GLOB_RECURSE UseRAIISources "../UseRAII/*.cpp")
list(APPEND ClangModernizeSources ${UseRAIISources})

file(GLOB_RECURSE UseLocalIteratorInForLoopSources "../UseLocalIteratorInForLoop/*.cpp")
list(APPEND ClangModernizeSources ${UseLocalIteratorInForLoopSources})

file(GLOB_RECURSE MoveToFirstUsageSources "../MoveToFirstUsage/*.cpp")
list(APPEND ClangModernizeSources ${MoveToFirstUsageSources})

file(GLOB_RECURSE UseConstSources "../UseConst/*.cpp")
list(APPEND ClangModernizeSources ${UseConstSources})

file(GLOB_RECURSE ForLoopStartFromZeroSources "../ForLoopStartFromZero/*.cpp")
list(APPEND ClangModernizeSources ${ForLoopStartFromZeroSources})

#file(GLOB_RECURSE UseAsyncSources "../UseAsync/*.cpp")
#list(APPEND ClangModernizeSources ${UseAsyncSources})

file(GLOB_RECURSE UseAlgorithmsSources "../UseAlgorithms/*.cpp")
list(APPEND ClangModernizeSources ${UseAlgorithmsSources})

file(GLOB_RECURSE UnglobalMethodSources "../UnglobalMethod/*.cpp")
list(APPEND ClangModernizeSources ${UnglobalMethodSources})

file(GLOB_RECURSE NestedIndexRenamerSources "../NestedIndexRenamer/*.cpp")
list(APPEND ClangModernizeSources ${NestedIndexRenamerSources})

file(GLOB_RECURSE UseSwapSources "../UseSwap/*.cpp")
list(APPEND ClangModernizeSources ${UseSwapSources})

file(GLOB_RECURSE FuseToSimdVectorSources "../FuseToSimdVector/*.cpp")
list(APPEND ClangModernizeSources ${FuseToSimdVectorSources})

file(GLOB_RECURSE UseFullyQualifiedTypesSources "../UseFullyQualifiedTypes/*.cpp")
list(APPEND ClangModernizeSources ${UseFullyQualifiedTypesSources})

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


