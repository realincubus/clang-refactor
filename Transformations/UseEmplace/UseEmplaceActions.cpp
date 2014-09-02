//===-- UseEmplace/UseEmplaceActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseEmplaceFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseEmplaceActions.h"
#include "UseEmplaceMatchers.h"

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

UseEmplaceFixer::UseEmplaceFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseEmplaceFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<CXXMemberCallExpr>(MatcherUseEmplaceID);
  if ( node ) {
      cout << "found node " << endl;
      if ( !Owner.isInRange( node, SM ) ) return;
    
      // this changes the function call
      {
	  auto* callee = node->getCallee();
	  callee->dumpColor();
	  auto* member_expr = dyn_cast_or_null<MemberExpr>(callee);
	  auto member_loc = member_expr->getMemberLoc();
	  
	  SourceLocation StartLoc = member_loc;
	  SourceLocation EndLoc = member_loc;
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "emplace_back" );
      }

      // this changes the argument
      auto* arg0 = node->getArg( 0 );
      auto* materialize_temporary_expr = dyn_cast_or_null<MaterializeTemporaryExpr>(arg0);
      auto* temp_expr = materialize_temporary_expr->GetTemporaryExpr();
      auto* bind_expr = dyn_cast_or_null<CXXBindTemporaryExpr>(temp_expr);
      auto* constructor_call = dyn_cast_or_null<CXXConstructExpr>(bind_expr->getSubExpr()); 
      string arguments_text = "";
      for( unsigned int i = 0, end = constructor_call->getNumArgs(); i < end ; i++ ){
	  if ( i + 1 == end ){
	    arguments_text += getString( constructor_call->getArg(i), SM ); 
	  }else{
	    arguments_text += getString( constructor_call->getArg(i), SM ) + string(","); 
	  }
      }
      
      SourceLocation StartLoc = materialize_temporary_expr->getLocStart();
      SourceLocation EndLoc = materialize_temporary_expr->getLocEnd();
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, arguments_text );
  }

}














