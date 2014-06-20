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
#include "Core/MatcherUtils.hpp"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseAlgorithmsID = "matcherUseAlgorithmsID";


// TODO add a way to look through compoundstatements with only one statement in it
StatementMatcher makeLoopInitMatcher(){
    return declStmt(
		hasSingleDecl(
		    varDecl(
			hasInitializer(
			    ignoringParenImpCasts(
				integerLiteral().bind("start_int")
			    )
			)
		    ).bind("iterator_var")
		)
	    );
}

StatementMatcher makeConditionMatcher() {
    return binaryOperator(
		hasOperatorName("<"),
		hasRHS(
		    ignoringParenImpCasts(
			integerLiteral().bind("end_int")
		    )
		)
	    );
}

StatementMatcher makeIteratorReferenceMatcher() {
    return arraySubscriptExpr(
		hasIndex(
		    declRefExpr().bind("iterator_ref")
		)
	    );
}

// TODO also match if there is a single statement in a compound expression
StatementMatcher makeFillandIotaMatcher(){
    return binaryOperator(
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
					).bind("referenced_var")
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
		    );

}

StatementMatcher makeCountMatcher(){
    return ifStmt(
#if 1
#if 1
		hasThen(
		    unaryOperator(
			hasOperatorName("++")
#if 1
#if 0
			,
			print()
#endif
			,
			hasUnaryOperand(
			    declRefExpr().bind("counter")
			)
#endif
		    )
		)
		,
#endif
#if 1
		hasCondition(
		    //anyOf(
#if 0
			binaryOperator(
			    hasOperatorName("=="),
			    hasRHS(
				ignoringParenImpCasts(
				    integerLiteral().bind("count_this")
				)
			    ),
			    hasLHS(
				makeIteratorReferenceMatcher()
			    )
			),
#endif
			binaryOperator(
			    hasOperatorName("=="),
			    hasLHS(
				ignoringParenImpCasts(
				    arraySubscriptExpr(
					
				    ).bind("array")
				)
			    ),
			    hasRHS(
				ignoringParenImpCasts(
				    integerLiteral().bind("count_this")
				)
			    )
			)
		    //)
		)
#endif
#endif
	    );
}

StatementMatcher makeUseAlgorithmsMatcher(){
    return forStmt(
	hasLoopInit(
	   makeLoopInitMatcher() 
	),
	hasCondition(
	    makeConditionMatcher()
	),
	hasBody(
	    anyOf(
		makeCountMatcher(),
		makeFillandIotaMatcher()
		
	    )
	)
    ).bind(MatcherUseAlgorithmsID);
}










