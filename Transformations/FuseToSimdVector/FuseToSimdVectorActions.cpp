//===-- FuseToSimdVector/FuseToSimdVectorActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the FuseToSimdVectorFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "FuseToSimdVectorActions.h"
#include "FuseToSimdVectorMatchers.h"

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

namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, std::string replacement ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }
}
}

FuseToSimdVectorFixer::FuseToSimdVectorFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void FuseToSimdVectorFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<DeclStmt>(MatcherFuseToSimdVectorID);
  const auto* decl_ref  = Result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr");
  const auto* ref_to = Result.Nodes.getNodeAs<VarDecl>("refto");
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      if ( decl_ref ) {
	  // TODO this means we are working with a reference to one of the components of a vector 
	  // TODO get position in the declare stmt
	  int ctr = 0;
	  bool found = false;
	  for( auto I = node->decl_begin(), E = node->decl_end(); I != E ; I++ ){
	      VarDecl* var_decl = dyn_cast_or_null<VarDecl>( *I );
	      if ( var_decl == ref_to ) {
		  found = true;
		  break;
	      }
	      ctr++;
	  }

	  if ( found ) {
	      SourceLocation StartLoc = decl_ref->getLocStart();
	      SourceLocation EndLoc = decl_ref->getLocEnd();
	      std::string replacement = Owner.new_name + string("[") + to_string(ctr) + string("]");
	      ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
	  }
	  return;
      }


      // get the initializers
      vector< Expr* > initializers;
      bool all_or_nothing = false;
      for( auto I = node->decl_begin(), E = node->decl_end(); I != E ; I++ ){
	  auto* decl = *I;
	  if ( auto* varDecl = dyn_cast_or_null<VarDecl>(decl) ) {
		// if the first has an initializer -> all others also have to have one
		if ( varDecl->hasInit() ) {
		    if ( I == node->decl_begin() ) {
			all_or_nothing = true;
		    }else{
		    }
		    auto* init_expr = varDecl->getInit();
		    initializers.push_back( init_expr );
		}else{
		  if ( all_or_nothing ) {
		      // if one of the vardeclares has not initializer -> bail out
		      return;
		  }
		}
	  }else{
	      return;
	  }
      }
    
      std::string init_text = "";
      for( auto* element : initializers ){
          auto init_string = getString( element, SM );
	  init_text += init_string + " ";
      }

      // TODO check whether this is ok
      auto first_decl_iterator = node->decl_begin();
      auto* first_decl = *first_decl_iterator;
      auto* first_vardecl = dyn_cast_or_null<VarDecl>( first_decl );
      auto type_string = getTypeString( first_vardecl, SM );

      // determin the size of the vector 
      double next_power_of_2 = node->getDeclGroup().getDeclGroup().size();
      next_power_of_2 = pow(2,ceil(log2(next_power_of_2)));
      int casted_power_of_2 = next_power_of_2;
      auto multiplyer = to_string( casted_power_of_2 );

      auto attribute_string = string(" __attribute__((vector_size(sizeof(") +
	  type_string + 
	  string(")*") + 
	  multiplyer  + 
	  string("))) " );
      

      // TODO retrieve from parameter
      std::string replacement = type_string + attribute_string + Owner.new_name;

      if ( init_text.compare("") != 0 ) {
	  replacement += string(" = ") + init_text;
      }

      replacement += ";";

      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
  }

}














