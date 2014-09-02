//===-- CollapseIfStatement/CollapseIfStatementActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the CollapseIfStatementFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "CollapseIfStatementActions.h"
#include "CollapseIfStatementMatchers.h"

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

using namespace std;


namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* first_condition, const Expr* second_condition  ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string first_source_text = getString( first_condition, SM );
  string second_source_text = getString( second_condition, SM );

  string replacement = first_source_text + string(" && ") + second_source_text; 

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      cout << "replacing " << endl;
    Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }else{
      cout << "is not replaceable " << endl;
  }
}
}

class CollapsibleIfStatementsRule //: public AbstractASTVisitorRule<CollapsibleIfStatementsRule>
{
private:
    bool compoundStmtContainsOnlyOneIfStmt(const CompoundStmt *compoundStmt)
    {
        return compoundStmt->size() == 1 && isa<const IfStmt>(*(compoundStmt->body_begin()));
    }

    const IfStmt *getInnerIfStmt(const IfStmt *ifStmt)
    {
        const Stmt *thenStmt = ifStmt->getThen();
        if (thenStmt && isa<const IfStmt>(thenStmt))
        {
            return dyn_cast<const IfStmt>(thenStmt);
        }
        if (thenStmt && isa<const CompoundStmt>(thenStmt))
        {
            const CompoundStmt *compoundStmt = dyn_cast<CompoundStmt>(thenStmt);
            if (compoundStmtContainsOnlyOneIfStmt(compoundStmt))
            {
                return dyn_cast<const IfStmt>(*(compoundStmt->body_begin()));
            }
        }
        return nullptr;
    }

    bool checkElseBranch(const IfStmt *outerIf, const IfStmt *innerIf)
    {
        return outerIf->getElse() || innerIf->getElse();
    }

public:

    pair<bool,const IfStmt*> VisitIfStmt(const IfStmt *ifStmt)
    {
        const IfStmt *innerIf = getInnerIfStmt(ifStmt);
        if (innerIf && !checkElseBranch(ifStmt, innerIf))
        {
	    cout << "if statement can be collapsed " << endl;
	    return make_pair(true, innerIf);
        }

        return make_pair<bool,const IfStmt*>(false,nullptr);
    }
};

CollapseIfStatementFixer::CollapseIfStatementFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void CollapseIfStatementFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  CollapsibleIfStatementsRule cifr;
  const IfStmt* ifstmt = Result.Nodes.getNodeAs<IfStmt>(MatcherCollapseIfStatementID);
  if ( ifstmt ) {
      auto isReplace = cifr.VisitIfStmt( ifstmt );
      if ( isReplace.first ){
	  auto condition_expr = ifstmt->getCond();
	  auto inner_condition_expr = isReplace.second->getCond();
	  // replace the original Condition with both connected by &&
	  SourceLocation StartLoc = condition_expr->getLocStart();
	  SourceLocation EndLoc = condition_expr->getLocEnd();
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, condition_expr, inner_condition_expr );

	  // replace the outer then with the inner then
	  auto inner_body = isReplace.second->getThen();
	  ReplaceRangeWithRange( Owner, SM, context, ifstmt->getThen()->getSourceRange(), inner_body->getSourceRange() );

	  // TODO replace the outer else with the inner else
	  
      }
  }

}














