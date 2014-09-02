//===-- UseVector/UseVectorActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseVectorFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseVectorActions.h"
#include "UseVectorMatchers.h"

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

UseVectorFixer::UseVectorFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseVectorFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<VarDecl>(MatcherUseVectorID);
  if ( node ) {
      cout << "found array decl " << endl;
      if ( !Owner.isInRange( node, SM ) ) return;
        
      auto name = node->getNameAsString();
      auto qual_type = node->getType();
      auto type_loc = node->getTypeSourceInfo()->getTypeLoc();
      auto Range = type_loc.getSourceRange(); 

      auto* vla_type = dyn_cast_or_null<const VariableArrayType>(qual_type.getTypePtr());
      // TODO check that there is no init list
      if ( vla_type ) {      
	  cout << "is vla type " << endl;
	  auto size = vla_type->getSizeExpr();    

	  auto element_qual_type = vla_type->getElementType();
	  auto element_type_string = element_qual_type.getAsString();

	  auto replacement = string("std::vector<") + element_type_string + string("> ") + name + string("(") + getString(size, SM) + string(")");


	  ReplaceWith( Owner, SM, Range.getBegin(), Range.getEnd(), context, replacement );
      }

  }

}














