//===-- UnglobalMethod/UnglobalMethodMatchers.cpp - Matchers for null casts ----------===//
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

#include "UnglobalMethodMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherUnglobalMethodID = "matcherUnglobalMethodID";

namespace clang {
    namespace ast_matchers{
	AST_MATCHER_P( Decl, lookup, internal::Matcher<Decl>, Internal) {
	    llvm::errs() << "lookup matcher called\n";
	    const auto* ptr = dyn_cast_or_null<const DeclContext>(&Node);
	    if ( !ptr ) {
		llvm::errs() << "node is not a DeclContext \n" ;
		//Node.dumpColor();
		return false;
	    }
	    for( auto declare_it = ptr->decls_begin(), end = ptr->decls_end(); 
		    declare_it != end; 
		    declare_it++ ){
		if ( Internal.matches( *(*declare_it), Finder, Builder) ) {
		    return true;
		}
	    }
	    return false;
	}
    }
}

DeclarationMatcher makeUnglobalMethodMatcher(){
    return functionDecl(
	forEachDescendant(
	    declRefExpr(to(decl().bind("var")))
	),
	hasDeclContext(
	    lookup(
		equalsBoundNode("var")
	    )
	)
    ).bind(MatcherUnglobalMethodID);
}












