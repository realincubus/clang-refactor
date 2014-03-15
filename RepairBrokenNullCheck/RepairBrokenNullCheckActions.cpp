//===-- RepairBrokenNullCheck/RepairBrokenNullCheckActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the RepairBrokenNullCheckFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "RepairBrokenNullCheckActions.h"
#include "RepairBrokenNullCheckMatchers.h"

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

#include "AbstractNullCheckRule.h"

using namespace std;
using namespace clang;


namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* lhs, const Expr* rhs, std::string replacement_op ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string lhs_string = getString( lhs, SM );
  string rhs_string = getString( rhs, SM );
  string replacement_text = lhs_string + string(" ") +  replacement_op + string(" ") + rhs_string; 

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement_text ));
}
}

RepairBrokenNullCheckFixer::RepairBrokenNullCheckFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

class BrokenNullCheckBaseRule : public AbstractNullCheckRule<BrokenNullCheckBaseRule>
{
private:
    bool isEqNullCheckBroken(BinaryOperator *binaryOperator)
    {
        return binaryOperator->getOpcode() == BO_LOr && isNeNullCheck(binaryOperator->getLHS());
    }

    bool isNeNullCheckBroken(BinaryOperator *binaryOperator)
    {
        return binaryOperator->getOpcode() == BO_LAnd && isEqNullCheck(binaryOperator->getLHS());
    }

    bool isNullCheckBroken(BinaryOperator *binaryOperator)
    {
        return isEqNullCheckBroken(binaryOperator) || isNeNullCheckBroken(binaryOperator);
    }

    bool isSameVariableBroken(BinaryOperator *binaryOperator)
    {
        string variableOfInterest = extractIdentifierFromExpr(binaryOperator->getLHS());
        return variableOfInterest == "" ? false :
            hasVariableInExpr(variableOfInterest, binaryOperator->getRHS());
    }

protected:
    virtual bool hasVariableInExpr(string variableOfInterest, Expr *expr) = 0;

public:
    pair<bool,string> VisitBinaryOperator(BinaryOperator *binaryOperator, SourceManager &SM)
    {
	cout << "in VisitBinaryOperator " << endl;
        if (isNullCheckBroken(binaryOperator) && isSameVariableBroken(binaryOperator))
        {
	    SourceLocation slb = binaryOperator->getOperatorLoc();
	    SourceLocation sle = slb.getLocWithOffset(1);
	    std::string op_text = Lexer::getSourceText( CharSourceRange::getTokenRange(SourceRange(slb, sle)), SM, LangOptions());

	    if ( binaryOperator->getOpcode() == BO_LOr ) {
		cout << "replace " << op_text << " with && " << endl;	
		return make_pair( true, "&&" );
	    }
	    if ( binaryOperator->getOpcode() == BO_LAnd ) {
		cout << "replace " << op_text << " with || " << endl;	
		return make_pair( true, "||" );
	    }
        }

        return make_pair(false,"");
    }
};

class BrokenNullCheckRule : public BrokenNullCheckBaseRule
{
protected:
    virtual bool hasVariableInExpr(string variableOfInterest, Expr* expr)
        override
    {
        VariableOfInterestInMemberExpr seekingVariable;
        return seekingVariable.hasVariableInExpr(variableOfInterest, expr, this);
    }

public:
#if 0
    virtual const string name() const override
    {
        return "broken null check";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual unsigned int supportedLanguages() const override
    {
        return LANG_C | LANG_CXX;
    }
#endif
};




void RepairBrokenNullCheckFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const BinaryOperator* binaryOperator = Result.Nodes.getNodeAs<BinaryOperator>(MatcherRepairBrokenNullCheckID);
  if ( binaryOperator ) {
      cout << "found binaryOperator " << endl;
      BrokenNullCheckRule bncr;
      auto isReplace = bncr.VisitBinaryOperator( (BinaryOperator*)binaryOperator, SM);

      if ( isReplace.first ) {
	  SourceLocation StartLoc = binaryOperator->getLocStart();
	  SourceLocation EndLoc = binaryOperator->getLocEnd();
	
	  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, binaryOperator->getLHS(), binaryOperator->getRHS(), isReplace.second ); 
      }
  }

}














