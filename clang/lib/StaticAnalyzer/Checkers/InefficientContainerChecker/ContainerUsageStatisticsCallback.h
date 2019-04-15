//===- ContainerUsageStatisticsCallback.h -------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CONTAINERUSAGESTATISTICSCALLBACK_H
#define LLVM_CONTAINERUSAGESTATISTICSCALLBACK_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"

namespace clang {
namespace ento {
namespace inefficientcontainer {

class InefficientContainerChecker;

class ContainerUsageStatisticsCallback : public ast_matchers::MatchFinder::MatchCallback
{
    const InefficientContainerChecker* C;
    BugReporter& BR;
    AnalysisDeclContext* ADC;

public:
    ContainerUsageStatisticsCallback(const InefficientContainerChecker* C,
                                     BugReporter& BR, AnalysisDeclContext* ADC)
            : C(C), BR(BR), ADC(ADC)
    {}

    virtual void run(const ast_matchers::MatchFinder::MatchResult& Result);
};

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang

#endif //LLVM_CONTAINERUSAGESTATISTICSCALLBACK_H
