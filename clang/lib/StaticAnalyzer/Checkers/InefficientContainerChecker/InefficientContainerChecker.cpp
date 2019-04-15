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
  Finder.addMatcher(
      declStmt(
          hasDescendant(
              varDecl(hasType(getVectorTypeMatcher()),
                      hasDescendant(cxxConstructExpr(
                          hasDescendant(expr().bind(VectorSize)))))
                  .bind(VectorDecl)),
          hasParent(
                  compoundStmt(unless(anyOf(hasDescendant(cxxMemberCallExpr(
                          on(declRefExpr(
                                  hasDeclaration(equalsBoundNode(VectorDecl)))),
                          callee(cxxMethodDecl(anyOf(
                                  hasName("clear"), hasName("insert"), hasName("emplace"),
                                  hasName("erase"), hasName("push_back"), hasName("emplace_back"),
                                  hasName("pop_back"), hasName("resize"), hasName("swap")))))),
                                            hasDescendant(callExpr(hasDescendant(declRefExpr(hasDeclaration(equalsBoundNode(VectorDecl))))))
                  )))
                          .bind(ListCompStmt)))
          .bind(Vector),
      CB);

  Finder.addMatcher(
      declStmt(
          hasDescendant(varDecl(hasType(getListTypeMatcher())).bind(ListDecl)),
          hasParent(
              compoundStmt(unless(anyOf(hasDescendant(cxxMemberCallExpr(
                               on(declRefExpr(
                                   hasDeclaration(equalsBoundNode(ListDecl)))),
                               callee(cxxMethodDecl(anyOf(
                                   hasName("size"), hasName("back"), hasName("rbegin"),
                                   hasName("crbegin"), hasName("rend"), hasName("crend"),
                                   hasName("insert"), hasName("emplace"), hasName("push_back"),
                                   hasName("emplace_back"), hasName("pop_back"), hasName("splice")))))),
                                           hasDescendant(callExpr(hasDescendant(declRefExpr(hasDeclaration(equalsBoundNode(ListDecl))))))
                                           )))
                  .bind(ListCompStmt)))
          .bind(List),
      CB);
}

void InefficientContainerChecker::check(const MatchFinder::MatchResult &Result) {
  const auto MatchedVector = *Result.Nodes.getNodeAs<DeclStmt>(Vector);
  const auto MatchedVectorDecl = *Result.Nodes.getNodeAs<VarDecl>(VectorDecl);


/*  const auto *MatchedVectorType = Result.Nodes.getNodeAs<QualType>(VectorType);
  const auto *MatchedVectorSize = Result.Nodes.getNodeAs<Expr>(VectorSize);

  const auto *MatchedList = Result.Nodes.getNodeAs<DeclStmt>(List);
  const auto *MatchedListDecl = Result.Nodes.getNodeAs<VarDecl>(ListDecl);
  const auto *MatchedListType = Result.Nodes.getNodeAs<QualType>(ListType);
  const auto *MatchedCompStmt =
      Result.Nodes.getNodeAs<CompoundStmt>(ListCompStmt);

  if (MatchedList) {
    const std::string insertion = "std::forward_list<" +
                                  MatchedListType->getAsString() + "> " +
                                  MatchedListDecl->getName().str() + ";";
    *//*diag(MatchedList->getBeginLoc(), "use std::forward_list instead")
        << MatchedList->getSingleDecl()->getDeclKindName()
        << FixItHint::CreateInsertion(MatchedList->getBeginLoc(), insertion);*//*
  } else if (MatchedVector) {
    llvm::APSInt result;
    if(MatchedVectorSize->isIntegerConstantExpr(result, *Result.Context))
    {
        const std::string insertion =
                "std::array<" + MatchedVectorType->getAsString() + ", " +
                result.toString(10) + "> " + MatchedVectorDecl->getName().str() + ";";
        *//*diag(MatchedVector->getBeginLoc(), "use std::array instead")
                << MatchedVector->getSingleDecl()->getDeclKindName()
                << FixItHint::CreateInsertion(MatchedVector->getBeginLoc(), insertion);*//*
    }
  }*/
}

void InefficientContainerChecker::checkASTCodeBody(const Decl *D,
                                                     AnalysisManager &AM,
                                                     BugReporter &BR) const {
    MatchFinder PossiblyInefficientContainersCollector;

    CandidateStorage Storage;
    ContainerUsageStatisticsCallback CB(this, BR, AM.getAnalysisDeclContext(D), Storage);
    registerContainerMatchers(PossiblyInefficientContainersCollector, &CB);
    PossiblyInefficientContainersCollector.match(*D->getBody(), AM.getASTContext());




}

} // namespace inefficientcontainer

void registerInefficientContainerChecker(CheckerManager &Mgr) {
    Mgr.registerChecker<clang::ento::inefficientcontainer::InefficientContainerChecker>();
}

} // namespace ento
} // namespace clang
