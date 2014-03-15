//===-- UseUnaryOperator/UseUnaryOperatorActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseUnaryOperatorFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseUnaryOperatorActions.h"
#include "UseUnaryOperatorMatchers.h"

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
void ReplaceWith(Transform &Owner, SourceManager &SM, SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, std::string operator_text, const Expr* var ){ 
    using namespace std;

    CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

    string source_text = getString( var, SM );
    string replacement_text = operator_text.substr(0,1) + operator_text.substr(0,1) + source_text; 

    Owner.addReplacementForCurrentTU(
	tooling::Replacement(SM, Range, replacement_text ));
}
}

UseUnaryOperatorFixer::UseUnaryOperatorFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}



void UseUnaryOperatorFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseUnaryOperatorID);
  if( binaryOperator ){
    cout << "found inc dec equal op" << endl;
    const Expr* lhs = binaryOperator->getLHS();

    SourceLocation StartLoc = binaryOperator->getLocStart();
    SourceLocation EndLoc = binaryOperator->getLocEnd();

    SourceLocation slb = binaryOperator->getOperatorLoc();
    SourceLocation sle = slb.getLocWithOffset(1);
    std::string operator_text = Lexer::getSourceText( CharSourceRange::getTokenRange(SourceRange(slb, sle)), SM, LangOptions());

    if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){
	ReplaceWith( Owner, SM, StartLoc, EndLoc, context, operator_text, lhs );
    }

  }

}














