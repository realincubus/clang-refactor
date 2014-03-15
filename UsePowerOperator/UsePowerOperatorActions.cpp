//===-- UsePowerOperator/TransformationTemplateActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UsePowerOperatorFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UsePowerOperatorActions.h"
#include "UsePowerOperatorMatchers.h"

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

string pow_func_call = string("pow(") + source_text + string(",2)");

Owner.addReplacementForCurrentTU(
      tooling::Replacement(SM, Range, pow_func_call ));
      //tooling::Replacement(SM, InsertionPoint, 0, " [[deprecated]]"));
}

/// \brief RecursiveASTVisitor for ensuring all nodes rooted at a given AST
/// subtree that have file-level source locations corresponding to a macro
/// argument have implicit NullTo(Member)Pointer nodes as ancestors.

} // namespace

UsePowerOperatorFixer::UsePowerOperatorFixer(unsigned &AcceptedChanges,
                           llvm::ArrayRef<llvm::StringRef> UserMacros,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UsePowerOperatorFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherID);
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();
  if ( binaryOperator ) {
      if (areSameExpr(&context,binaryOperator->getLHS(),binaryOperator->getRHS()) ){
	cout << "found multop with same expression on both sides" << endl;
	SourceLocation StartLoc = binaryOperator->getLocStart();
	SourceLocation EndLoc = binaryOperator->getLocEnd();
	if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ) {
	    ReplaceWith( Owner, SM, StartLoc, EndLoc, context, binaryOperator->getLHS());
	}
      }
  }
}














