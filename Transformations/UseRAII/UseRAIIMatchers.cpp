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
#include "Core/MatcherUtils.hpp"

using namespace clang::ast_matchers;
using namespace clang;


StatementMatcher makeUseRAIIMatcher(){
    return stmt(
	    binaryOperator(
		hasOperatorName("="),
		hasLHS(
		    declRefExpr(
			to(
			    varDecl().bind("refs_decl")
			)
		    )
		)
	    ).bind("binary_operator"),
	    predecessorStmt(
		declStmt(
		    hasSingleDecl(
			varDecl(
			    equalsBoundNode("refs_decl"),
			    hasLocalStorage()
			).bind("decl")
		    )
		).bind("decl_stmt")
	    )
    ).bind("stmt");
}












