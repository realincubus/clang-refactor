//===-- QueryMangledName/QueryMangledNameMatchers.cpp - Matchers for null casts ----------===//
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

#include "QueryMangledNameMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherQueryMangledNameID = "matcherQueryMangledNameID";


DeclarationMatcher makeQueryMangledNameMatcher(){
    return namedDecl().bind(MatcherQueryMangledNameID);
}

StatementMatcher makeQueryMangledNameMatcherRef	() {
    return anyOf(
	    declRefExpr().bind("decl_ref"),
	    callExpr(unless(memberCallExpr())).bind("call_expr"),
	    memberCallExpr().bind("member_call_expr")
    );
}











