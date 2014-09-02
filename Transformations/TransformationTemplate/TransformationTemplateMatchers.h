//===-- RemoveUnusedParameter/RemoveUnusedParameterMatchers.h - Matchers for null casts --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declarations for matcher-generating functions
/// and names for bound nodes found by AST matchers.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "clang/ASTMatchers/ASTMatchers.h"

// Names to bind with matched expressions.
extern const char *MatcherTransformationTemplateID;

clang::ast_matchers::StatementMatcher makeTransformationTemplateMatcher();

