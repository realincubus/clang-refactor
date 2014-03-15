//===-- UseUnaryOperator/UseUnaryOperator.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the UseUnaryOperatorTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "UseUnaryOperator.h"
#include "UseUnaryOperatorActions.h"
#include "UseUnaryOperatorMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;

int UseUnaryOperatorTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
				const llvm::cl::list<std::string>& LineRanges 
			       ) {
  ClangTool UseUnaryOperatorTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  UseUnaryOperatorFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeUseUnaryOperatorMatcher(), &Fixer);

  if (int result = UseUnaryOperatorTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct UseUnaryOperatorFactory : TransformFactory {
  UseUnaryOperatorFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new UseUnaryOperatorTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<UseUnaryOperatorFactory>
X("use-unary-operators", "transforms b += 1 to ++b");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int UseUnaryOperatorTransformAnchorSource = 0;
