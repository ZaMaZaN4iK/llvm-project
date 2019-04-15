//===- InefficientContainerChecker.h -------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_INEFFICIENTCONTAINERCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_INEFFICIENTCONTAINERCHECK_H

#include "ContainerUsageStatisticsCallback.h"

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"

namespace clang {
namespace ento {
namespace inefficientcontainer {

class InefficientContainerChecker : public Checker<check::ASTCodeBody> {
public:
  InefficientContainerChecker() = default;

  void checkASTCodeBody(const Decl *D, AnalysisManager &AM, BugReporter &BR) const;

private:
  void registerMatchers(ast_matchers::MatchFinder& Finder, ContainerUsageStatisticsCallback* CB) const;
  void check(const ast_matchers::MatchFinder::MatchResult &Result);
};

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_INEFFICIENTCONTAINERCHECK_H
