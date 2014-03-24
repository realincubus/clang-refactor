//===-- RemoveRedudantConditional/RemoveRedudantConditionalActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the RemoveRedudantConditionalFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "RemoveRedudantConditionalActions.h"
#include "RemoveRedudantConditionalMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

#include <iostream>

#include "RedundantContionalOperatorRule.hpp"

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;

namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* argument ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string replacement = getString( argument, SM ); 

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }
}
}

RemoveRedudantConditionalFixer::RemoveRedudantConditionalFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void RemoveRedudantConditionalFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<ConditionalOperator>(MatcherRemoveRedudantConditionalID);
  RedundantConditionalOperatorRule rule;
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      const Expr* replacement = nullptr;
      auto result = rule.VisitConditionalOperator( (ConditionalOperator*)node, &context );

      if ( result == RedundantConditionalOperatorRule::ViolationType::NoViolation ){
	return;
      }
      if ( result == RedundantConditionalOperatorRule::ViolationType::RedundantCondition ){
	 replacement = node->getCond();
      }
      if ( result == RedundantConditionalOperatorRule::ViolationType::SameValue ){
	 replacement = node->getTrueExpr();
      }
      {
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();

	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
  }

}














