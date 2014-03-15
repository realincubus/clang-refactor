//===-- UsePowerOperator/TransformationTemplate.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the UsePowerOperatorTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "UsePowerOperator.h"
#include "UsePowerOperatorActions.h"
#include "UsePowerOperatorMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;


int UsePowerOperatorTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges ) {
  ClangTool UsePowerOperatorTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  llvm::SmallVector<llvm::StringRef, 1> MacroNames;
  MatchFinder Finder;
  UsePowerOperatorFixer Fixer(AcceptedChanges, MacroNames, /*Owner=*/ *this);

  Finder.addMatcher(makeMatcher(), &Fixer);

  if (int result = UsePowerOperatorTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct UsePowerOperatorFactory : TransformFactory {
  UsePowerOperatorFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new UsePowerOperatorTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<UsePowerOperatorFactory>
X("use-pow", "uses power operator instead of a * a");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int UsePowerOperatorTransformAnchorSource = 0;
