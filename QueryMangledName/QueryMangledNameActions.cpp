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

  auto getMangledName = [&](const NamedDecl* nd){ 
      std::string str;
      llvm::raw_string_ostream sstr(str);
      if ( !isa<FunctionDecl>(nd) && !isa<VarDecl>(nd) ) {
	  llvm::errs() << "return because not Function or VarDecl\n";
	  return string("");
      }
      if ( isa<CXXConstructorDecl>(nd) || isa<CXXDestructorDecl>(nd) ) {
	  llvm::errs() << "return because not Ctor or Dtor\n";
	  return string("");
      }
      mangle_context->mangleName(nd,sstr);
      return sstr.str();
  };

  {
      // in case we marked a declaration
      const auto* node = Result.Nodes.getNodeAs<NamedDecl>(MatcherQueryMangledNameID);
      if ( node ) {
	  if ( Owner.isTarget( node, SM ) ) {
	      // filter structors
	      auto mangled_name = getMangledName( node );
	      Owner.setFoundMangledName(mangled_name);
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
	        auto mangled_name = getMangledName( value_decl );
	        Owner.setFoundMangledName(mangled_name);
	      }
	  }
      }
  }
  {
      // in case we marked a call expr
      const auto* node = Result.Nodes.getNodeAs<CallExpr>("call_expr");
      if ( node ) {
	  if ( Owner.isTarget( node, SM ) ) {
	      if ( auto function_decl = node->getDirectCallee() ) {
		// put it to a string 
		auto mangled_name = getMangledName( function_decl );
	        Owner.setFoundMangledName(mangled_name);
	      }
	  }
      }
  }
  {
      // in case we marked a member call expr
      const auto* node = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call_expr");
      if ( node ) {
	  llvm::errs() << "found a member call expr\n";
	  // test its callee not the whole expr
	  auto callee = node->getCallee();
	  callee->dumpColor();

	  auto member_expr = dyn_cast_or_null<MemberExpr>(callee);
	  auto member_loc = member_expr->getMemberLoc();

	  member_expr->getLocStart().print( llvm::errs(), SM );
	  member_loc.print( llvm::errs(), SM );
	  
	  if ( Owner.isTarget( member_loc, member_loc, SM ) ) {
	      llvm::errs() << "it is the target\n";
	      if ( auto method_decl = node->getMethodDecl() ) {
		llvm::errs() << "got its method decl\n";
		// put it to a string 
		auto mangled_name = getMangledName( method_decl );
	        Owner.setFoundMangledName(mangled_name);
	      }
	  }
      }
  }


}














