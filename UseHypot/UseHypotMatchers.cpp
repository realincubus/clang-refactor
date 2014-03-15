//===-- UseHypot/UseHypotMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseHypotMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseHypotID = "matcherUseHypotID";


// search sqrt(pow(a,2) + pow(b,2))
StatementMatcher makeUseHypotMatcher(){
    return callExpr(
	callee(
	    functionDecl(
		hasName("sqrt")
	    )
	),
	hasArgument(0,
	    binaryOperator(
	    hasOperatorName("+"),
	    hasRHS(
		callExpr(
		    hasArgument(1,integerLiteral(equals(2))),
		    callee(
			functionDecl(
			    hasName("pow")
			)
		    )
		)
	    ),
	    hasLHS(
		callExpr(
		    hasArgument(1,integerLiteral(equals(2))),
		    callee(
			functionDecl(
			    hasName("pow")
			)
		    )
		)
	    )
	    )
	)
    ).bind(MatcherUseHypotID);
}












