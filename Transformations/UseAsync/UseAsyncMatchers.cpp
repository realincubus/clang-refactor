//===-- UseAsync/UseAsyncMatchers.cpp - Matchers for null casts ----------===//
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

#include "UseAsyncMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUseAsyncID = "matcherUseAsyncID";
const char *MatcherFunctionDeclID = "matcherFunctionDeclID";

namespace clang {
    namespace ast_matchers{
	AST_MATCHER(VarDecl, isStaticLocal) {
	    llvm::errs() << "isStaticLocal\n";
	    //llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
	    if( Node.isStaticLocal() ) return true;
	    return false;
	}
    }
}



// TODO if declared pure simply believe it
DeclarationMatcher makePureFunctionMatcher(){
    return functionDecl(
	forEachDescendant(
	    declRefExpr(
		hasDeclaration(
		    hasDeclContext(
			decl().bind("ref_decl")
		    )
		)
	    )
	),
	hasDescendant(
	    decl(
		equalsBoundNode("ref_decl")
	    )
	),
	// TODO if there is a static variable this is not a pure function
	unless(
	    hasDescendant(
		varDecl(
		    isStaticLocal()
		)
	    )
	),
	unless(
	    hasAnyParameter(
		anyOf(
		    hasType(
			pointerType()
		    ),
		    hasType(
			referenceType()
		    )
		)
	    )
	)
    );
}

// TODO search for all referecenes to the vardecl
//      so you can change it to asyncs get
StatementMatcher makeUseAsyncMatcher(){
    // TODO stuff the callexpr into a async call
    return callExpr(
	    callee( 
		functionDecl(
		    
		).bind(MatcherFunctionDeclID)
		//TODO add methodDecl's which dont change the class's state
	    )
	    ,
	    // check wheather used inside an initializer
	    hasParent(
		varDecl()
		// TODO save type and covert it to auto
	    )
    ).bind(MatcherUseAsyncID);
}












