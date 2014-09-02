//===-- TransformationTemplate/TransformationTemplateMatchers.cpp - Matchers for null casts ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definitions for matcher-generating functions
///
//===----------------------------------------------------------------------===//

#include "TransformationTemplateMatchers.h"
#include "clang/AST/ASTContext.h"

using namespace clang::ast_matchers;
using namespace clang;

const char *MatcherTransformationTemplateID = "matcherTransformationTemplateID";


StatementMatcher makeTransformationTemplateMatcher(){
    return anything().bind(MatcherTransformationTemplateID);
}












