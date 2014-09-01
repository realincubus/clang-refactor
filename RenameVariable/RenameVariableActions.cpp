//===-- RenameVariable/RenameVariableActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the RenameVariableFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "RenameVariableActions.h"
#include "RenameVariableMatchers.h"

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

RenameVariableFixer::RenameVariableFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}



void RenameVariableFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();
  auto* mangle_context = context.createMangleContext();

  auto replaceDeclare = [&]( const ValueDecl* value_decl ) {

	static std::list<const ValueDecl*> isReplaced;

	auto found = std::find( begin(isReplaced), end(isReplaced), value_decl );
	if ( found != isReplaced.end() ) {
	    // was already replaced
	    return;
	}
	isReplaced.push_back( value_decl );

	// FIXME CRITICAL: this works. but i highly doubt thats the way it should work
	//                 i simply cant find out how to get the source location of the 
	//                 name in the declaration
	SourceLocation StartLoc = value_decl->getLocEnd();
	SourceLocation EndLoc = StartLoc;

	auto replacement = Owner.new_name;
	
	llvm::errs() << "inserting replacement for declaration\n";
	ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
  };
  auto replaceFunctionDeclare = [&]( const FunctionDecl* function_decl ) {

	static std::list<const ValueDecl*> isReplaced;

	auto found = std::find( begin(isReplaced), end(isReplaced), function_decl );
	if ( found != isReplaced.end() ) {
	    // was already replaced
	    return;
	}
	isReplaced.push_back( function_decl );

	auto decl_name_info = function_decl->getNameInfo();
	SourceLocation StartLoc = decl_name_info.getLoc();
	SourceLocation EndLoc = StartLoc;

	auto replacement = Owner.new_name;
	
	llvm::errs() << "inserting replacement for declaration\n";
	ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
  };

  auto getMangledName = [&](const NamedDecl* nd){ 
      std::string str;
      llvm::raw_string_ostream sstr(str);
      if ( !(isa<FunctionDecl>(nd) || isa<VarDecl>(nd)) ) return string("");
      if ( isa<CXXConstructorDecl>(nd) || isa<CXXDestructorDecl>(nd) ) return string("");
      mangle_context->mangleName(nd,sstr);
      return sstr.str();
  };


  if ( !Owner.isFoundMangledName ) {
     llvm::errs() << "mangled name was not found before\n";
     return;
  }

  // case DeclRefExpr
  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherRenameVariableID);
      if ( node ) {
	  auto* value_decl = node->getDecl();
	  auto mangled_name = getMangledName(value_decl);

	  llvm::errs() << "calculated mangled name " << mangled_name << "\n";

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( mangled_name ) == 0) ) return;

	  // TODO should not be needed anymore
	  replaceDeclare( value_decl );
	  
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();

	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, Owner.new_name );
      }
  }

  // TODO merge with variable rename. code is nearly identical
  // case CallExpr
  {
      const auto* node = Result.Nodes.getNodeAs<CallExpr>("call_expr");
      if ( node ) {
	  auto* function_decl = node->getDirectCallee();
	  if ( !function_decl ) {
	      llvm::errs() << "could not fetch the direct callee\n";
	      assert( 0 && "critical error" );
	      return;
	  }
	  auto mangled_name = getMangledName(function_decl);

	  llvm::errs() << "calculated mangled name " << mangled_name << "\n";

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( mangled_name ) == 0) ) return;

	  // TODO should not be needed anymore
	  replaceFunctionDeclare( function_decl );

	  auto callee = node->getCallee();
	  
	  SourceLocation StartLoc = callee->getLocStart();
	  SourceLocation EndLoc = callee->getLocEnd();

	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, Owner.new_name );
      }
  }

  // TODO merge with variable rename. code is nearly identical
  // case CXXMemberCallExpr
  {
      const auto* node = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call_expr");
      if ( node ) {
	  auto* method_decl = node->getMethodDecl();
	  auto mangled_name = getMangledName(method_decl);

	  llvm::errs() << "calculated mangled name " << mangled_name << "\n";

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( mangled_name ) == 0) ) return;

	  // TODO should not be needed anymore
	  replaceFunctionDeclare( method_decl );

	  auto callee = node->getCallee();
	  if ( auto member_expr = dyn_cast_or_null<MemberExpr>(callee) ){
	      auto member_loc = member_expr->getMemberLoc();
	      
	      SourceLocation StartLoc = member_loc;
	      SourceLocation EndLoc = member_loc;

	      ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, Owner.new_name );
	  }
      }
  }

   auto isSearchedDecl = [&](const ValueDecl* node){ 
      if ( node ) {
	  auto* value_decl = static_cast<const ValueDecl*>(node);
	  auto mangled_name = getMangledName( value_decl );

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( mangled_name ) == 0) ) return false;

	  return true;
      }
      return false;
  };

  // case VarDecl
  {
      const auto* node = Result.Nodes.getNodeAs<VarDecl>("decl");
      if( isSearchedDecl( node ) ) {
	  replaceDeclare( node );
      }
  }

  // case FunctionDecl
  {
      const auto* node = Result.Nodes.getNodeAs<FunctionDecl>("func_decl");
      if( isSearchedDecl( node ) ) {
	  replaceFunctionDeclare( node );
      }
  }
  // case CXXMemberDecl
  {
      const auto* node = Result.Nodes.getNodeAs<CXXMethodDecl>("method_decl");
      if( isSearchedDecl( node ) ) {
	  replaceFunctionDeclare( node );
      }
  }


}














