//===-- UseFusedMultiplyAdd/UseFusedMultiplyAddActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseFusedMultiplyAddFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseFusedMultiplyAddActions.h"
#include "UseFusedMultiplyAddMatchers.h"

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
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* arg1, const Expr* arg2, const Expr* arg3 ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text_arg1 = getString( arg1, SM );
  string source_text_arg2 = getString( arg2, SM );
  string source_text_arg3 = getString( arg3, SM );

  string replacement_text = string("fma(") + source_text_arg1 + string(", ") + source_text_arg2 + string(", ") + source_text_arg3 + string(")") ; 

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement_text ));
}
}

UseFusedMultiplyAddFixer::UseFusedMultiplyAddFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseFusedMultiplyAddFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseFusedMultiplyAddID);
  if ( binaryOperator ) {
    cout << "fma op" << endl;

    SourceLocation StartLoc = binaryOperator->getLocStart();
    SourceLocation EndLoc = binaryOperator->getLocEnd();

    const BinaryOperator* multop_lhs = dyn_cast<const BinaryOperator>(binaryOperator->getLHS());
    if ( multop_lhs ) {
	const Expr* arg3 = binaryOperator->getRHS();
	const Expr* arg1 = multop_lhs->getLHS();
	const Expr* arg2 = multop_lhs->getRHS();
	ReplaceWith( Owner, SM, StartLoc, EndLoc, context, arg1, arg2, arg3 );
    }

    const BinaryOperator* multop_rhs = dyn_cast<const BinaryOperator>(binaryOperator->getRHS());
    if ( multop_rhs ) {
	const Expr* arg3 = binaryOperator->getLHS();
	const Expr* arg1 = multop_rhs->getLHS();
	const Expr* arg2 = multop_rhs->getRHS();
	ReplaceWith( Owner, SM, StartLoc, EndLoc, context, arg1, arg2, arg3 );
    }

  } 

}














