//===-- UnglobalMethod/UnglobalMethodActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UnglobalMethodFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UnglobalMethodActions.h"
#include "UnglobalMethodMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"
#include "clang/AST/Mangle.h"

#include "Core/Utility.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;

namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* argument ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text = getString( argument, SM );
  string replacement = source_text; 

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }
}
}

UnglobalMethodFixer::UnglobalMethodFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

void UnglobalMethodFixer::dumpGlobalReferences() {
    for( auto gref : globalReferences ){
	std::string err;
	std::string fname;
	llvm::raw_string_ostream fnamestream(fname);
	fnamestream << gref.first << ".ref";
	llvm::raw_fd_ostream out(fnamestream.str().c_str(),err,llvm::sys::fs::F_None);
	for( auto& ref : gref.second ){
	    out << ref << "\n";
	}
	out.close();
    }
}

void UnglobalMethodFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  auto manglectx = context.createMangleContext();
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherUnglobalMethodID);


  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      auto filename = SM.getFilename( SM.getSpellingLoc(node->getLocStart()) );

      if ( SM.isWrittenInSameFile(node->getLocStart(),node->getDecl()->getLocStart()) ) return;

      std::string str;
      llvm::raw_string_ostream ostr(str);
      manglectx->mangleName(node->getDecl(),ostr);
      llvm::errs() << "found declRefExpr to non local variable " << str << " in " << filename << "\n";

      globalReferences[filename].push_back(ostr.str());

      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, node );
  }

}














