//===-- UseLocalIteratorInForLoop/UseLocalIteratorInForLoopMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseLocalIteratorInForLoopMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseLocalIteratorInForLoopID = "matcherUseLocalIteratorInForLoopID";
const char *MatcherForStmtID = "matcherForStmtID";
const char *MatcherDeclRefID = "matcherDeclRefID";

#if 0
StatementMatcher makeUseLocalIteratorInForLoopMatcher(){
    return forStmt(
	    hasLoopInit(
		binaryOperator(
		    hasOperatorName("="),
		    hasLHS(
			declRefExpr()
		    )
		).bind(MatcherUseLocalIteratorInForLoopID)
	    )
    );
}
#endif

DeclarationMatcher makeUseLocalIteratorInForLoopMatcher(){
    return functionDecl(
	    forEachDescendant(
		forStmt(
		    hasLoopInit(
			binaryOperator(
			    hasLHS(
				declRefExpr().bind(MatcherDeclRefID)
			    )
			)
		    )
		).bind(MatcherForStmtID)	    
	    )
    ).bind(MatcherUseLocalIteratorInForLoopID);
}














