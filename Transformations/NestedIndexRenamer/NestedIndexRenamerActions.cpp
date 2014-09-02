//===-- NestedIndexRenamer/NestedIndexRenamerActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the NestedIndexRenamerFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "NestedIndexRenamerActions.h"
#include "NestedIndexRenamerMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

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

NestedIndexRenamerFixer::NestedIndexRenamerFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void NestedIndexRenamerFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* index_var_decl = Result.Nodes.getNodeAs<VarDecl>("index_decl");
  const auto* nested_var_decl = Result.Nodes.getNodeAs<VarDecl>("nested_index_decl");

  if ( index_var_decl && nested_var_decl ) {
      if ( !Owner.isInRange( index_var_decl, SM ) ) return;

      auto index_name = index_var_decl->getNameAsString();
      auto nested_index_name = nested_var_decl->getNameAsString();

      if ( index_name.compare( nested_index_name ) == 0 ) {
	  auto StartLoc = index_var_decl->getLocStart();
	  auto StartLocNested = nested_var_decl->getLocStart();
	  auto pos = StartLoc.printToString( SM );
	  auto posNested = StartLocNested.printToString( SM );
	  llvm::errs() << "the loop at " << pos << " has nested index in " << posNested << "\n"; 
      }


#if 0
      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, node );
#endif
  }

}














