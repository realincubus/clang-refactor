//===-- UseSwap/UseSwapMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseSwapMatchers.h"
#include "clang/AST/ASTContext.h"
#include "Core/MatcherUtils.hpp"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseSwapID = "matcherUseSwapID";

StatementMatcher makeSwapMatcher(){
    return stmt(
	binaryOperator(
	    print("mid expression"),
	    hasOperatorName("="),
	    hasLHS(
		ignoringParenImpCasts(
		    declRefExpr(to(decl().bind("op_a_decl"))).bind("op_a")
		)
	    ),
	    hasRHS(
		ignoringParenImpCasts(
		    declRefExpr(to(decl().bind("op_b_decl"))).bind("op_b")
		)
	    )
	),
	predecessorStmt(
	    binaryOperator(
		print("predecessorStmt"),
		hasOperatorName("="),
		hasLHS(
		    ignoringParenImpCasts(
			declRefExpr() // should be the temporary
		    )
		),
		hasRHS(
		    ignoringParenImpCasts(
			declRefExpr(to(decl(equalsBoundNode("op_a_decl"))))
		    )
		)
	    ).bind("predecessor_stmt")
	),
	succecessorStmt(
	    binaryOperator(
		print("succecessorStmt"),
		hasOperatorName("="),
		hasLHS(
		    ignoringParenImpCasts(
			declRefExpr(to(decl(equalsBoundNode("op_b_decl"))))
		    )
		),
		hasRHS(
		    ignoringParenImpCasts(
			declRefExpr() // should be the temporary
		    )
		)
	    ).bind("successor_stmt")
	)
    ).bind(MatcherUseSwapID);
}

StatementMatcher makeUseSwapMatcher(){
    return makeSwapMatcher();
}












