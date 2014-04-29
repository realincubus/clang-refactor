//===-- UseAlgorithms/UseAlgorithmsActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseAlgorithmsFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseAlgorithmsActions.h"
#include "UseAlgorithmsMatchers.h"

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

UseAlgorithmsFixer::UseAlgorithmsFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseAlgorithmsFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  {
      const auto* node = Result.Nodes.getNodeAs<ForStmt>(MatcherUseAlgorithmsID);
      if ( node ) {
	  if ( !Owner.isInRange( node, SM ) ) return;
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();

	  auto array_subscript = Result.Nodes.getNodeAs<ArraySubscriptExpr>("array");
	  auto array_name = getString( array_subscript->getBase(), SM );
	  auto array_lb_node = Result.Nodes.getNodeAs<IntegerLiteral>("start_int");
	  auto array_low = getString( array_lb_node, SM );
	  auto array_ub_node = Result.Nodes.getNodeAs<IntegerLiteral>("end_int");
	  auto array_up = getString( array_ub_node, SM );

	  // case fill
	  auto fill_int_node = Result.Nodes.getNodeAs<IntegerLiteral>("fill_int");
	  string last_arg_text;
	  string algorithm_used;
	  if ( fill_int_node ) {
	    algorithm_used = "std::fill";
	    last_arg_text = getString( fill_int_node, SM );
	  }

	  // special case iota -> fill with acesnding numbers 
	  auto iota_var = Result.Nodes.getNodeAs<VarDecl>("referenced_var");
	  auto iterator_var = Result.Nodes.getNodeAs<VarDecl>("iterator_var");
	  if ( iota_var == iterator_var ){
	      auto iota_node = Result.Nodes.getNodeAs<DeclRefExpr>("iota_var");
	      if ( iota_node ) {
		algorithm_used = "std::iota";
		last_arg_text = array_low;
	      }
	  }

	  string replacement = algorithm_used + string("(")  
				+ string("&") + array_name + string("[") + array_low + string("], ") 
				+ string("&") + array_name + string("[") + array_up  + string("], ") + last_arg_text 
				+ string(")");
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
  }
  {
      const auto* node = Result.Nodes.getNodeAs<ForStmt>(MatcherUseAlgorithmsID);
      if ( node ) {
	  if ( !Owner.isInRange( node, SM ) ) return;
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();
	  
	  string replacement = "";
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
  }

}














