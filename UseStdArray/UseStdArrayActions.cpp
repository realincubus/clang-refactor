//===-- UseStdArray/UseStdArrayActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseStdArrayFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseStdArrayActions.h"
#include "UseStdArrayMatchers.h"

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

UseStdArrayFixer::UseStdArrayFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseStdArrayFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<VarDecl>(MatcherUseStdArrayID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;
      
      llvm::errs() << "found a constantArrayType\n";
      auto name = node->getNameAsString();
      auto qual_type = node->getType();
      auto* type = dyn_cast<const ConstantArrayType>(qual_type.getTypePtr());
      assert( type && "type is null" );

      auto size = type->getSize();    
      auto element_qual_type = type->getElementType();
      //auto* element_type = element_qual_type.getTypePtr();
      auto element_type_string = element_qual_type.getAsString();
      
      auto type_loc = node->getTypeSourceInfo()->getTypeLoc();

      auto Range = type_loc.getSourceRange();

      auto replacement = string("std::array<") + element_type_string + string(",") + size.toString(10,true) + string("> ") + name;

      ReplaceWith( Owner, SM, Range.getBegin(), Range.getEnd(), context, replacement );
  }

}














