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

  {
      const auto* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherRenameVariableID);
      if ( node ) {
	  auto* value_decl = node->getDecl();
	  if ( !Owner.isInRange( value_decl, SM ) ) return;
	  
	  SourceLocation StartLoc = node->getLocStart();
	  SourceLocation EndLoc = node->getLocEnd();

	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, "new_name" );
      }
  }

  {
    const auto* node = Result.Nodes.getNodeAs<VarDecl>(MatcherRenameVariableDeclID);
    if ( node ) {
	if ( Owner.isInRange( node , SM ) ){
#if 1
	    string decl_init_text = "";
	    if ( node->hasInit() ){ 
		auto decl_init = node->getInit();
		decl_init_text = string(" = ") + getString( decl_init, SM );
	    }
#endif
	    auto Range = node->getSourceRange();
	    auto decl_type = string(" new_name 2") ;
	    
	    cout << "found decl" << endl;
	    //SourceLocation StartLoc = node->getLocStart();
	    //SourceLocation EndLoc = node->getLocEnd();
	    ReplaceWithString( Owner, SM, Range.getBegin(), Range.getEnd(), context, decl_type );
	}
    }
  }

}














