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


StatementMatcher inSingleLineCompoundStmt( StatementMatcher innerMatcher ){
    return anyOf(
	    compoundStmt(
		statementCountIs(1),
		hasAnySubstatement(
		    innerMatcher
		)
	    ).bind("is_in_compound"),
		innerMatcher
	    );

}

// TODO add a way to look through compoundstatements with only one statement in it
StatementMatcher makeLoopInitMatcher(){
    return inSingleLineCompoundStmt(
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
    );
}

StatementMatcher makeConditionMatcher() {
    return inSingleLineCompoundStmt(
	    binaryOperator(
		hasOperatorName("<"),
		hasRHS(
		    ignoringParenImpCasts(
			integerLiteral().bind("end_int")
		    )
		)
	    )
    );
}

StatementMatcher makeIteratorReferenceMatcher() {
    return inSingleLineCompoundStmt(
	    arraySubscriptExpr(
		hasIndex(
		    declRefExpr().bind("iterator_ref")
		)
	    )
    );
}

// TODO also match if there is a single statement in a compound expression
StatementMatcher makeFillandIotaMatcher(){
    return inSingleLineCompoundStmt(
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
		    )
    );

}

StatementMatcher makeCountMatcher(){
    return inSingleLineCompoundStmt(
	    ifStmt(
		hasThen(
		    unaryOperator(
			hasOperatorName("++"),
			hasUnaryOperand(
			    declRefExpr().bind("counter")
			)
		    )
		),
		// TODO make this look better 
		// already implemented isCommutative but this does not work with 
		// bind
		hasCondition(
		    anyOf(
			binaryOperator(
			    hasOperatorName("=="),
			    hasRHS(
				ignoringParenImpCasts(
				    arraySubscriptExpr().bind("array")
				)
			    ),
			    hasLHS(
				ignoringParenImpCasts(
				    integerLiteral().bind("count_this")
				)
			    )
			),
			binaryOperator(
			    hasOperatorName("=="),
			    hasLHS(
				ignoringParenImpCasts(
				    arraySubscriptExpr().bind("array")
				)
			    ),
			    hasRHS(
				ignoringParenImpCasts(
				    integerLiteral().bind("count_this")
				)
			    )
			)
		    )
		)
	    )
    );
}

// TODO add checking for index
StatementMatcher makeCopyMatcher() {
    return inSingleLineCompoundStmt(
	    binaryOperator(
		hasOperatorName("="),
		hasLHS(
		    ignoringParenImpCasts(
			arraySubscriptExpr().bind("copy_destination")
		    )
		),
		hasRHS(
		    ignoringParenImpCasts(
			arraySubscriptExpr().bind("array")
		    )
		)
	    )
    );
}


StatementMatcher makeAccumulateMatcher(){
    return inSingleLineCompoundStmt( binaryOperator(
	    hasOperatorName("+="),
	    hasLHS(
		ignoringParenImpCasts(
		    declRefExpr().bind("accumulate_counter")
		)
	    ),
	    hasRHS(
		ignoringParenImpCasts(
		    arraySubscriptExpr().bind("array")
		)
	    )
	) 
    );
}

StatementMatcher makeCopyToIteratorMatcher(){
    return memberCallExpr(
	on(
	    declRefExpr().bind("copy_destination")
	),
	argumentCountIs(1),
	hasArgument(0, 
	    ignoringParenImpCasts(
		arraySubscriptExpr().bind("array")
	    )
	),
	callee(
	    methodDecl(
		anyOf(
		    hasName("push_back"),
		    hasName("push_front")
		)
	    ).bind("insertion_type")
	)
    );
}
#if 0
StatementMatcher makeAllOfMatcher(){
    return if ( 
}
#endif

#if 0
StatementMatcher ignoreOneLineCompoundStatement( StatementMatcher Submatcher ){
    return statementCountIs(1)
}
#endif

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
		makeFillandIotaMatcher(),
		makeCopyMatcher(),
		makeAccumulateMatcher(),
		makeCopyToIteratorMatcher()
	    )
	)
    ).bind(MatcherUseAlgorithmsID);
}










