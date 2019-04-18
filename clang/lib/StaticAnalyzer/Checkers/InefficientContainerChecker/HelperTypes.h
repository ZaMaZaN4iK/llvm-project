//===- HelperTypes.h -------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_HELPERTYPES_H
#define LLVM_HELPERTYPES_H

#include "clang/AST/ASTTypeTraits.h"

#include <map>
#include <unordered_map>

namespace clang {
namespace ento {
namespace inefficientcontainer {

static constexpr char VariableDeclaration[] = "VarDecl";

enum class OperationType {
    Add_Begin,
    Add_Middle,
    Add_End,
    Read,
    Update,
    Delete_Begin,
    Delete_Middle,
    Delete_End
};

std::string OperationTypeToString(const OperationType opType);

using CandidateStorage = std::map<clang::ast_type_traits::DynTypedNode,
    std::unordered_map<OperationType, std::size_t>>;

} // namespace inefficientcontainer
} // namespace ento
} // namespace clang

#endif //LLVM_HELPERTYPES_H
