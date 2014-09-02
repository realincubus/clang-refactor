//===-- QueryUsages/QueryUsagesMatchers.cpp - Matchers for null casts ----------===//
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
///
//===----------------------------------------------------------------------===//

#include "QueryUsagesMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherQueryUsagesID = "matcherQueryUsagesID";


StatementMatcher makeQueryUsagesMatcher(){
    return declRefExpr().bind("decl_ref");
}

DeclarationMatcher makeQueryDeclarationMatcher(){
    return namedDecl().bind("named_decl");
}









