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

    // Node initialization. Has to be incapsulated somehow
    std::unordered_map<OperationType, std::size_t> value = {{ OperationType::Add_Begin, 0 },
                                                            { OperationType::Add_Middle, 0 },
                                                            { OperationType::Add_End, 0 },
                                                            { OperationType::Read, 0 },
                                                            { OperationType::Update, 0 },
                                                            { OperationType::Delete_Begin, 0 },
                                                            { OperationType::Delete_Middle, 0 },
                                                            { OperationType::Delete_End, 0 },
                                                            { OperationType::Other, 0 }};
    Storage.insert(std::make_pair(MatchedNode, value));
}

void OperationStatisticsCallback::run(const ast_matchers::MatchFinder::MatchResult& Result)
{
    const auto& MatchedOperationNodes = Result.Nodes.getMap();
    // Match variable declaration
    const auto MatchedCalleeDecl = MatchedOperationNodes.find(VariableDeclaration)->second.get<VarDecl>();

    // Something went wrong
    if(MatchedCalleeDecl == nullptr)
    {
        return;
    }

    auto MatchedOperation = OperationType::Other;

    // Find currently matched operation
    if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Add_Begin)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Add_Begin;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Add_Middle)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Add_Middle;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Add_End)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Add_End;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Delete_Begin)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Delete_Begin;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Delete_Middle)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Delete_Middle;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Delete_End)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Delete_End;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Read)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Read;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Update)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Update;
    }
    else if(MatchedOperationNodes.find(OperationTypeToString(OperationType::Other)) != MatchedOperationNodes.cend())
    {
        MatchedOperation = OperationType::Other;
    }

    // Find proper node from current scope
    clang::ast_type_traits::DynTypedNode node = clang::ast_type_traits::DynTypedNode::create(*MatchedCalleeDecl);
    auto it = Storage.find(node);
    if(it != Storage.end())
    {
        it->second[MatchedOperation]++;
    }
}

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang
