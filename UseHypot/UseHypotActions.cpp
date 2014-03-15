//===-- UseHypot/UseHypotActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseHypotFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseHypotActions.h"
#include "UseHypotMatchers.h"

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
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* arg1, const Expr* arg2 ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);
  
  string source_text_arg1 = getString( arg1, SM );
  string source_text_arg2 = getString( arg2, SM );
  string replacement_text = string("hypot(") + source_text_arg1 + string(", ") + source_text_arg2 + string(")") ; 

  Owner.addReplacementForCurrentTU(tooling::Replacement(SM, Range, replacement_text ));
}
}

UseHypotFixer::UseHypotFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseHypotFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const CallExpr* sqrtFunctionCall = Result.Nodes.getNodeAs<CallExpr>(MatcherUseHypotID);
  if ( sqrtFunctionCall ) {
    cout << "pow pow op" << endl;

    SourceLocation StartLoc = sqrtFunctionCall->getLocStart();
    SourceLocation EndLoc = sqrtFunctionCall->getLocEnd();

    const BinaryOperator* multop = static_cast<const BinaryOperator * >(sqrtFunctionCall->getArg(0));
    const CallExpr* lhs_call = static_cast<const CallExpr*>( multop->getLHS() );
    const Expr* arg1 = static_cast<const Expr*>( lhs_call->getArg(0) );

    const CallExpr* rhs_call = static_cast<const CallExpr*>( multop->getRHS() );
    const Expr* arg2 = static_cast<const Expr*>( rhs_call->getArg(0) );
    if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){
	ReplaceWith( Owner, SM, StartLoc, EndLoc, context, arg1, arg2 );
    }
  }

}














