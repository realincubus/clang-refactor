//===-- RemoveIdentityOperations/RemoveIdentityOperations.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the RemoveIdentityOperationsTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "RemoveIdentityOperations.h"
#include "RemoveIdentityOperationsActions.h"
#include "RemoveIdentityOperationsMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int RemoveIdentityOperationsTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  ClangTool RemoveIdentityOperationsTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  RemoveIdentityOperationsFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeRemoveIdentityOperationsMatcherPlus(), &Fixer);
  Finder.addMatcher(makeRemoveIdentityOperationsMatcherMult(), &Fixer);

  if (int result = RemoveIdentityOperationsTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct RemoveIdentityOperationsFactory : TransformFactory {
  RemoveIdentityOperationsFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new RemoveIdentityOperationsTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<RemoveIdentityOperationsFactory>
X("remove-identity-ops", "removes a (*|/) 1 a (+|-) 0");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int RemoveIdentityOperationsTransformAnchorSource = 0;
