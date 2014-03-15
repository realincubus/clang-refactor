//===-- UseExpMinusOne/UseExpMinusOne.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the UseExpMinusOneTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "UseExpMinusOne.h"
#include "UseExpMinusOneActions.h"
#include "UseExpMinusOneMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int UseExpMinusOneTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  ClangTool UseExpMinusOneTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  UseExpMinusOneFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeUseExpMinusOneMatcher(), &Fixer);

  if (int result = UseExpMinusOneTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct UseExpMinusOneFactory : TransformFactory {
  UseExpMinusOneFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) LLVM_OVERRIDE {
    return new UseExpMinusOneTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<UseExpMinusOneFactory>
X("use-expm1", "transform exp(var)-1 to expm1(var)");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int UseExpMinusOneTransformAnchorSource = 0;
