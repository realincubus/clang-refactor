//===-- UnInvertLogic/UnInvertLogicActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UnInvertLogicFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UnInvertLogicActions.h"
#include "UnInvertLogicMatchers.h"

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

UnInvertLogicFixer::UnInvertLogicFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

class InvertedLogicRule
{
private:
    static bool isInvertedLogic( const Expr *condExpr)
    {
        auto* binaryOperator = dyn_cast_or_null<const BinaryOperator>(condExpr);
        auto* unaryOperator = dyn_cast_or_null<const UnaryOperator>(condExpr);
        return (binaryOperator && binaryOperator->getOpcode() == BO_NE) ||
            (unaryOperator && unaryOperator->getOpcode() == UO_LNot);
    }

public:
    static bool VisitIfStmt(const IfStmt *ifStmt)
    {
        if (ifStmt->getElse() && isInvertedLogic(ifStmt->getCond()))
        {
	    return true;
        }

        return false;
    }

    static bool VisitConditionalOperator(const ConditionalOperator *conditionalOperator)
    {
        if (isInvertedLogic(conditionalOperator->getCond()))
        {
	    return true;
        }

        return false;
    }
};

void UnInvertLogicFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<IfStmt>(MatcherUnInvertLogicID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      if ( InvertedLogicRule::VisitIfStmt( node ) ) {
	// TODO invert the branches
	auto* then_branch = node->getThen();
	auto* else_branch = node->getElse();
	if ( else_branch ) {

	    auto* condition = node->getCond();
	    auto* binaryOperator = static_cast<const BinaryOperator*>(condition);
	    if ( !binaryOperator ) {
		llvm::errs() << "binaryOperator is nullptr \n";
		exit(-1);
	    }
	    auto replace_op = string("==");

	    SourceLocation StartLoc = binaryOperator->getOperatorLoc();
	    SourceLocation EndLoc = binaryOperator->getOperatorLoc();
	    ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replace_op );


	    auto else_text = getString( else_branch, SM );
	    auto then_text = getString( then_branch, SM );
	    {
		SourceLocation StartLoc = then_branch->getLocStart();
		SourceLocation EndLoc = then_branch->getLocEnd();
		ReplaceWith( Owner, SM, StartLoc, EndLoc, context, else_text );
	    }
	    {
		SourceLocation StartLoc = else_branch->getLocStart();
		SourceLocation EndLoc = else_branch->getLocEnd();
		ReplaceWith( Owner, SM, StartLoc, EndLoc, context, then_text );
	    }
	}
      }

      //ReplaceWith( Owner, SM, StartLoc, EndLoc, context, node );
      
  }

}














