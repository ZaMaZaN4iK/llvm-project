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
    auto MatchedNode = Result.Nodes.getMap().find(VariableDeclaration)->second;

    // Initialize node. Have to be incapsulated somehow
    std::unordered_map<OperationType, std::size_t> value = {{ OperationType::Add_Begin, 0 },
                                                            { OperationType::Add_Middle, 0 },
                                                            { OperationType::Add_End, 0 },
                                                            { OperationType::Read, 0 },
                                                            { OperationType::Update, 0 },
                                                            { OperationType::Delete_Begin, 0 },
                                                            { OperationType::Delete_Middle, 0 },
                                                            { OperationType::Delete_End, 0 }};
    Storage.insert(std::make_pair(MatchedNode, value));
}

void OperationStatisticsCallback::run(const ast_matchers::MatchFinder::MatchResult& Result)
{
    auto MatchedNode = Result.Nodes.getMap().find("Op")->second.get<CXXMemberCallExpr>();
    auto MatchedCalleeDecl = Result.Nodes.getMap().find("VectorDecl")->second.get<VarDecl>();
    if(MatchedNode != nullptr && MatchedCalleeDecl != nullptr)
    {
        clang::ast_type_traits::DynTypedNode node = clang::ast_type_traits::DynTypedNode::create(*MatchedCalleeDecl);
        auto it = Storage.find(node);
        if(it != Storage.end())
        {
           it->second[OperationType::Add_Begin]++;
        }
    }
}

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang
