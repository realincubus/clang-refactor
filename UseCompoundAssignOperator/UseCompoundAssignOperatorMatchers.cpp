//===-- UseCompoundAssignOperator/UseCompoundAssignOperatorMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseCompoundAssignOperatorMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseCompoundAssignOperatorID = "matcherUseCompoundAssignOperatorID";

StatementMatcher makeUseCompoundAssignOperatorMatcher(){
    return binaryOperator(
	hasOperatorName("="),
	hasRHS(
		binaryOperator(
		    anyOf(  
			hasOperatorName("+"),
			hasOperatorName("*"),
			hasOperatorName("-"),
			hasOperatorName("/"),
			hasOperatorName("%")
#if 0
			hasOperatorName("<<"),
			hasOperatorName(">>"),
			hasOperatorName("&"),
			hasOperatorName("&"),
			hasOperatorName("^"),
			hasOperatorName("|")
#endif
		    )
		)
	      )
    ).bind(MatcherUseCompoundAssignOperatorID);
}











