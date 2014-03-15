//===-- UseRound/UseRoundMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseRoundMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseRoundID = "matcherUseRoundID";


// search int( 0.5 + val ) -> round( val )
StatementMatcher makeUseRoundMatcher(){
    return functionalCastExpr(
#if 1
	hasDestinationType(
	    isInteger()
	)
#if 0
	,
	hasArgument(0,
	    binaryOperator(
		hasOperatorName("+")
		,
		anyOf(
		    hasLHS(
			floatLiteral(
			    //equals(0.5)
			)	    
		    ),
		    hasRHS(
			floatLiteral(
			    //equals(0.5)
			)	    
		    )
		)
	    )
	)	    
#endif
#endif
    ).bind(MatcherUseRoundID);
}












