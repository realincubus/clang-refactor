//===-- UseRAII/UseRAIIMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseRAIIMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseRAIIID = "matcherUseRAIIID";
const char *MatcherDeclRef = "matcherDeclRef";
const char *MatcherBinOp = "matcherBinOp";


StatementMatcher makeUseRAIIMatcher(){
#if 0
    return binaryOperator(
		hasLHS(
		    declRefExpr()
		    )
	    ).bind(MatcherUseRAIIID);
#endif
#if 1
    return compoundStmt(
	    forEach(
	    //hasDescendant(
		binaryOperator(
		    hasOperatorName("="),
		    hasLHS(
			declRefExpr(
			    
			).bind(MatcherDeclRef)
		    ),
		    hasRHS(
			anything()
		    )
		).bind(MatcherBinOp)
	    )
	    
    ).bind(MatcherUseRAIIID);
#endif
}












