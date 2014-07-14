
#pragma once

using namespace clang::tooling;
using namespace clang;
#include "clang/AST/ASTContext.h"
#include "clang/Lex/Lexer.h"

namespace TransformationUtility {
inline bool isReplaceableRange(SourceLocation StartLoc, SourceLocation EndLoc,
                               const SourceManager &SM,
                               const Transform &Owner) {
  return SM.isWrittenInSameFile(StartLoc, EndLoc) &&
         Owner.isFileModifiable(SM, StartLoc);
}

inline std::string getString(const SourceRange sr, const SourceManager &SM) {
  return Lexer::getSourceText(CharSourceRange::getTokenRange(sr), SM,
                              LangOptions());
}

template <typename T>
inline std::string getString(const T *node, const SourceManager &SM) {
  SourceLocation expr_start = node->getLocStart();
  SourceLocation expr_end = node->getLocEnd();
  return Lexer::getSourceText(
      CharSourceRange::getTokenRange(SourceRange(expr_start, expr_end)), SM,
      LangOptions());
}

inline bool areSameExpr(ASTContext *Context, const Expr *First,
                        const Expr *Second) {
  if (!First || !Second)
    return false;

  llvm::FoldingSetNodeID FirstID, SecondID;
  First->Profile(FirstID, *Context, true);
  Second->Profile(SecondID, *Context, true);
  return FirstID == SecondID;
}
inline void ReplaceWithSelf(Transform &Owner, SourceManager &SM,
                            SourceLocation StartLoc, SourceLocation EndLoc,
                            const clang::ASTContext &Context,
                            const Expr *self) {
  using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  string source_text = getString(self, SM);
  string replacement_text = source_text;

  if (isReplaceableRange(StartLoc, EndLoc, SM, Owner)) {
    Owner.addReplacementForCurrentTU(
        tooling::Replacement(SM, Range, replacement_text));
  }
}
inline void ReplaceWithString(Transform &Owner, SourceManager &SM,
                              SourceLocation StartLoc, SourceLocation EndLoc,
                              const clang::ASTContext &Context,
                              std::string replacement) {
  using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  if (isReplaceableRange(StartLoc, EndLoc, SM, Owner)) {
    Owner.addReplacementForCurrentTU(
        tooling::Replacement(SM, Range, replacement));
  }
}

inline void ReplaceRangeWithRange(Transform &Owner, SourceManager &SM,
                                  const clang::ASTContext &Context,
                                  SourceRange replace_range,
                                  const SourceRange replacement_range) {
  using namespace std;

  CharSourceRange Range(replace_range, true);
  string replacement_text = getString(replacement_range, SM);

  if (isReplaceableRange(replace_range.getBegin(), replace_range.getEnd(), SM,
                         Owner)) {
    Owner.addReplacementForCurrentTU(
        tooling::Replacement(SM, Range, replacement_text));
  }
}
inline std::string getTypeString(VarDecl *vdecl, SourceManager& SM) {
  auto type_loc = vdecl->getTypeSourceInfo()->getTypeLoc();
  auto range = type_loc.getSourceRange();
      return getString( range, SM );
}
}
