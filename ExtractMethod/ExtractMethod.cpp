//===-- ExtractMethod/ExtractMethod.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the ExtractMethodTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "ExtractMethod.h"
#include "ExtractMethodActions.h"
#include "ExtractMethodMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int ExtractMethodTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  parsePositionArguments( LineRanges ); 
  ClangTool ExtractMethodTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  ExtractMethodFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeExtractMethodMatcher(), &Fixer);

  if (int result = ExtractMethodTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }
  MatchFinder FinderCompound;
  FinderCompound.addMatcher(makeExtractMethodMatcherCompound(), &Fixer);

  if (int result = ExtractMethodTool.run(createActionFactory(FinderCompound))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct ExtractMethodFactory : TransformFactory {
  ExtractMethodFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new ExtractMethodTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<ExtractMethodFactory>
X("extract-method", "fill in");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int ExtractMethodTransformAnchorSource = 0;
