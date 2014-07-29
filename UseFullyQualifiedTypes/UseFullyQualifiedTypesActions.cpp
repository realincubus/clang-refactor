//===-- UseFullyQualifiedTypes/UseFullyQualifiedTypesActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseFullyQualifiedTypesFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseFullyQualifiedTypesActions.h"
#include "UseFullyQualifiedTypesMatchers.h"

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

UseFullyQualifiedTypesFixer::UseFullyQualifiedTypesFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

// idea from TypePrinter.cpp 897
std::string getScopeAsString(const DeclContext *DC) {
    if (DC->isTranslationUnit()) return "";
    if (DC->isFunctionOrMethod()) return "";
    getScopeAsString(DC->getParent());

    if (const auto* NS = dyn_cast_or_null<const NamespaceDecl>(DC)) {
	if (true && (NS->isAnonymousNamespace() || NS->isInline())){
	    return "" ;
	}
	if (NS->getIdentifier()){
	    return NS->getName().str() + std::string("::");
	}else{
	    return std::string("(anonymous namespace)::");
	}
    } 
    return "";
}

void UseFullyQualifiedTypesFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<VarDecl>(MatcherUseFullyQualifiedTypesID);
  const auto* type = Result.Nodes.getNodeAs<Type>("type");
  const auto* type_loc = Result.Nodes.getNodeAs<TypeLoc>("type_loc");
  if ( type_loc ) {
      if ( !Owner.isInRange( type_loc, SM ) ) return;
      llvm::errs() << "is in range\n" ;
#if 0
      auto sloc = SM.getSpellingLoc( type_loc->getLocStart() ) ;
      if (!Owner.isFileModifiable(SM, sloc))
	  return;
#endif

      llvm::errs() << "got loc start\n" ;
      type_loc->getLocStart().dump(SM) ;
      //sloc.dump(SM);
      if (!Owner.isFileModifiable(SM, type_loc->getLocStart())) return;
      
      const auto* record = type->getAsCXXRecordDecl();
      if ( record ) {
	  llvm::errs() << "is a record and can be replaced\n";
	  auto scope_text = getScopeAsString( record->getDeclContext() );
	  auto type_loc_text = getString( type_loc, SM );
	  auto replacement = scope_text + type_loc_text;

	  SourceLocation StartLoc = type_loc->getLocStart();
	  SourceLocation EndLoc = type_loc->getEndLoc();
	  llvm::errs() << "inserting the replacement: " << replacement << "\n";
	  ReplaceWithString( Owner, SM, StartLoc, EndLoc, context, replacement );
      }
  }

}














