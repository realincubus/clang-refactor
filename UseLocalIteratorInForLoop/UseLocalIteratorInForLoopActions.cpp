//===-- UseLocalIteratorInForLoop/UseLocalIteratorInForLoopActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the UseLocalIteratorInForLoopFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "UseLocalIteratorInForLoopActions.h"
#include "UseLocalIteratorInForLoopMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"
#include "clang/Analysis/Analyses/LiveVariables.h"

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

#if 0
namespace {
    class LivenessObserver : public LiveVariables::Observer {
    public:
        LivenessObserver (){
    
        }
        virtual ~LivenessObserver (){
    
        }

	void observeStmt( const Stmt* S, const CFGBlock* block, 
		const LiveVariables::LivenessValues& Live ) override {

	}
    
    private:
    };
}
#endif


UseLocalIteratorInForLoopFixer::UseLocalIteratorInForLoopFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void UseLocalIteratorInForLoopFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  const auto* node = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseLocalIteratorInForLoopID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      // TODO one lucky day implement liveness analysis here 
      // right now it takes to much time.
      //LiveVariables* L = 

      auto* decl_ref = dyn_cast_or_null<DeclRefExpr>(node->getLHS());

      auto* value_decl = decl_ref->getDecl();
#if 0
      auto* declarator_decl = dyn_cast_or_null<DeclaratorDecl>(value_decl);
      auto type_loc = declarator_decl->getTypeSourceInfo()->getTypeLoc();
#endif

      string type_text = value_decl->getType().getAsString();

#if 1
      SourceLocation StartLoc = decl_ref->getLocStart();
      SourceLocation EndLoc = decl_ref->getLocEnd();
      // TODO get the type of the original iterator 
      string replacement = type_text + string(" ") + getString( decl_ref, SM );
      ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
#endif
  }

}














