//===-- TransformationTemplate/TransformationTemplateActions.h - Matcher callback ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the TransformationTemplateFixer class which
/// is used as a ASTMatcher callback.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Core/Transform.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring.h"

class TransformationTemplateFixer : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  TransformationTemplateFixer(unsigned &AcceptedChanges, Transform &Owner) :
      AcceptedChanges(AcceptedChanges), 
      Owner(Owner) 
  {

  }

  /// \brief Entry point to the callback called when matches are made.
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);

private:
  unsigned &AcceptedChanges;
  Transform &Owner;
};
