//===-- FuseToSimdVector/FuseToSimdVectorMatchers.cpp - Matchers for null casts ----------===//
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

#include "FuseToSimdVectorMatchers.h"
#include "clang/AST/ASTContext.h"
#include "Core/MatcherUtils.hpp"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherFuseToSimdVectorID = "matcherFuseToSimdVectorID";

StatementMatcher makeDeclStatementMatcher() {
    return  declStmt(
	    declCountIs(3)
	).bind(MatcherFuseToSimdVectorID);
}

StatementMatcher makeFuseToSimdVectorMatcher(){
    return anyOf(
	    makeDeclStatementMatcher(),
	    // TODO gather all references to the variable declaration in the declStmt
	    declRefExpr(
		to(
		    varDecl(
			hasParent( 
			    makeDeclStatementMatcher()
			)
		    ).bind("refto")
		)
	    ).bind("declrefexpr")
	);
}












