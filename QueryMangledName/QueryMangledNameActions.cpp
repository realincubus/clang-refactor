//===-- QueryMangledName/QueryMangledNameActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the QueryMangledNameFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "QueryMangledNameActions.h"
#include "QueryMangledNameMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

#include "clang/AST/Mangle.h"

#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;


void QueryMangledNameFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  auto mangle_context = context.createMangleContext();
  SourceManager& SM = context.getSourceManager();
  std::string str;
  llvm::raw_string_ostream sstr(str);

  {
      // in case we marked a declaration
      const auto* node = Result.Nodes.getNodeAs<NamedDecl>(MatcherQueryMangledNameID);
      if ( node ) {
	  if ( Owner.isTarget( node, SM ) ) {
	      // put it to a string
	      mangle_context->mangleName(node,sstr);
	      Owner.setFoundMangledName(sstr.str());
	  }
      }
  }
  {
      // in case we marked a declaration reference
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>("decl_ref");
      if ( node ) {
	  if ( Owner.isTarget( node, SM ) ) {
	      if ( auto value_decl = node->getDecl() ) {
		// put it to a string 
	        mangle_context->mangleName(value_decl,sstr);
	        Owner.setFoundMangledName(sstr.str());
	      }
	  }
      }
  }
}














