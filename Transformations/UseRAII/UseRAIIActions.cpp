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

UseRAIIFixer::UseRAIIFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseRAIIFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  llvm::errs() << "starting actions\n";
#if 1
  const auto* node = Result.Nodes.getNodeAs<Stmt>("stmt");
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      return;

      const auto* binary_operator = Result.Nodes.getNodeAs<BinaryOperator>("binary_operator");
      const auto* var_decl = Result.Nodes.getNodeAs<VarDecl>("decl");

      assert( binary_operator && var_decl && "could not fetch" ) ;

      llvm::errs() << "fetch rhs\n";
      auto rhs = binary_operator->getRHS();
      if ( !rhs ) return;

      // add the initializer to the declare
      {
	  llvm::errs() << "add init\n";
	  SourceLocation StartLoc = var_decl->getLocStart();
	  SourceLocation EndLoc = var_decl->getLocEnd();
	  string replacement = getString( var_decl, SM ) + string(" = ") + getString( rhs, SM );
	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
      // remove the assign statement
      {
	  llvm::errs() << "remove assign statement\n";
	  SourceLocation StartLoc = binary_operator->getLocStart();
	  SourceLocation EndLoc = binary_operator->getLocEnd();
	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, "" );
      }
  }
  llvm::errs() << "done\n";
#endif

}














