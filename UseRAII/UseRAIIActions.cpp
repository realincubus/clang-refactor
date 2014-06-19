//===-- UseRAII/UseRAIIActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseRAIIFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseRAIIActions.h"
#include "UseRAIIMatchers.h"

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

#if 0
class RedundantLocalVariableRule
{
private:
    NamedDecl *extractReturnDeclRef(CompoundStmt *compoundStmt)
    {
        Stmt *lastStmt = (Stmt *)*(compoundStmt->body_end() - 1);
        ReturnStmt *returnStmt = dyn_cast<ReturnStmt>(lastStmt);
        if (returnStmt)
        {
            ImplicitCastExpr *implicitCastExpr =
                dyn_cast_or_null<ImplicitCastExpr>(returnStmt->getRetValue());
            if (implicitCastExpr)
            {
                DeclRefExpr *returnExpr =
                    dyn_cast_or_null<DeclRefExpr>(implicitCastExpr->getSubExpr());
                if (returnExpr)
                {
                    return returnExpr->getFoundDecl();
                }

            }
        }
        return nullptr;
    }

    NamedDecl *extractNamedDecl(CompoundStmt *compoundStmt)
    {
        Stmt *lastSecondStmt = (Stmt *)*(compoundStmt->body_end() - 2);
        DeclStmt *declStmt = dyn_cast<DeclStmt>(lastSecondStmt);
        if (declStmt && declStmt->isSingleDecl())
        {
            return dyn_cast<NamedDecl>(declStmt->getSingleDecl());
        }
        return nullptr;
    }

public:

    bool VisitCompoundStmt(CompoundStmt *compoundStmt)
    {
        if (compoundStmt->size() >= 2)
        {
            NamedDecl *returnDeclRef = extractReturnDeclRef(compoundStmt);
            NamedDecl *namedDecl = extractNamedDecl(compoundStmt);
            if (returnDeclRef && namedDecl && returnDeclRef->getName().equals(namedDecl->getName()))
            {
                //addViolation(namedDecl, this);
            }
        }

        return true;
    }
};
#endif

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

UseRAIIFixer::UseRAIIFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseRAIIFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

#if 1
  const auto* node = Result.Nodes.getNodeAs<CompoundStmt>(MatcherUseRAIIID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;
      // TODO filter declares that are not in this compound statement
      const auto* decl_ref = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherDeclRef);

      cout << "searching binary operator " << endl;
      // TODO determin stmts position inside the compound stmt 
      const auto* binary_operator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherBinOp);
      if ( !binary_operator ) return;

      cout << getString( binary_operator, SM ) << endl;
      decltype(node->body_begin()) position = node->body_end();
      for( auto it = node->body_begin(), end = node->body_end(); it != end ; it++ ){
	  if ( (*it) == binary_operator ) {
	      position = it;
	      break;
	  }
      }
      if ( position == node->body_end() ) {
	return;
      }
      cout << "found binary operator in the compound statment " << endl;
      
      // this does not work if it is the first statement in the compound statement
      if ( position == node->body_begin() ) {
	  cout << "is at the begining" << endl;	  
	  cout << getString( binary_operator, SM ) << endl;
	  return;
      }
      auto second_last = position;
      second_last--;

      auto declStmt = dyn_cast_or_null<DeclStmt>(*second_last);

      if ( !(declStmt) || !declStmt->isSingleDecl() ){
	  return;
      }
      cout << "second last statement is a single declaration statement " << endl;
      auto rhs = binary_operator->getRHS();
      if ( !rhs ) return;
      auto single_decl = declStmt->getSingleDecl();
      if ( !single_decl ) return;
      auto val_dec = dyn_cast_or_null<ValueDecl>(single_decl);
      if ( !val_dec ) return;

      auto var_dec = dyn_cast_or_null<VarDecl>(val_dec);
      if ( !var_dec ) return;
      if ( var_dec->isStaticLocal() ) return;

      if ( val_dec != decl_ref->getDecl() ) return;

      // add the initializer to the declare
      {
	  SourceLocation StartLoc = single_decl->getLocStart();
	  SourceLocation EndLoc = single_decl->getLocEnd();
	  string replacement = getString( single_decl, SM ) + string(" = ") + getString( rhs, SM );
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
      // remove the assign statement
      {
	  SourceLocation StartLoc = binary_operator->getLocStart();
	  SourceLocation EndLoc = binary_operator->getLocEnd();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, "" );
      }
  }
#endif

}














