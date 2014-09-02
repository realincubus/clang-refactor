//===-- UseCompoundAssignOperator/UseCompoundAssignOperatorActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseCompoundAssignOperatorFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseCompoundAssignOperatorActions.h"
#include "UseCompoundAssignOperatorMatchers.h"

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


void ReplaceWithCompoundAssignment(Transform &Owner, SourceManager &SM, SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* rhs, const Expr* lhs, std::string operator_text ){ 
  using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text_rhs = getString( rhs, SM );
  string source_text_lhs = getString( lhs, SM );

  string replacement_text = source_text_lhs + string(" ") + operator_text + string("= ") + source_text_rhs; 

  Owner.addReplacementForCurrentTU(tooling::Replacement(SM, Range, replacement_text ));
}

UseCompoundAssignOperatorFixer::UseCompoundAssignOperatorFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

void UseCompoundAssignOperatorFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;

  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const BinaryOperator* assignOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseCompoundAssignOperatorID);
  if ( !Owner.isInRange( assignOperator, SM ) ) return;

  // FIXME CRITICAL / and - are not commutative
  if ( assignOperator ) {
    cout << "found a assign op" << endl;
    const Expr* lhs = assignOperator->getLHS();
    const Expr* rhs = assignOperator->getRHS();
    const BinaryOperator* basicOperator = static_cast<const BinaryOperator*>(rhs);

    SourceLocation slb = basicOperator->getOperatorLoc();
    SourceLocation sle = slb.getLocWithOffset(1);
    std::string operator_text = Lexer::getSourceText( CharSourceRange::getTokenRange(SourceRange(slb, sle)), SM, LangOptions());

    SourceLocation StartLoc = assignOperator->getLocStart();
    SourceLocation EndLoc = assignOperator->getLocEnd();

    if ( areSameExpr( &context, lhs, basicOperator->getLHS()->IgnoreParenImpCasts() ) ){
	if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ) {
	    ReplaceWithCompoundAssignment( Owner, SM, StartLoc, EndLoc, context, basicOperator->getRHS(), lhs, operator_text );
	}
    }
    if ( areSameExpr( &context, lhs, basicOperator->getRHS()->IgnoreParenImpCasts() ) ){
	if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ) {
	    ReplaceWithCompoundAssignment( Owner, SM, StartLoc, EndLoc, context, basicOperator->getLHS(), lhs, operator_text );
	}
    }  
  }

}














