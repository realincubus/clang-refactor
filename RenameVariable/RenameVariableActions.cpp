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

  auto replaceDeclare = [&]( const ValueDecl* value_decl ) {

	static std::list<const ValueDecl*> isReplaced;

	auto found = std::find( begin(isReplaced), end(isReplaced), value_decl );
	if ( found != isReplaced.end() ) {
	    // was already replaced
	    return;
	}
	isReplaced.push_back( value_decl );

	std::string initializer_string = "";
        // TODO handle initializers
	if ( const auto* var_decl = dyn_cast_or_null<const VarDecl>(value_decl) ){
	    if ( var_decl->hasInit() ){ 
		auto decl_init = var_decl->getInit();
		initializer_string = string(" = ") + getString( decl_init, SM );
	    }
	}
	SourceLocation StartLoc = value_decl->getLocStart();
	SourceLocation EndLoc = value_decl->getLocEnd();
	const auto* type_loc = Result.Nodes.getNodeAs<TypeLoc>("type_loc");
	auto type_string = getString( type_loc, SM );

	auto replacement = type_string + string(" ") + Owner.new_name + initializer_string;
	
	llvm::errs() << "inserting replacement for declaration\n";
	ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
  };


  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherRenameVariableID);
      if ( node ) {
	  auto* value_decl = node->getDecl();
	  llvm::errs() << "found ref in\n";
	  node->getLocStart().dump(SM);
	  if ( !Owner.isTarget( value_decl, SM ) ) {
	      llvm::errs() << "target does not match\n";
	      return;
	  }
	  replaceDeclare( value_decl );
	  
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();

	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, Owner.new_name );
      }
  }
  {
      const auto* node = Result.Nodes.getNodeAs<VarDecl>("decl");
      if ( node ) {
	  llvm::errs() << "found a matching decl node\n";
	  auto* value_decl = static_cast<const ValueDecl*>(node);
	  if ( !Owner.isTarget( value_decl, SM ) ) return;
	  replaceDeclare( value_decl );
      }
  }


}














