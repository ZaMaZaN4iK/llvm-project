//===- InefficientContainerChecker.cpp -------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "InefficientContainerChecker.h"

#include "ContainerUsageStatisticsCallback.h"
#include "HelperTypes.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"

#include <iostream>
#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace ento {
namespace inefficientcontainer {

constexpr char Vector[] = "vector";
constexpr char VectorDecl[] = "vectorDecl";
constexpr char VectorType[] = "vectorType";
constexpr char VectorSize[] = "vectorSize";

constexpr char List[] = "list";
constexpr char ListDecl[] = "listDecl";
constexpr char ListType[] = "listType";
constexpr char ListCompStmt[] = "listCompStmt";


using ListMatcher = ast_matchers::internal::BindableMatcher<QualType>;

std::string OperationTypeToString(const OperationType opType)
{
    switch (opType)
    {
        case OperationType::Add_Begin:
            return "AddBegin";
        case OperationType::Add_Middle:
            return "AddMiddle";
        case OperationType::Add_End:
            return "AddEnd";
        case OperationType::Read:
            return "Read";
        case OperationType::Update:
            return "Update";
        case OperationType::Delete_Begin:
            return "DeleteBegin";
        case OperationType::Delete_Middle:
            return "DeleteMiddle";
        case OperationType::Delete_End:
            return "DeleteEnd";
        case OperationType::Other:
            return "Other";
        default:
            return "";
    }
}

ListMatcher getListTypeMatcher() {
  return qualType(hasUnqualifiedDesugaredType(
      recordType(hasDeclaration(classTemplateSpecializationDecl(
          hasName("::std::list"),
          hasTemplateArgument(0, templateArgument(refersToType(
                                     qualType().bind(ListType)))))))));
}

ListMatcher getVectorTypeMatcher() {
    return qualType(hasUnqualifiedDesugaredType(
            recordType(hasDeclaration(classTemplateSpecializationDecl(
                    hasName("::std::vector"),
                    hasTemplateArgument(0, templateArgument(refersToType(
                            qualType().bind(VectorType)))))))));
}

void InefficientContainerChecker::registerContainerMatchers(ast_matchers::MatchFinder& Finder,
                                                            ContainerUsageStatisticsCallback* CB) const {
    Finder.addMatcher(stmt(forEachDescendant(varDecl(hasType(getVectorTypeMatcher())).bind(VariableDeclaration))), CB);
    Finder.addMatcher(stmt(forEachDescendant(varDecl(hasType(getListTypeMatcher())).bind(VariableDeclaration))), CB);
}

void InefficientContainerChecker::registerOperationMatchers(ast_matchers::MatchFinder& Finder,
                                                            OperationStatisticsCallback* CB) const {

    // Add operations
    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                    callee(cxxMethodDecl(anyOf(hasName("push_front"), hasName("emplace_front"))))).
                    bind(OperationTypeToString(OperationType::Add_Begin)))),
            CB);

    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                                    callee(cxxMethodDecl(anyOf(hasName("insert"), hasName("emplace"))))).
                                    bind(OperationTypeToString(OperationType::Add_Middle)))),
            CB);

    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                    callee(cxxMethodDecl(anyOf(hasName("push_back"), hasName("emplace_back"))))).
                    bind(OperationTypeToString(OperationType::Add_End)))),
            CB);

    // Delete operations
    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                    callee(cxxMethodDecl(hasName("pop_front")))).
                    bind(OperationTypeToString(OperationType::Delete_Begin)))),
            CB);

    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                    callee(cxxMethodDecl(hasName("erase")))).
                    bind(OperationTypeToString(OperationType::Delete_Middle)))),
            CB);

    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                    on(declRefExpr(hasDeclaration(varDecl().bind(VariableDeclaration)))),
                    callee(cxxMethodDecl(hasName("pop_back")))).
                    bind(OperationTypeToString(OperationType::Delete_End)))),
            CB);

}



void InefficientContainerChecker::checkASTCodeBody(const Decl *D,
                                                     AnalysisManager &AM,
                                                     BugReporter &BR) const {
    CandidateStorage Storage;

    // Collect container definitions and save their AST nodes
    MatchFinder PossiblyInefficientContainersCollector;
    ContainerUsageStatisticsCallback CB(this, BR, AM.getAnalysisDeclContext(D), Storage);
    registerContainerMatchers(PossiblyInefficientContainersCollector, &CB);
    PossiblyInefficientContainersCollector.match(*D->getBody(), AM.getASTContext());

    // Collect information about performed operations on the containers
    MatchFinder OperationCollector;
    OperationStatisticsCallback CBO(this, BR, AM.getAnalysisDeclContext(D), Storage);
    registerOperationMatchers(OperationCollector, &CBO);
    OperationCollector.match(*D->getBody(), AM.getASTContext());

    // Start container efficiency analysis
    analyze(Storage, BR, AM);
}

void InefficientContainerChecker::analyze(const CandidateStorage& storage, BugReporter &BR, AnalysisManager &AM) const
{
    for(const auto& VariableDeclaration : storage)
    {
        const auto& OperationOnVariable = VariableDeclaration.second;
        if(OperationOnVariable.find(OperationType::Add_Begin)->second +
           OperationOnVariable.find(OperationType::Add_Middle)->second <
           OperationOnVariable.find(OperationType::Add_End)->second)
        {
            const auto wrongDecl = VariableDeclaration.first.get<VarDecl>();

            //Wrong matched variable declaration
            if(wrongDecl == nullptr)
            {
                continue;
            }

            BR.EmitBasicReport(wrongDecl, this, "Inefficient container", "Performance degradation",
                    "Found inefficient container usage",
                    PathDiagnosticLocation::createBegin(wrongDecl, BR.getSourceManager(),
                            AM.getAnalysisDeclContext(wrongDecl)), wrongDecl->getSourceRange());
        }
    }
}

} // namespace inefficientcontainer

void registerInefficientContainerChecker(CheckerManager &Mgr) {
    Mgr.registerChecker<clang::ento::inefficientcontainer::InefficientContainerChecker>();
}

} // namespace ento
} // namespace clang
