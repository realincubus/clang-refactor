//===-- QueryMangledName/QueryMangledName.cpp ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the QueryMangledNameTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "QueryMangledName.h"
#include "QueryMangledNameActions.h"
#include "QueryMangledNameMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int QueryMangledNameTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  parsePositionArguments( LineRanges ); 
  ClangTool QueryMangledNameTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  QueryMangledNameFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeQueryMangledNameMatcher(), &Fixer);
  Finder.addMatcher(makeQueryMangledNameMatcherRef(), &Fixer);

  if (int result = QueryMangledNameTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct QueryMangledNameFactory : TransformFactory {
  QueryMangledNameFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new QueryMangledNameTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<QueryMangledNameFactory>
X( "query-mangled", "will retrieve the mangled name inside the target area");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int QueryMangledNameTransformAnchorSource = 0;
