//===-- UseEmplace/UseEmplaceMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseEmplaceMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseEmplaceID = "matcherUseEmplaceID";


StatementMatcher makeUseEmplaceMatcher(){
    return memberCallExpr(
	argumentCountIs(1),
	callee(
	    methodDecl(
		ofClass(
		    classTemplateSpecializationDecl(
			hasTemplateArgument(0,refersToType(type().bind("container_element_type")))
		    )
		),
		hasName("push_back")	
	    )
	),
	hasArgument(0,  
#if 1
	    materializeTemporaryExpr(
#if 0
		bindTemporaryExpr(
		    temporaryObjectExpr(
			hasDeclaration(
			    constructorDecl(
				    hasType(
					type(
					    equalsBoundNode("container_element_type")
					)
				    )
				)
			    )
		    )
		)
#endif
	    )
#endif
	)
    ).bind(MatcherUseEmplaceID);
}












