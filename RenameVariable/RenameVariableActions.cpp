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


  assert( Owner.isFoundMangledName && "mangled name was not found before" );
  llvm::errs() << "foundMangledName " << Owner.foundMangledName << "\n";

  // case DeclRefExpr
  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherRenameVariableID);
      if ( node ) {
	  auto* value_decl = node->getDecl();
	  std::string str;
	  llvm::raw_string_ostream sstr(str);
	  mangle_context->mangleName(value_decl,sstr);

	  llvm::errs() << "calculated mangled name " << sstr.str() << "\n";

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( sstr.str() ) == 0) ) return;

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
	  std::string str;
	  llvm::raw_string_ostream sstr(str);
	  mangle_context->mangleName(function_decl,sstr);

	  llvm::errs() << "calculated mangled name " << sstr.str() << "\n";

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( sstr.str() ) == 0) ) return;

	  // TODO should not be needed anymore
	  replaceFunctionDeclare( function_decl );

	  auto callee = node->getCallee();
	  
	  SourceLocation StartLoc = callee->getLocStart();
	  SourceLocation EndLoc = callee->getLocEnd();

	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, Owner.new_name );
      }
  }

   auto isSearchedDecl = [&](const ValueDecl* node){ 
      if ( node ) {
	  auto* value_decl = static_cast<const ValueDecl*>(node);
	  std::string str;
	  llvm::raw_string_ostream sstr(str);
	  mangle_context->mangleName(value_decl,sstr);

	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( sstr.str() ) == 0) ) return false;

	  return true;
      }
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


}














