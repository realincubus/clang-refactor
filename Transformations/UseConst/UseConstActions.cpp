//===-- UseConst/UseConstActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseConstFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseConstActions.h"
#include "UseConstMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/Analyses/PseudoConstantAnalysis.h"

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

UseConstFixer::UseConstFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseConstFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<FunctionDecl>(MatcherUseConstID);
  const auto* var_decl = Result.Nodes.getNodeAs<VarDecl>(MatcherVarID);
  // TODO perhaps later also consider parameters
  if ( dyn_cast_or_null<ParmVarDecl>(var_decl) ) return;

  if ( node ) {
      if ( node->hasBody() ){
	  PseudoConstantAnalysis pca(node->getBody());

	  if ( !Owner.isInRange( node, SM ) ) return;
	  if ( pca.isPseudoConstant( var_decl ) ) {
	      string type_text = var_decl->getType().getAsString();
	      string replacement = string("const ") + type_text;

	      auto loc = var_decl->getTypeSourceInfo()->getTypeLoc();
#if 1
	      SourceLocation StartLoc = loc.getLocStart();
	      SourceLocation EndLoc = loc.getLocEnd();
	      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
	  }
#endif
      }
  }

}














