//===-- RenameVariable/RenameVariableMatchers.cpp - Matchers for null casts ----------===//
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

#include "RenameVariableMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherRenameVariableID = "matcherRenameVariableID";
const char *MatcherRenameVariableDeclID = "matcherRenameVariableDeclID";

// TODO put the varDecl in makeFunctionDeclMatcher
StatementMatcher makeRenameVariableMatcher(){
    return anyOf(
	declRefExpr(to(varDecl(
	    hasTypeLoc(typeLoc().bind("type_loc"))
	))).bind(MatcherRenameVariableID),
	callExpr(
	).bind("call_expr"),
	declStmt(hasSingleDecl(varDecl(
	    hasTypeLoc(typeLoc().bind("type_loc"))
	).bind("decl")))
    );
}

DeclarationMatcher makeFunctionDeclMatcher(){
    return functionDecl().bind("func_decl");
}









