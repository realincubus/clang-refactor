//===-- UseFusedMultiplyAdd/UseFusedMultiplyAdd.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the UseFusedMultiplyAddTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "UseFusedMultiplyAdd.h"
#include "UseFusedMultiplyAddActions.h"
#include "UseFusedMultiplyAddMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int UseFusedMultiplyAddTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths) {
  ClangTool UseFusedMultiplyAddTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  UseFusedMultiplyAddFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeUseFusedMultiplyAddMatcher(), &Fixer);

  if (int result = UseFusedMultiplyAddTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct UseFusedMultiplyAddFactory : TransformFactory {
  UseFusedMultiplyAddFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new UseFusedMultiplyAddTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<UseFusedMultiplyAddFactory>
X("use-fma", "will transform a * b + c to fma( a,b,c )");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int UseFusedMultiplyAddTransformAnchorSource = 0;
