//===-- NestedIndexRenamer/NestedIndexRenamerMatchers.cpp - Matchers for null casts ----------===//
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

#include "NestedIndexRenamerMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherNestedIndexRenamerID = "matcherNestedIndexRenamerID";


StatementMatcher makeNestedIndexRenamerMatcher(){
    return forStmt( 
	hasLoopInit(
	    anyOf(
		declStmt(
		    hasSingleDecl(
			varDecl().bind("index_decl")
		    )
		),
		binaryOperator(
		    hasOperatorName("="),
		    hasLHS(
			declRefExpr(
			    to(
				varDecl().bind("index_decl")
			    )
			)
		    )
		)
	    )
	),
	hasBody(
	    forEachDescendant(
		forStmt(
		    hasLoopInit(
			anyOf(
			    declStmt(
				hasSingleDecl(
				    varDecl(

				    ).bind("nested_index_decl")
				)
			    ),
			    binaryOperator(
				hasOperatorName("="),
				hasLHS(
				    declRefExpr(
					to(
					    varDecl().bind("nested_index_decl")
					)
				    )
				)
			    )
			)
		    )
		)
	    )
	)
    );
}












