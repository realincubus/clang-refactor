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

  auto replaceDeclare = [&]( const ValueDecl* value_decl ) {

	static std::list<const ValueDecl*> isReplaced;

	auto found = std::find( begin(isReplaced), end(isReplaced), value_decl );
	if ( found != isReplaced.end() ) {
	    // was already replaced
	    return;
	}
	isReplaced.push_back( value_decl );

	std::string initializer_string = "";
        // handle initializers
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

  assert( Owner.isFoundMangledName && "mangled name was not found before" );
  llvm::errs() << "foundMangledName " << Owner.foundMangledName << "\n";

  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherRenameVariableID);
      if ( node ) {
	  llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts( new DiagnosticOptions() );
	  DiagnosticsEngine Diagnostics( llvm::IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), DiagOpts.get() );
	  auto* value_decl = node->getDecl();
	  auto* mangle_context = context.createMangleContext();
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
  {
      const auto* node = Result.Nodes.getNodeAs<VarDecl>("decl");
      if ( node ) {
	  llvm::errs() << "found a matching decl node\n";
	  auto* value_decl = static_cast<const ValueDecl*>(node);
	  auto* mangle_context = context.createMangleContext();
	  std::string str;
	  llvm::raw_string_ostream sstr(str);
	  mangle_context->mangleName(value_decl,sstr);

	  llvm::errs() << "calculated mangled name " << sstr.str() << "\n";
	  // if the queried mangled name and the actual mangled name are not the same bail out
	  if ( !(Owner.foundMangledName.compare( sstr.str() ) == 0) ) return;
#if 0
	  if ( !Owner.isTarget( value_decl, SM ) ) return;
#endif

	  replaceDeclare( value_decl );
      }
  }


}














