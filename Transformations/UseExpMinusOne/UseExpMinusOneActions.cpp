//===-- UseExpMinusOne/UseExpMinusOneActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseExpMinusOneFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseExpMinusOneActions.h"
#include "UseExpMinusOneMatchers.h"

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
  string pow_func_call = string("expm1(") + source_text + string(")");

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, pow_func_call ));
}
}

UseExpMinusOneFixer::UseExpMinusOneFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseExpMinusOneFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseExpMinusOneID);
  if ( binaryOperator ) {
      cout << "found exp minus 1" << endl;
      const CallExpr* lhs = static_cast<CallExpr*>(binaryOperator->getLHS());
      const Expr* argument = lhs->getArg(0);

      SourceLocation StartLoc = binaryOperator->getLocStart();
      SourceLocation EndLoc = binaryOperator->getLocEnd();

      if( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){
	ReplaceWith( Owner, SM, StartLoc, EndLoc, context, argument);
      }
  }else{
      assert( 0 && "strange" );
  }
}














