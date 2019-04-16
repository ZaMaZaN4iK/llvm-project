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
    auto MatchedNode = Result.Nodes.getMap().find("VarDecl")->second;

    // Initialize node. Must be incapsulated somehow

    std::unordered_map<OperationType, std::size_t> value = {{ OperationType::Add, 0 },
                                                            {OperationType::Read, 0},
                                                            {OperationType::Update, 0},
                                                            {OperationType::Delete, 0}};
    Storage.insert(std::make_pair(MatchedNode, value) );
}


} // namespace inefficientcontainer
} // namespace ento
} // namespace clang
