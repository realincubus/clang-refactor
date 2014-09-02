//===-- UseAsync/UseAsyncActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseAsyncFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseAsyncActions.h"
#include "UseAsyncMatchers.h"

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

UseAsyncFixer::UseAsyncFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

bool hasPointerOrReferenceType( const ParmVarDecl* param ) {

    auto qualtype = param->getType();
    auto type = qualtype.getTypePtrOrNull();
    if ( !type ) return true; // cant tell it pointer or not
    if ( type->isPointerType() ) return true;
    if ( type->isReferenceType() ) return true;

    return false;
}

bool hasCopyArguments( const FunctionDecl* fdecl ) {
    auto num_params = fdecl->getNumParams();
    for (unsigned int i = 0; i < num_params; ++i){
        auto param = fdecl->getParamDecl(i);
	if ( hasPointerOrReferenceType( param ) ) return false;
    }
    return true;
}

// TODO intrinsic problem !!!!!! the function needs to be defined
// TODO if there is a static local variable
// TODO if there is a declRefExpr to a variable that is declared outside of the function
// TODO if there is a if there is a function call to another function that has the same problems -> recursion
bool isStatechanging( const Stmt* body ) {
    if ( !body ) return true;

    return true;
}


void UseAsyncFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<CallExpr>(MatcherUseAsyncID);
  const auto* decl = Result.Nodes.getNodeAs<FunctionDecl>(MatcherFunctionDeclID);

  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      if ( !hasCopyArguments( decl ) ) return;
      if ( !decl->isDefined() ) return;
      if ( isStatechanging( decl->getBody() ) ) return;

      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
      //ReplaceWith( Owner, SM, StartLoc, EndLoc, context, node );
  }

}














