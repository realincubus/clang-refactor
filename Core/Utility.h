

#pragma once

using namespace clang::tooling;
using namespace clang;
#include "clang/AST/ASTContext.h"
#include "clang/Lex/Lexer.h"

namespace TransformationUtility {
    static bool isReplaceableRange(SourceLocation StartLoc, SourceLocation EndLoc,
			    const SourceManager &SM, const Transform &Owner) {
      return SM.isWrittenInSameFile(StartLoc, EndLoc) &&
	     Owner.isFileModifiable(SM, StartLoc);
    }

    static std::string getString( SourceRange sr, const SourceManager& SM ){
	return Lexer::getSourceText( CharSourceRange::getTokenRange(sr), SM, LangOptions());
    }

    static std::string getString( const Stmt* expr , const SourceManager& SM ){
	SourceLocation expr_start = expr->getLocStart();
	SourceLocation expr_end = expr->getLocEnd();
	return Lexer::getSourceText( CharSourceRange::getTokenRange(SourceRange(expr_start, expr_end)), SM, LangOptions());
    }

    static bool areSameExpr(ASTContext *Context, const Expr *First,
				    const Expr *Second) {
	if (!First || !Second)
	return false;

	llvm::FoldingSetNodeID FirstID, SecondID;
	First->Profile(FirstID, *Context, true);
	Second->Profile(SecondID, *Context, true);
	return FirstID == SecondID;
    }
    static void ReplaceWithSelf(Transform &Owner, SourceManager &SM, SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, const Expr* self ){ 
      using namespace std;

      CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

      string source_text = getString( self, SM );
      string replacement_text = source_text; 

      if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
	  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement_text ));
      }
    }

    static void ReplaceRangeWithRange(Transform &Owner, SourceManager &SM, const clang::ASTContext& Context, SourceRange replace_range, const SourceRange replacement_range ){ 
      using namespace std;

      CharSourceRange Range(replace_range, true);
      string replacement_text = getString( replacement_range, SM );


      if ( isReplaceableRange( replace_range.getBegin(), replace_range.getEnd(), SM, Owner ) ){ 
	  Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement_text ));
      }
    }


}
