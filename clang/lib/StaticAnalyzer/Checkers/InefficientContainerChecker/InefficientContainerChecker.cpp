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

constexpr char VarDecl[] = "VarDecl";

using ListMatcher = ast_matchers::internal::BindableMatcher<QualType>;

ListMatcher getListTypeMatcher() {
  return qualType(hasUnqualifiedDesugaredType(
      recordType(hasDeclaration(classTemplateSpecializationDecl(
          hasName("::std::list"),
          hasTemplateArgument(0, templateArgument(refersToType(
                                     qualType().bind(ListType)))))))));
}

ListMatcher getVectorTypeMatcher() {
    return qualType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(classTemplateSpecializationDecl(hasName("::std::vector"),hasTemplateArgument(0, templateArgument(refersToType(qualType().bind(VectorType)))))))));
}

void InefficientContainerChecker::registerContainerMatchers(ast_matchers::MatchFinder& Finder,
                                                            ContainerUsageStatisticsCallback* CB) const {
    Finder.addMatcher(stmt(forEachDescendant(varDecl(hasType(getVectorTypeMatcher())).bind(VarDecl))), CB);
}

void InefficientContainerChecker::registerOperationMatchers(ast_matchers::MatchFinder& Finder,
                                                            OperationStatisticsCallback* CB) const {
    Finder.addMatcher(
            stmt(forEachDescendant(cxxMemberCallExpr(
                                    on(declRefExpr(hasDeclaration(varDecl().bind("VectorDecl")))),
                                    callee(cxxMethodDecl(anyOf(hasName("push_back"), hasName("insert"), hasName("emplace"))))).bind("Op"))),
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

    // Collect an informations about performed operations on the containers
    MatchFinder OperationCollector;
    OperationStatisticsCallback CBO(this, BR, AM.getAnalysisDeclContext(D), Storage);
    registerOperationMatchers(OperationCollector, &CBO);
    OperationCollector.match(*D->getBody(), AM.getASTContext());

}

} // namespace inefficientcontainer

void registerInefficientContainerChecker(CheckerManager &Mgr) {
    Mgr.registerChecker<clang::ento::inefficientcontainer::InefficientContainerChecker>();
}

} // namespace ento
} // namespace clang
