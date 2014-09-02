//===-- UseRAII/UseRAII.h - C++11 nullptr migration -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides the definition of the UseRAIITransform
/// class which is the main interface to the use-nullptr transform that tries to
/// make use of nullptr where possible.
///
//===----------------------------------------------------------------------===//

#ifndef CLANG_MODERNIZE_USE_NULLPTR_H
#define CLANG_MODERNIZE_USE_NULLPTR_H

#include "Core/Transform.h"
#include "llvm/Support/Compiler.h" // For override

/// \brief Subclass of Transform that transforms null pointer constants into
/// C++11's nullptr keyword where possible.
class UseRAIITransform : public Transform {
public:
  UseRAIITransform(const TransformOptions &Options)
      : Transform("UseRAII", Options) {}

  /// \see Transform::run().
  virtual int apply(const clang::tooling::CompilationDatabase &Database,
                    const std::vector<std::string> &SourcePaths,
		    const llvm::cl::list<std::string>& LineRanges 
		    ) override;
};

#endif // CLANG_MODERNIZE_USE_NULLPTR_H
