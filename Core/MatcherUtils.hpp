

#pragma once


#include "clang/ASTMatchers/ASTMatchers.h"


using namespace clang::ast_matchers;
using namespace clang;

namespace clang {
namespace ast_matchers{

AST_MATCHER_P2(BinaryOperator, isCommutative , internal::Matcher<Expr>, Internal1, internal::Matcher<Expr>, Internal2 ) {

    if ( Internal1.matches( *Node.getLHS(), Finder, Builder) && Internal2.matches( *Node.getRHS(), Finder, Builder ) ) {
	llvm::errs() << "submatcher does not match L R\n" ;
	return true;
    }
    if ( Internal1.matches( *Node.getRHS(), Finder, Builder) && Internal2.matches( *Node.getLHS(), Finder, Builder ) ){
	llvm::errs() << "submatcher does not match R L\n" ;
	return true;
    }

    return false;
}

AST_MATCHER(Stmt, print) {
    llvm::errs() << "printSubtree\n";
    llvm::errs() << "StatementClassName " << Node.getStmtClassName() << "\n";
    Node.dumpColor();
    return true;
}

} // ast_matchers
} // clang

