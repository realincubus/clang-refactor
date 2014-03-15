//===-- UseMathConstants/UseMathConstantsActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseMathConstantsFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseMathConstantsActions.h"
#include "UseMathConstantsMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

#include <cmath>

#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;

namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, std::string macro_name ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text = macro_name;

  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, macro_name ));
}
}

UseMathConstantsFixer::UseMathConstantsFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

#define EPSILON 10E-7
#define IS_SIMILAR_TO( x , y ) ( x-y < + EPSILON && x-y > -EPSILON ) 

void UseMathConstantsFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const FloatingLiteral* node = Result.Nodes.getNodeAs<FloatingLiteral>(MatcherUseMathConstantsID);
  if ( node ) {
      SourceLocation StartLoc = node->getLocStart();
      SourceLocation EndLoc = node->getLocEnd();
    
      auto value = node->getValueAsApproximateDouble();

      pair<double,string> val_and_name[] ={
	  {M_E,"M_E"},          
	  {M_LOG2E,"M_LOG2E"},     
	  {M_LOG10E,"M_LOG10E"}, 
	  {M_LN2,"M_LN2"},  
	  {M_LN10,"M_LN10"},  
	  {M_PI,"M_PI"},  
	  {M_PI_2,"M_PI_2"},  
	  {M_PI_4,"M_PI_4"},  
	  {M_1_PI,"M_1_PI"},  
	  {M_2_PI,"M_2_PI"},  
	  {M_2_SQRTPI,"M_2_SQRTPI" },  
	  {M_SQRT2,"M_SQRT2"},  
	  {M_SQRT1_2,"M_SQRT1_2"}
      }; 

      for( auto p : val_and_name ){
	  if ( IS_SIMILAR_TO( value, p.first ) ){
	      if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){
		    ReplaceWith( Owner, SM, StartLoc, EndLoc, context, p.second ); 
		    return;
	      }
	  }
      }

  }

}














