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
    SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* argument ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text = getString( argument, SM );
  string replacement = source_text; 

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
    
      auto* arg0 = node->getArg( 0 );

      arg0->dumpColor();

      auto* materialize_temporary_expr = dyn_cast_or_null<MaterializeTemporaryExpr>(arg0);
      
      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, node );
  }

}














