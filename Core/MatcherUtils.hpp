

#pragma once


#include "clang/ASTMatchers/ASTMatchers.h"


using namespace clang::ast_matchers;
using namespace clang;

namespace clang {
namespace ast_matchers{

AST_MATCHER_P2(BinaryOperator, isCommutative , internal::Matcher<Expr>, Internal1, internal::Matcher<Expr>, Internal2 ) {

    if ( Internal1.matches( *Node.getLHS(), Finder, Builder) && Internal2.matches( *Node.getRHS(), Finder, Builder ) ) {
	return true;
    }
    if ( Internal1.matches( *Node.getRHS(), Finder, Builder) && Internal2.matches( *Node.getLHS(), Finder, Builder ) ){
	return true;
    }

    return false;
}

AST_MATCHER(Stmt, print) {
    llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
    Node.dumpColor();
    return true;
}

#if 1
AST_MATCHER_P(Stmt, ignoringOneLineCompoundStatement , internal::Matcher<Stmt>, Internal1 ) {

    auto compound_statement = dyn_cast_or_null<CompoundStmt>(&Node);
    if ( compound_statement ){
	// if the node we look at is a compound statment check wheather it 
	// stores 1 sub statement
	if ( compound_statement->size() != 1 ) return false;

	// if there is just one statement run the interal matcher on it
	if ( Internal1.matches( *(*compound_statement->body_begin()), Finder, Builder) ) {
	    return true;
	}
    }else{
	// if the statement is not a compound statement 
	// run the internal matcher on the node itself
	if ( Internal1.matches( Node, Finder, Builder ) ){
	    return true;
	}
    }

    return false;
}
#endif

} // ast_matchers
} // clang

