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
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"
#include "clang/StaticAnalyzer/Core/PathDiagnosticConsumers.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/AST/ParentMap.h"
#include "llvm/ADT/BitVector.h"


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



UseLocalIteratorInForLoopFixer::UseLocalIteratorInForLoopFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}

using namespace ento;
namespace {
/// A simple visitor to record what VarDecls occur in EH-handling code.
class EHCodeVisitor : public RecursiveASTVisitor<EHCodeVisitor> {
public:
  bool inEH;
  llvm::DenseSet<const VarDecl *> &S;
  
  bool TraverseObjCAtFinallyStmt(ObjCAtFinallyStmt *S) {
    SaveAndRestore<bool> inFinally(inEH, true);
    return ::RecursiveASTVisitor<EHCodeVisitor>::TraverseObjCAtFinallyStmt(S);
  }
  
  bool TraverseObjCAtCatchStmt(ObjCAtCatchStmt *S) {
    SaveAndRestore<bool> inCatch(inEH, true);
    return ::RecursiveASTVisitor<EHCodeVisitor>::TraverseObjCAtCatchStmt(S);
  }
  
  bool TraverseCXXCatchStmt(CXXCatchStmt *S) {
    SaveAndRestore<bool> inCatch(inEH, true);
    return TraverseStmt(S->getHandlerBlock());
  }
  
  bool VisitDeclRefExpr(DeclRefExpr *DR) {
    if (inEH)
      if (const VarDecl *D = dyn_cast<VarDecl>(DR->getDecl()))
        S.insert(D);
    return true;
  }
  
  EHCodeVisitor(llvm::DenseSet<const VarDecl *> &S) :
  inEH(false), S(S) {}
};

// FIXME: Eventually migrate into its own file, and have it managed by
// AnalysisManager.
class ReachableCode {
  const CFG &cfg;
  llvm::BitVector reachable;
public:
  ReachableCode(const CFG &cfg)
    : cfg(cfg), reachable(cfg.getNumBlockIDs(), false) {}
  
  void computeReachableBlocks();
  
  bool isReachable(const CFGBlock *block) const {
    return reachable[block->getBlockID()];
  }
};
}

void ReachableCode::computeReachableBlocks() {
  if (!cfg.getNumBlockIDs())
    return;
  
  SmallVector<const CFGBlock*, 10> worklist;
  worklist.push_back(&cfg.getEntry());

  while (!worklist.empty()) {
    const CFGBlock *block = worklist.pop_back_val();
    llvm::BitVector::reference isReachable = reachable[block->getBlockID()];
    if (isReachable)
      continue;
    isReachable = true;
    for (CFGBlock::const_succ_iterator i = block->succ_begin(),
                                       e = block->succ_end(); i != e; ++i)
      if (const CFGBlock *succ = *i)
        worklist.push_back(succ);
  }
}

static const Expr *
LookThroughTransitiveAssignmentsAndCommaOperators(const Expr *Ex) {
  while (Ex) {
    const BinaryOperator *BO =
      dyn_cast<BinaryOperator>(Ex->IgnoreParenCasts());
    if (!BO)
      break;
    if (BO->getOpcode() == BO_Assign) {
      Ex = BO->getRHS();
      continue;
    }
    if (BO->getOpcode() == BO_Comma) {
      Ex = BO->getRHS();
      continue;
    }
    break;
  }
  return Ex;
}

namespace {
class DeadStoreObs : public LiveVariables::Observer {
  const CFG &cfg;
  ASTContext &Ctx;
  AnalysisDeclContext* AC;
  ParentMap& Parents;
  llvm::SmallPtrSet<const VarDecl*, 20> Escaped;
  std::unique_ptr<ReachableCode> reachableCode;
  const CFGBlock *currentBlock;
  std::unique_ptr<llvm::DenseSet<const VarDecl *>> InEH;
  const Stmt* Position;
  const VarDecl* VarD;
  bool IsAlive = false;

  enum DeadStoreKind { Standard, Enclosing, DeadIncrement, DeadInit };

public:

  DeadStoreObs(const CFG &cfg, ASTContext &ctx, AnalysisDeclContext *ac,
               ParentMap &parents,
               llvm::SmallPtrSet<const VarDecl *, 20> &escaped,
	       const Stmt* position,
	       const VarDecl* var_decl
	       )
      : cfg(cfg), Ctx(ctx), AC(ac), Parents(parents),
        Escaped(escaped), currentBlock(0),Position(position),VarD(var_decl) {}

  virtual ~DeadStoreObs() {}

  bool isLive(const LiveVariables::LivenessValues &Live, const VarDecl *D) {
    if (Live.isLive(D))
      return true;
    // Lazily construct the set that records which VarDecls are in
    // EH code.
#if 1
    if (!InEH.get()) {
      InEH.reset(new llvm::DenseSet<const VarDecl *>());
      EHCodeVisitor V(*InEH.get());
      V.TraverseStmt(AC->getBody());
    }
    // Treat all VarDecls that occur in EH code as being "always live"
    // when considering to suppress dead stores.  Frequently stores
    // are followed by reads in EH code, but we don't have the ability
    // to analyze that yet.
    return InEH->count(D);
#endif
  }

  bool isAlive( ) {return IsAlive;}
  
  void CheckVarDecl(const VarDecl *VD, const Expr *Ex, const Expr *Val,
                    DeadStoreKind dsk,
                    const LiveVariables::LivenessValues &Live) {

    if (!VD->hasLocalStorage())
      return;
    // Reference types confuse the dead stores checker.  Skip them
    // for now.
    if (VD->getType()->getAs<ReferenceType>())
      return;

    if (!isLive(Live, VD) &&
        !(VD->hasAttr<UnusedAttr>() || VD->hasAttr<BlocksAttr>() ||
          VD->hasAttr<ObjCPreciseLifetimeAttr>())) {
      
      auto start_loc = Ex->getLocStart();
      auto text_loc = start_loc.printToString(Ctx.getSourceManager());
      switch(dsk){
          case DeadInit: {
	      llvm::errs() << text_loc << " dead init \n";
              break;
          }
          case Standard: {
	      llvm::errs() << text_loc << " standard ??? \n";
              break;
          }
	  case Enclosing :{
	      llvm::errs()<< text_loc << " enclosing ??? \n";
	      break;
	  }
	  case DeadIncrement :{
	      llvm::errs() << text_loc<< " dead increment ??? \n";
	      break;
	  }
      }
    }
  }

  void CheckDeclRef(const DeclRefExpr *DR, const Expr *Val, DeadStoreKind dsk,
                    const LiveVariables::LivenessValues& Live) {
    if (const VarDecl *VD = dyn_cast<VarDecl>(DR->getDecl()))
      CheckVarDecl(VD, DR, Val, dsk, Live);
  }

  bool isIncrement(VarDecl *VD, const BinaryOperator* B) {
    if (B->isCompoundAssignmentOp())
      return true;

    const Expr *RHS = B->getRHS()->IgnoreParenCasts();
    const BinaryOperator* BRHS = dyn_cast<BinaryOperator>(RHS);

    if (!BRHS)
      return false;

    const DeclRefExpr *DR;

    if ((DR = dyn_cast<DeclRefExpr>(BRHS->getLHS()->IgnoreParenCasts())))
      if (DR->getDecl() == VD)
        return true;

    if ((DR = dyn_cast<DeclRefExpr>(BRHS->getRHS()->IgnoreParenCasts())))
      if (DR->getDecl() == VD)
        return true;

    return false;
  }

  void observeStmt(const Stmt *S, const CFGBlock *block,
                   const LiveVariables::LivenessValues &Live) override {

    currentBlock = block;

    if ( S == Position ) {
	llvm::errs() << "found next position in the liveness analysis \n";
	if ( isLive(Live, VarD) ){
	    IsAlive = true;
	    llvm::errs() << "the iterator variable is alive at this point \n";
	}else{
	    // TODO it can be that it was used inside a initializer this need special handling
	    IsAlive = false;
	    // TODO save the block and check all coming statements in this block for usage of VarD
	    llvm::errs() << "the iterator variable is not alive at this point \n";
	}
    }
    return;
#if 0
    
    // Skip statements in macros.
    if (S->getLocStart().isMacroID())
      return;

    // Only cover dead stores from regular assignments.  ++/-- dead stores
    // have never flagged a real bug.
    if (const BinaryOperator* B = dyn_cast<BinaryOperator>(S)) {
      if (!B->isAssignmentOp()) return; // Skip non-assignments.

      if (DeclRefExpr *DR = dyn_cast<DeclRefExpr>(B->getLHS()))
        if (VarDecl *VD = dyn_cast<VarDecl>(DR->getDecl())) {
          // Special case: check for assigning null to a pointer.
          //  This is a common form of defensive programming.
          const Expr *RHS =
            LookThroughTransitiveAssignmentsAndCommaOperators(B->getRHS());
          RHS = RHS->IgnoreParenCasts();
          
          QualType T = VD->getType();
          if (T->isPointerType() || T->isObjCObjectPointerType()) {
            if (RHS->isNullPointerConstant(Ctx, Expr::NPC_ValueDependentIsNull))
              return;
          }

          // Special case: self-assignments.  These are often used to shut up
          //  "unused variable" compiler warnings.
          if (const DeclRefExpr *RhsDR = dyn_cast<DeclRefExpr>(RHS))
            if (VD == dyn_cast<VarDecl>(RhsDR->getDecl()))
              return;

          // Otherwise, issue a warning.
          DeadStoreKind dsk = Parents.isConsumedExpr(B)
                              ? Enclosing
                              : (isIncrement(VD,B) ? DeadIncrement : Standard);

          CheckVarDecl(VD, DR, B->getRHS(), dsk, Live);
        }
    }
    else if (const UnaryOperator* U = dyn_cast<UnaryOperator>(S)) {
      if (!U->isIncrementOp() || U->isPrefix())
        return;

      const Stmt *parent = Parents.getParentIgnoreParenCasts(U);
      if (!parent || !isa<ReturnStmt>(parent))
        return;

      const Expr *Ex = U->getSubExpr()->IgnoreParenCasts();

      if (const DeclRefExpr *DR = dyn_cast<DeclRefExpr>(Ex))
        CheckDeclRef(DR, U, DeadIncrement, Live);
    }
    else if (const DeclStmt *DS = dyn_cast<DeclStmt>(S))
      // Iterate through the decls.  Warn if any initializers are complex
      // expressions that are not live (never used).
      for (const auto *DI : DS->decls()) {
        const auto *V = dyn_cast<VarDecl>(DI);

        if (!V)
          continue;
          
        if (V->hasLocalStorage()) {          
          // Reference types confuse the dead stores checker.  Skip them
          // for now.
          if (V->getType()->getAs<ReferenceType>())
            return;
            
          if (const Expr *E = V->getInit()) {
            while (const ExprWithCleanups *exprClean =
                    dyn_cast<ExprWithCleanups>(E))
              E = exprClean->getSubExpr();
            
            // Look through transitive assignments, e.g.:
            // int x = y = 0;
            E = LookThroughTransitiveAssignmentsAndCommaOperators(E);
            
            // Don't warn on C++ objects (yet) until we can show that their
            // constructors/destructors don't have side effects.
            if (isa<CXXConstructExpr>(E))
              return;
            
            // A dead initialization is a variable that is dead after it
            // is initialized.  We don't flag warnings for those variables
            // marked 'unused' or 'objc_precise_lifetime'.
            if (!isLive(Live, V) &&
                !V->hasAttr<UnusedAttr>() &&
                !V->hasAttr<ObjCPreciseLifetimeAttr>()) {
              // Special case: check for initializations with constants.
              //
              //  e.g. : int x = 0;
              //
              // If x is EVER assigned a new value later, don't issue
              // a warning.  This is because such initialization can be
              // due to defensive programming.
              if (E->isEvaluatable(Ctx))
                return;

              if (const DeclRefExpr *DRE =
                  dyn_cast<DeclRefExpr>(E->IgnoreParenCasts()))
                if (const VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  // Special case: check for initialization from constant
                  //  variables.
                  //
                  //  e.g. extern const int MyConstant;
                  //       int x = MyConstant;
                  //
                  if (VD->hasGlobalStorage() &&
                      VD->getType().isConstQualified())
                    return;
                  // Special case: check for initialization from scalar
                  //  parameters.  This is often a form of defensive
                  //  programming.  Non-scalars are still an error since
                  //  because it more likely represents an actual algorithmic
                  //  bug.
                  if (isa<ParmVarDecl>(VD) && VD->getType()->isScalarType())
                    return;
                }

	      llvm::errs() << "dead init \n";
            }
          }
        }
      }
#endif
  }
};

} // end anonymous namespace



/// @from StaticAnalysis DeadStoreChecker
namespace {
class FindEscaped {
public:
  llvm::SmallPtrSet<const VarDecl*, 20> Escaped;

  void operator()(const Stmt *S) {
    // Check for '&'. Any VarDecl whose address has been taken we treat as
    // escaped.
    // FIXME: What about references?
    const UnaryOperator *U = dyn_cast<UnaryOperator>(S);
    if (!U)
      return;
    if (U->getOpcode() != UO_AddrOf)
      return;

    const Expr *E = U->getSubExpr()->IgnoreParenCasts();
    if (const DeclRefExpr *DR = dyn_cast<DeclRefExpr>(E))
      if (const VarDecl *VD = dyn_cast<VarDecl>(DR->getDecl()))
        Escaped.insert(VD);
  }
};
} // end anonymous namespace

/// @from StaticAnalysis DeadStoreChecker
namespace {
  bool checkASTCodeBody(const Decl *D, AnalysisDeclContextManager& mgr, ASTContext& Context, Stmt* pos, const VarDecl* var ) {

    // Don't do anything for template instantiations.
    // Proving that code in a template instantiation is "dead"
    // means proving that it is dead in all instantiations.
    // This same problem exists with -Wunreachable-code.
    if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D))
      if (FD->isTemplateInstantiation())
        return false;

    if (LiveVariables *L = mgr.getContext(D)->getAnalysis<LiveVariables>()) {
      CFG &cfg = *mgr.getContext(D)->getCFG();
      AnalysisDeclContext *AC = mgr.getContext(D);
      ParentMap &pmap = mgr.getContext(D)->getParentMap();
      FindEscaped FS; 
      cfg.VisitBlockStmts(FS);
      DeadStoreObs A(cfg, Context, AC, pmap, FS.Escaped, pos, var);
      L->runOnAllBlocks(A);
      return A.isAlive();
    }   
    return false;
  }
}

void UseLocalIteratorInForLoopFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  AnalysisDeclContextManager AnaCtxMgr(true,true,true,true,true,true,true);
  AnaCtxMgr.getCFGBuildOptions().setAllAlwaysAdd();


#if 0
  const auto* node = Result.Nodes.getNodeAs<BinaryOperator>(MatcherUseLocalIteratorInForLoopID);
  if ( node ) {
      if ( !Owner.isInRange( node, SM ) ) return;

      // TODO one lucky day implement liveness analysis here 
      // right now it takes to much time.
      //LiveVariables* L = 

      auto* decl_ref = dyn_cast_or_null<DeclRefExpr>(node->getLHS());
      DeadStoresChecker dsc;


      auto* value_decl = decl_ref->getDecl();

      dsc.checkASTCodeBody( value_decl, AnaCtxMgr, context ); 
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
#endif
  const auto* node = Result.Nodes.getNodeAs<FunctionDecl>(MatcherUseLocalIteratorInForLoopID);
  const auto* for_stmt = Result.Nodes.getNodeAs<ForStmt>(MatcherForStmtID);
  const auto* decl_ref = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherDeclRefID);

  if ( node ){
	ParentMap &pmap = AnaCtxMgr.getContext(node)->getParentMap();
	// then pass the nextpos to the analysis and find out whether it is alive
	// get statement after the for statement and check whether 
	// the iterator is alive at this point
	auto* init = for_stmt->getInit();
	auto* parent = pmap.getParentIgnoreParenCasts(for_stmt);
	if ( auto* compound_stmt = dyn_cast_or_null<CompoundStmt>(parent) ){
	    auto pos_in_compound = std::find( compound_stmt->body_begin(), compound_stmt->body_end(), for_stmt );
	    if ( pos_in_compound == compound_stmt->body_end() ) {
		llvm::errs() << "not found \n";
		// this must never happen
		exit(-1);
	    }
	    auto next_pos = pos_in_compound;
	    next_pos++;

	    // check to be a valid position
	    // // TODO this means its save to local declare
	    if ( next_pos == compound_stmt->body_end() ) 
		return;

	    auto start_loc = (*next_pos)->getLocStart();
	    auto text_loc = start_loc.printToString(SM);
	    llvm::errs() << "next pos is " << text_loc << "\n";

	    if ( auto* var_decl = dyn_cast_or_null<VarDecl>(decl_ref->getDecl()) ){
		if( !checkASTCodeBody( node, AnaCtxMgr, context, *next_pos, var_decl ) ) {
		  auto* value_decl = decl_ref->getDecl();
		  string type_text = value_decl->getType().getAsString();
		  SourceLocation StartLoc = decl_ref->getLocStart();
		  SourceLocation EndLoc = decl_ref->getLocEnd();
		  // TODO get the type of the original iterator 
		  string replacement = type_text + string(" ") + getString( decl_ref, SM );
		  ReplaceWith( Owner, SM, StartLoc, EndLoc, context, replacement );
		}
	    }
	}else{
	    llvm::errs() << "parent is a \n";
	    parent->dumpColor();
	}
  }
}














