//===-- Core/Transform.cpp - Transform Base Class Def'n -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the definition for the base Transform class from
/// which all transforms must subclass.
///
//===----------------------------------------------------------------------===//

#include "Core/Transform.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/STLExtras.h"

using namespace clang;

llvm::cl::OptionCategory TransformsOptionsCategory("Transforms' options");

namespace {

using namespace tooling;
using namespace ast_matchers;

/// \brief Custom FrontendActionFactory to produce FrontendActions that simply
/// forward (Begin|End)SourceFileAction calls to a given Transform.
class ActionFactory : public clang::tooling::FrontendActionFactory {
public:
  ActionFactory(MatchFinder &Finder, Transform &Owner)
  : Finder(Finder), Owner(Owner) {}

  virtual FrontendAction *create() LLVM_OVERRIDE {
    return new FactoryAdaptor(Finder, Owner);
  }

private:
  class FactoryAdaptor : public ASTFrontendAction {
  public:
    FactoryAdaptor(MatchFinder &Finder, Transform &Owner)
        : Finder(Finder), Owner(Owner) {}

    ASTConsumer *CreateASTConsumer(CompilerInstance &, StringRef) {
      return Finder.newASTConsumer();
    }

    virtual bool BeginSourceFileAction(CompilerInstance &CI,
                                       StringRef Filename) LLVM_OVERRIDE {
      if (!ASTFrontendAction::BeginSourceFileAction(CI, Filename))
        return false;

      return Owner.handleBeginSource(CI, Filename);
    }

    virtual void EndSourceFileAction() LLVM_OVERRIDE {
      Owner.handleEndSource();
      return ASTFrontendAction::EndSourceFileAction();
    }

  private:
    MatchFinder &Finder;
    Transform &Owner;
  };

  MatchFinder &Finder;
  Transform &Owner;
};
} // namespace

Transform::Transform(llvm::StringRef Name, const TransformOptions &Options)
    : Name(Name), GlobalOptions(Options) {
  Reset();
}

Transform::~Transform() {}

bool Transform::isInRange( const clang::Expr* expr , const clang::SourceManager& SM ){

    // assume all was ment if not present
    if ( line_ranges.empty() ) return true;

    auto start_loc = expr->getLocStart();
    auto end_loc = expr->getLocEnd();
    FullSourceLoc full_start_loc( start_loc, SM );
    FullSourceLoc full_end_loc( end_loc, SM );
    auto expr_line_begin = full_start_loc.getSpellingLineNumber();
    auto expr_column_begin = full_start_loc.getSpellingColumnNumber();
    auto expr_line_end = full_end_loc.getSpellingLineNumber();
    auto expr_column_end = full_end_loc.getSpellingColumnNumber();
    for( auto line_range : line_ranges ){

	if ( expr_line_begin >= line_range.line_begin && expr_line_end <= line_range.line_end ){
	    if ( expr_line_begin == line_range.line_end ){
		if ( ! ( expr_column_begin >= line_range.column_begin && expr_column_end <= line_range.column_end ) ){
		    return false;
		}
	    }
	    return true;
	}
    }
    return false;
}



bool Transform::isFileModifiable(const SourceManager &SM,
                                 const SourceLocation &Loc) const {
  if (SM.isWrittenInMainFile(Loc))
    return true;

  const FileEntry *FE = SM.getFileEntryForID(SM.getFileID(Loc));
  if (!FE)
    return false;

  return GlobalOptions.ModifiableFiles.isFileIncluded(FE->getName());
}

bool Transform::handleBeginSource(CompilerInstance &CI, StringRef Filename) {
  CurrentSource = Filename;

  if (Options().EnableTiming) {
    Timings.push_back(std::make_pair(Filename.str(), llvm::TimeRecord()));
    Timings.back().second -= llvm::TimeRecord::getCurrentTime(true);
  }
  return true;
}

void Transform::handleEndSource() {
  CurrentSource.clear();
  if (Options().EnableTiming)
    Timings.back().second += llvm::TimeRecord::getCurrentTime(false);
}

void Transform::addTiming(llvm::StringRef Label, llvm::TimeRecord Duration) {
  Timings.push_back(std::make_pair(Label.str(), Duration));
}

bool
Transform::addReplacementForCurrentTU(const clang::tooling::Replacement &R) {
  if (CurrentSource.empty())
    return false;

  TranslationUnitReplacements &TU = Replacements[CurrentSource];
  if (TU.MainSourceFile.empty())
    TU.MainSourceFile = CurrentSource;
  TU.Replacements.push_back(R);

  return true;
}

FrontendActionFactory *Transform::createActionFactory(MatchFinder &Finder) {
  return new ActionFactory(Finder, /*Owner=*/ *this);
}

Version Version::getFromString(llvm::StringRef VersionStr) {
  llvm::StringRef MajorStr, MinorStr;
  Version V;

  llvm::tie(MajorStr, MinorStr) = VersionStr.split('.');
  if (!MinorStr.empty()) {
    llvm::StringRef Ignore;
    llvm::tie(MinorStr, Ignore) = MinorStr.split('.');
    if (MinorStr.getAsInteger(10, V.Minor))
      return Version();
  }
  if (MajorStr.getAsInteger(10, V.Major))
    return Version();
  return V;
}

TransformFactory::~TransformFactory() {}

namespace {
bool versionSupported(Version Required, Version AvailableSince) {
  // null version, means no requirements, means supported
  if (Required.isNull())
    return true;
  return Required >= AvailableSince;
}
} // end anonymous namespace

bool TransformFactory::supportsCompilers(CompilerVersions Required) const {
  return versionSupported(Required.Clang, Since.Clang) &&
         versionSupported(Required.Gcc, Since.Gcc) &&
         versionSupported(Required.Icc, Since.Icc) &&
         versionSupported(Required.Msvc, Since.Msvc);
}
