//===-- ForLoopStartFromZero/ForLoopStartFromZero.cpp - C++11 nullptr migration ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the implementation of the ForLoopStartFromZeroTransform
/// class.
///
//===----------------------------------------------------------------------===//

#include "ForLoopStartFromZero.h"
#include "ForLoopStartFromZeroActions.h"
#include "ForLoopStartFromZeroMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"

using clang::ast_matchers::MatchFinder;
using namespace clang::tooling;
using namespace clang;
namespace cl = llvm::cl;

int ForLoopStartFromZeroTransform::apply(const CompilationDatabase &Database,
                               const std::vector<std::string> &SourcePaths,
			       const llvm::cl::list<std::string>& LineRanges 
			       ) {
  parsePositionArguments( LineRanges ); 
  ClangTool ForLoopStartFromZeroTool(Database, SourcePaths);

  unsigned AcceptedChanges = 0;

  MatchFinder Finder;
  ForLoopStartFromZeroFixer Fixer(AcceptedChanges, /*Owner=*/ *this);

  Finder.addMatcher(makeForLoopStartFromZeroMatcher(), &Fixer);

  if (int result = ForLoopStartFromZeroTool.run(createActionFactory(Finder))) {
    llvm::errs() << "Error encountered during translation.\n";
    return result;
  }

  setAcceptedChanges(AcceptedChanges);

  return 0;
}

struct ForLoopStartFromZeroFactory : TransformFactory {
  ForLoopStartFromZeroFactory() {
    Since.Clang = Version(3, 0);
    Since.Gcc = Version(4, 6);
    Since.Icc = Version(12, 1);
    Since.Msvc = Version(10);
  }

  Transform *createTransform(const TransformOptions &Opts) override {
    return new ForLoopStartFromZeroTransform(Opts);
  }
};

// Register the factory using this statically initialized variable.
static TransformFactoryRegistry::Add<ForLoopStartFromZeroFactory>
X( "from-zero", "fill in");

// This anchor is used to force the linker to link in the generated object file
// and thus register the factory.
volatile int ForLoopStartFromZeroTransformAnchorSource = 0;
