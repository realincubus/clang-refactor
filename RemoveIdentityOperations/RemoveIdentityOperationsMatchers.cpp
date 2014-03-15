//===-- RemoveIdentityOperations/RemoveIdentityOperationsMatchers.cpp - Matchers for null casts ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definitions for matcher-generating functions
/// and a custom AST_MATCHER for identifying casts of type CK_NullTo*.
///
//===----------------------------------------------------------------------===//

#include "RemoveIdentityOperationsMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherRemoveIdentityOperationsID_plus = "matcherRemoveIdentityOperationsID_plus";
const char *MatcherRemoveIdentityOperationsID_mult = "matcherRemoveIdentityOperationsID_mult";


StatementMatcher makeRemoveIdentityOperationsMatcherPlus(){
    return binaryOperator(
	anyOf(
	    hasOperatorName("+"),
	    hasOperatorName("-")
	),
	anyOf(
	hasRHS(
		integerLiteral(equals(0))
	),
	hasLHS(
		integerLiteral(equals(0))
	)
	)
    ).bind(MatcherRemoveIdentityOperationsID_plus);
}

StatementMatcher makeRemoveIdentityOperationsMatcherMult(){
    return binaryOperator(
	anyOf(
	    hasOperatorName("*"),
	    hasOperatorName("/")
	),
	anyOf(
	hasRHS(
	    integerLiteral(equals(1))
	),
	hasLHS(
	    integerLiteral(equals(1))
	)
	)
    ).bind(MatcherRemoveIdentityOperationsID_mult);
}








