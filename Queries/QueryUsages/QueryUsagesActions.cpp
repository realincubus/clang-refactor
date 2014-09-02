//===-- QueryUsages/QueryUsagesActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the QueryUsagesFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "QueryUsagesActions.h"
#include "QueryUsagesMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;


void QueryUsagesFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();
  static std::map< const NamedDecl*, std::pair<int,int> > usage_map;

  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>("decl_ref");
      if ( node ) {
	  const auto value_decl = node->getDecl();
	  const auto named_decl = static_cast<const NamedDecl*>(value_decl);
	  auto& decl_use = usage_map[named_decl];
	  decl_use.second++;
      }
  }
  {
      const auto* node = Result.Nodes.getNodeAs<NamedDecl>("named_decl");
      if ( node ) {
	  auto& decl_use = usage_map[node];
	  decl_use.first++;
      }
  }

}














