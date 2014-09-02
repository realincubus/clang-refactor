//===-- UseEmpty/UseEmptyActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseEmptyFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseEmptyActions.h"
#include "UseEmptyMatchers.h"

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
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, std::string replacement ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }
}
}

UseEmptyFixer::UseEmptyFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseEmptyFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseEmptyID);
  if ( node ) {
      cout << "found node " << endl;
      if ( !Owner.isInRange( node, SM ) ) return;

      const CXXMemberCallExpr* size_side = nullptr;
      const Expr* zero_side = nullptr;

      if ( ( size_side = dyn_cast_or_null<CXXMemberCallExpr>(node->getLHS()) ) ){
	zero_side = node->getRHS();
      }else{
	size_side = dyn_cast_or_null<CXXMemberCallExpr>(node->getRHS());
	zero_side = node->getLHS();
      }

      auto* member_call_expr = size_side;
      auto* callee = member_call_expr->getCallee();
      auto* member_expr = dyn_cast_or_null<MemberExpr>(callee);
      auto member_loc = member_expr->getMemberLoc();
      // replace member call
      {
	  SourceLocation StartLoc = member_loc;
	  SourceLocation EndLoc = member_loc;
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "empty" );
      }
      // replace operator
      {
	  SourceLocation StartLoc = node->getOperatorLoc();
	  SourceLocation EndLoc = node->getOperatorLoc();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "" );
      }
      
      // replace lhs
      {
	  SourceLocation StartLoc = zero_side->getLocStart();
	  SourceLocation EndLoc = zero_side->getLocEnd();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "" );
      }

  }

}













