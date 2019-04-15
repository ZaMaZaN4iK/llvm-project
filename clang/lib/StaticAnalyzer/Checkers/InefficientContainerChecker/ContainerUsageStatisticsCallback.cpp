//===- ContainerUsageStatisticsCallback.cpp -------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ContainerUsageStatisticsCallback.h"

namespace clang {
namespace ento {
namespace inefficientcontainer {


void ContainerUsageStatisticsCallback::run(const ast_matchers::MatchFinder::MatchResult& Result)
{
    // Store statistic somewhere
}


} // namespace inefficientcontainer
} // namespace ento
} // namespace clang
