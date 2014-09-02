//===-- RemoveIdentityOperations/RemoveIdentityOperationsActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the RemoveIdentityOperationsFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "RemoveIdentityOperationsActions.h"
#include "RemoveIdentityOperationsMatchers.h"

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
  string pow_func_call = string("pow(") + source_text + string(",2)"); 

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, pow_func_call ));
}
}

RemoveIdentityOperationsFixer::RemoveIdentityOperationsFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void RemoveIdentityOperationsFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  {
      const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherRemoveIdentityOperationsID_mult);
      if ( binaryOperator ) {
	cout << "found mult div by one" << endl;
	const Expr* lhs = binaryOperator->getLHS();
	const Expr* rhs = binaryOperator->getRHS();

	SourceLocation StartLoc = binaryOperator->getLocStart();
	SourceLocation EndLoc = binaryOperator->getLocEnd();
	if ( !isReplaceableRange(StartLoc,EndLoc,SM,Owner) ) {
	    return;
	}
	
	if ( dyn_cast<IntegerLiteral>(lhs) ){
	    ReplaceWithSelf( Owner, SM, StartLoc, EndLoc, context, rhs );
	}

	if ( dyn_cast<IntegerLiteral>(rhs) ){
	    ReplaceWithSelf( Owner, SM, StartLoc, EndLoc, context, lhs );
	}
      }
  }
  {
      const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherRemoveIdentityOperationsID_plus);
      if ( binaryOperator ) {
	cout << "found plus sub 0" << endl;
	const Expr* lhs = binaryOperator->getLHS();
	const Expr* rhs = binaryOperator->getRHS();

	SourceLocation StartLoc = binaryOperator->getLocStart();
	SourceLocation EndLoc = binaryOperator->getLocEnd();

	if ( !isReplaceableRange(StartLoc,EndLoc,SM,Owner) ) {
	    return;
	}
	
	if ( dyn_cast<IntegerLiteral>(lhs) ){
	    ReplaceWithSelf( Owner, SM, StartLoc, EndLoc, context, rhs );
	}

	if ( dyn_cast<IntegerLiteral>(rhs) ){
	    ReplaceWithSelf( Owner, SM, StartLoc, EndLoc, context, lhs );
	}
      }
  }

}














