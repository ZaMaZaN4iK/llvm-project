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

#include "HelperTypes.h"

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
                                     BugReporter& BR, AnalysisDeclContext* ADC,
                                     CandidateStorage &Storage)
            : C(C), BR(BR), ADC(ADC), Storage(Storage)
    {}

    void run(const ast_matchers::MatchFinder::MatchResult& Result) override;

private:
    CandidateStorage& Storage;
};

class OperationStatisticsCallback : public ast_matchers::MatchFinder::MatchCallback
{
    const InefficientContainerChecker* C;
    BugReporter& BR;
    AnalysisDeclContext* ADC;

public:
    OperationStatisticsCallback(const InefficientContainerChecker* C,
                                     BugReporter& BR, AnalysisDeclContext* ADC,
                                     CandidateStorage &Storage)
            : C(C), BR(BR), ADC(ADC), Storage(Storage)
    {}

    void run(const ast_matchers::MatchFinder::MatchResult& Result) override;

private:
    CandidateStorage& Storage;
};

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang

#endif //LLVM_CONTAINERUSAGESTATISTICSCALLBACK_H
