//===-- UseSwap/UseSwapActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseSwapFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseSwapActions.h"
#include "UseSwapMatchers.h"

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

UseSwapFixer::UseSwapFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseSwapFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseSwapID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;
      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();

      auto swap_op_a = Result.Nodes.getNodeAs<DeclRefExpr>("op_a");
      auto swap_op_b = Result.Nodes.getNodeAs<DeclRefExpr>("op_b");
      auto predecessor = Result.Nodes.getNodeAs<BinaryOperator>("predecessor_stmt"); 
      auto successor = Result.Nodes.getNodeAs<BinaryOperator>("successor_stmt"); 
      // doing the swap transformation
      if (!(swap_op_a && swap_op_b && predecessor && successor) ){
	  llvm::errs() << "did not find the operands\n";
	  return;
      }
      auto op_a = getString( swap_op_a, SM );
      auto op_b = getString( swap_op_b, SM );

      string replacement = "std::swap(" + op_a + string(", ") + op_b + ")";

      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );

      {
	  StartLoc = predecessor->getLocStart();
	  EndLoc = predecessor->getLocEnd();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "" );
      }
      {
	  StartLoc = successor->getLocStart();
	  EndLoc = successor->getLocEnd();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "" );
      }
  }

}














