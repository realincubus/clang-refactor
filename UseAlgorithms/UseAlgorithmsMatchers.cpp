//===-- UseAlgorithms/UseAlgorithmsMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseAlgorithmsMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseAlgorithmsID = "matcherUseAlgorithmsID";

// TODO add a way to look through compoundstatements with only one statement in it

StatementMatcher makeUseAlgorithmsMatcher(){
    return forStmt(
	hasLoopInit(
	    declStmt(
		hasSingleDecl(
		    varDecl(
			hasInitializer(
			    ignoringParenImpCasts(
				integerLiteral().bind("start_int")
			    )
			)
		    ).bind("iterator_var")
		)
	    )
	),
	hasCondition(
	    binaryOperator(
		hasOperatorName("<"),
		hasRHS(
		    ignoringParenImpCasts(
			integerLiteral().bind("end_int")
		    )
		)
	    )
	),
	hasBody(
	    binaryOperator(
		hasOperatorName("="),
		hasRHS(
		    anyOf(
			ignoringParenImpCasts( // TODO check whether it is constexpr or const or if it is a reference to the loop var in this case it transforms to iota
			    integerLiteral(

			    ).bind("fill_int")
			),
			ignoringParenImpCasts(
			    declRefExpr(
				to(
				    varDecl(
#if 1
					// TODO this does not work
					equalsBoundNode("iterator_var")
#endif
				    )
				)
			    ).bind("iota_var")
			)
		    )
		),
		hasLHS(
		    arraySubscriptExpr(
#if 0
			hasIndex(
			    ignoringParenImpCasts(
				declRefExpr(
				    to(
					varDecl(
					    equalsBoundNode("iterator_var")
					)
				    )
				)
			    )
			)
#endif
		    ).bind("array")
		)
	    )
	)
    ).bind(MatcherUseAlgorithmsID);
}












