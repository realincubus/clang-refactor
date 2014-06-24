

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

AST_MATCHER_P(Stmt, print, std::string, pos ) {
    llvm::errs() << "StatementClassName " << Node.getStmtClassName() << " invoked at " << pos << "\n";
    Node.dumpColor();
    return true;
}

AST_MATCHER_P2(CompoundStmt, predecessor, const Stmt*, stmt, internal::Matcher<Stmt>, Internal ) {
    
    for( auto I = Node.body_begin(), E = Node.body_end(); I != E ; I++ ){
	if ( *I == stmt ){
	    decltype(I) previous = I - 1;
	    // if the statement is the first in the list
	    if ( previous == I ) return false;
	    if ( Internal.matches( *(*previous), Finder, Builder ) ) {
		return true;
	    }
	    return false;
	}
    }
    return false;
}

AST_MATCHER_P2(CompoundStmt, successor, const Stmt*, stmt, internal::Matcher<Stmt>, Internal ) {
    
    for( auto I = Node.body_begin(), E = Node.body_end(); I != E ; I++ ){
	if ( *I == stmt ){
	    decltype(I) next = I + 1;
	    // if the statement is the last in the list
	    if ( next == Node.body_end() ) return false;
	    if ( Internal.matches( *(*next), Finder, Builder ) ) {
		return true;
	    }
	    return false;
	}
    }
    return false;
}

AST_MATCHER_P(Stmt, predecessorStmt, internal::Matcher<Stmt>, Internal ) {

    StatementMatcher ParentMatcher = 
	    compoundStmt(
		predecessor( 
		    &Node,
		    Internal
		)
	    );
    if ( Finder->matchesAncestorOf( Node, ParentMatcher, Builder, ASTMatchFinder::AMM_ParentOnly) ) {
	return true;	
    }
    return false;
}

AST_MATCHER_P(Stmt, succecessorStmt, internal::Matcher<Stmt>, Internal ) {

    StatementMatcher ParentMatcher = 
	    compoundStmt(
		successor( 
		    &Node,
		    Internal
		)
	    );
    if ( Finder->matchesAncestorOf( Node, ParentMatcher, Builder, ASTMatchFinder::AMM_ParentOnly) ) {
	return true;	
    }
    return false;
}

AST_MATCHER_P2(CompoundStmt, getSubStatement, int, id, internal::Matcher<Stmt>, Internal ) {

    int ctr = 0;
    for( auto I = Node.body_begin(), E = Node.body_end(); I != E ; I++ ){
	if ( ctr == id ) {
	    if ( Internal.matches( *(*I), Finder, Builder ) ) return true;
	    return false;
	}
	ctr++;
    }

    return false;
}


} // ast_matchers
} // clang

