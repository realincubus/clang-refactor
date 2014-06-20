//===-- ForLoopStartFromZero/ForLoopStartFromZeroActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the ForLoopStartFromZeroFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "ForLoopStartFromZeroActions.h"
#include "ForLoopStartFromZeroMatchers.h"

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

ForLoopStartFromZeroFixer::ForLoopStartFromZeroFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void ForLoopStartFromZeroFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherForLoopStartFromZeroID);
  const auto* literal = Result.Nodes.getNodeAs<IntegerLiteral>(MatcherInitID);
  //const auto* loop_var = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherForLoopVariableID);
  const auto* comparism_operator = Result.Nodes.getNodeAs<BinaryOperator>("comparism_operator");
  if ( node ) {
      llvm::errs() << "found a node\n" ;
      if ( !Owner.isInRange( node, SM ) ) return;

      // replace the literal with 0
      {
	  assert( literal && "somehow null" );	
	  SourceLocation StartLoc = literal->getLocStart();
	  SourceLocation EndLoc = literal->getLocEnd();
	  string replacement = "0";
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
      // replace the comparism statement
      {
	  assert( comparism_operator && "somehow null" );
	  auto pos = comparism_operator->getOperatorLoc();
	  SourceLocation StartLoc = pos;
	  SourceLocation EndLoc = pos;
	  string replacement = "<";
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
      
      // replace the index reference
      {
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();
	  string replacement = string("(") + getString( node, SM ) + string(" + ") + getString( literal , SM ) + string(")");
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
  }

}














