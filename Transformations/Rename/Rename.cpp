//===-- Rename/Rename.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the RenameTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "Rename.h"
#include "RenameActions.h"
#include "RenameMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int RenameTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  parsePositionArguments( LineRanges ); 
  ClangTool RenameTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  RenameFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeRenameMatcher(), &Fixer);
  Finder.addMatcher(makeFunctionDeclMatcher(), &Fixer);

  if (int result = RenameTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct RenameFactory : TransformFactory {
  RenameFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new RenameTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<RenameFactory>
X("rename", "renames a variable ");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int RenameTransformAnchorSource = 0;
