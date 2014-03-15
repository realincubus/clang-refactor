//===-- UseRound/UseRoundActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseRoundFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseRoundActions.h"
#include "UseRoundMatchers.h"

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
void ReplaceWith(Transform &Owner, SourceManager &SM, SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* argument ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text = getString( argument, SM );
  string replacement_text = string("lround(") + source_text + string(")");

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement_text ));
}
}

UseRoundFixer::UseRoundFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

void UseRoundFixer::mutualReplace( const FloatingLiteral* one_side, const Expr* other_side, SourceLocation StartLoc, SourceLocation EndLoc, SourceManager& SM, ASTContext& context){
  if ( one_side ){
      if ( one_side->getValueAsApproximateDouble() == 0.5 ) {
	    if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){
		ReplaceWith( Owner, SM, StartLoc, EndLoc, context, other_side ); 
	    }
      }
  }
}

void UseRoundFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const CXXFunctionalCastExpr* castExpr = Result.Nodes.getNodeAs<CXXFunctionalCastExpr>(MatcherUseRoundID);
  if ( castExpr ) {
      cout << "found a round op" << endl;
      SourceLocation StartLoc = castExpr->getLocStart();
      SourceLocation EndLoc = castExpr->getLocEnd();
      
      const ImplicitCastExpr* implicitCast = dyn_cast<const ImplicitCastExpr>(castExpr->getSubExpr());
      if ( implicitCast ){
	  const BinaryOperator* binaryOperator = dyn_cast<const BinaryOperator>(implicitCast->getSubExpr());
	  if ( binaryOperator ){
	      const Expr* lhs = binaryOperator->getLHS();
	      const Expr* rhs = binaryOperator->getRHS();

	      assert( lhs  && "lhs is null" );
	      assert( rhs  && "rhs is null" );

	      const FloatingLiteral* float_lhs = dyn_cast<const FloatingLiteral>( lhs );
	      const FloatingLiteral* float_rhs = dyn_cast<const FloatingLiteral>( rhs );
	      mutualReplace( float_lhs, rhs, StartLoc, EndLoc, SM, context);
	      mutualReplace( float_rhs, lhs, StartLoc, EndLoc, SM, context);
	  }

      }
  }

}














