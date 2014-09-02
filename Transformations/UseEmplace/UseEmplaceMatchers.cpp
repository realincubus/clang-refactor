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

namespace clang {
    namespace ast_matchers{
	AST_MATCHER_P(Stmt, bindTemporaryExpr2, internal::Matcher<Expr>, Internal) {
	    llvm::errs() << "bind2 matcher called\n";
	    const auto* ptr = dyn_cast_or_null<const CXXBindTemporaryExpr>(&Node);
	    if ( !ptr ) {
		llvm::errs() << "node is not a CXXBindTemporaryExpr \n" ;
		llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
		Node.dumpColor();
		return false;
	    }
	    if ( !Internal.matches(*ptr->getSubExpr(), Finder, Builder) ) {
		llvm::errs() << "submatcher does not match \n" ;
		return false;
	    }

	    return true;
	}
	AST_MATCHER_P(CXXRecordDecl, hasType2, internal::Matcher<Type>, Internal) {
	    llvm::errs() << "hasType2 matcher called\n";
	    const auto* ptr = dyn_cast_or_null<const CXXRecordDecl>(&Node);
	    if ( !ptr ) {
		llvm::errs() << "node is not a CXXRecordDecl \n" ;
		Node.dumpColor();
		return false;
	    }
	    if ( !Internal.matches(*ptr->getTypeForDecl(), Finder, Builder) ) {
		llvm::errs() << "submatcher does not match \n" ;
		return false;
	    }else{
		ptr->getTypeForDecl()->dump();
	    }

	    return true;
	}
	AST_MATCHER(Stmt, printSubtree) {
	    llvm::errs() << "printSubtree\n";
	    llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
	    Node.dumpColor();
	    return false;
	}
	AST_MATCHER(CXXRecordDecl, printSubtreeDecl) {
	    llvm::errs() << "printSubtreeDecl\n";
	    //llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
	    Node.dumpColor();
	    return false;
	}

    }
}



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
	    //materializeTemporaryExpr2(
		bindTemporaryExpr2(
		    //printSubtree()
		    temporaryObjectExpr(
			hasDeclaration(
			    constructorDecl(
				ofClass(
				    hasType2( 
					type(
					    equalsBoundNode("container_element_type")
					)
				    )
				)
#if 0
				,
				hasType(
				    type(
					equalsBoundNode("container_element_type")
				    )
				)
#endif
#if 0
#endif
			    )
			)
		    )
		)
	    //)
#endif
	)
    ).bind(MatcherUseEmplaceID);
}












