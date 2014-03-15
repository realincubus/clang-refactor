//===-- UsePowerOperator/TransformationTemplateMatchers.cpp - Matchers for null casts ----------===//
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

#include "UsePowerOperatorMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherID = "matcherID";

namespace clang {
namespace ast_matchers {


} // end namespace ast_matchers
} // end namespace clang


StatementMatcher makeMatcher(){
    return binaryOperator(
	hasOperatorName("*")
    ).bind(MatcherID);
}

#if 0
StatementMatcher makePowMultMatcher(){
    return binaryOperator(
	hasOperatorName("*"),
	anyOf(
	    hasLHS(
		callExpr(
		    hasArgument(1,integerLiteral(equals(2))),
		    callee(
			functionDecl(
			    hasName("pow")
			)
		    )
		)
	    ),
	    hasRHS(
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
    ).bind(MatcherID);
}
#endif









