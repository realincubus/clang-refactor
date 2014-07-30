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

// TODO right now this just works with variables which have local storage
//      the intresting part is, to transform variables in multiple files 
//      this should in theory work via the compilation database
StatementMatcher makeRenameVariableMatcher(){
    return anyOf(
	declRefExpr(to(varDecl(
	    //hasLocalStorage(),
	    hasTypeLoc(typeLoc().bind("type_loc"))
	))).bind(MatcherRenameVariableID),
	declStmt(hasSingleDecl(varDecl(
	    //hasLocalStorage(),
	    hasTypeLoc(typeLoc().bind("type_loc"))
	).bind("decl")))
    );
}









