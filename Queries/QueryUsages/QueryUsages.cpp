//===-- QueryUsages/QueryUsages.cpp ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the QueryUsagesTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "QueryUsages.h"
#include "QueryUsagesActions.h"
#include "QueryUsagesMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int QueryUsagesTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  parsePositionArguments( LineRanges ); 
  ClangTool QueryUsagesTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  QueryUsagesFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeQueryUsagesMatcher(), &Fixer);
  Finder.addMatcher(makeQueryDeclarationMatcher(), &Fixer);

  if (int result = QueryUsagesTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct QueryUsagesFactory : TransformFactory {
  QueryUsagesFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new QueryUsagesTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<QueryUsagesFactory>
X( "query_usages", "tracks the usages of all variables in all files");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int QueryUsagesTransformAnchorSource = 0;
