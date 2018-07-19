//===--- ArithmeticEvaluator.cpp ------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// Simple arithmetic evaluator using the Evaluator class.
//
//===----------------------------------------------------------------------===//
#include "swift/AST/DiagnosticEngine.h"
#include "swift/AST/Evaluator.h"
#include "swift/AST/SimpleRequest.h"
#include "swift/Basic/SourceManager.h"
#include "gtest/gtest.h"
#include <cmath>
#include <memory>

using namespace swift;
using namespace llvm;

class ArithmeticExpr {
 public:
  enum class Kind {
    Literal,
    Binary,
  } kind;

  // Note: used for internal caching.
  Optional<double> cachedValue;

 protected:
  ArithmeticExpr(Kind kind) : kind(kind) { }
};

class Literal : public ArithmeticExpr {
 public:
  const double value;

  Literal(double value) : ArithmeticExpr(Kind::Literal), value(value) { }
};

class Binary : public ArithmeticExpr {
 public:
  enum class OperatorKind {
    Sum,
    Product,
  } operatorKind;

  ArithmeticExpr *lhs;
  ArithmeticExpr *rhs;

  Binary(OperatorKind operatorKind, ArithmeticExpr *lhs, ArithmeticExpr *rhs)
    : ArithmeticExpr(Kind::Binary), operatorKind(operatorKind),
      lhs(lhs), rhs(rhs) { }
};

void simple_display(llvm::raw_ostream &out, ArithmeticExpr *expr) {
  switch (expr->kind) {
  case ArithmeticExpr::Kind::Literal:
    out << "Literal: " << static_cast<Literal *>(expr)->value;
    break;

  case ArithmeticExpr::Kind::Binary:
    out << "Binary: ";
    switch (static_cast<Binary *>(expr)->operatorKind) {
    case Binary::OperatorKind::Sum:
      out << "sum";
      break;

    case Binary::OperatorKind::Product:
      out << "product";
      break;
    }
    break;
  }
}

/// Rule to evaluate the value of the expression.
template<typename Derived, CacheKind Caching>
struct EvaluationRule
  : public SimpleRequest<Derived, Caching, double, ArithmeticExpr *>
{
  using SimpleRequest<Derived, Caching, double, ArithmeticExpr *>::SimpleRequest;

  double evaluate(Evaluator &evaluator, ArithmeticExpr *expr) const {
    switch (expr->kind) {
    case ArithmeticExpr::Kind::Literal:
      return static_cast<Literal *>(expr)->value;

    case ArithmeticExpr::Kind::Binary: {
      auto binary = static_cast<Binary *>(expr);

      // Evaluate the left- and right-hand sides.
      double lhsValue, rhsValue;
      std::tie(lhsValue, rhsValue) =
        evaluator(Derived{binary->lhs}, Derived{binary->rhs});
      switch (binary->operatorKind) {
      case Binary::OperatorKind::Sum:
        return lhsValue + rhsValue;

      case Binary::OperatorKind::Product:
        return lhsValue * rhsValue;
      }
    }
    }
  }

  static bool brokeCycle;
  double breakCycle() const {
    brokeCycle = true;
    return NAN;
  }

  void diagnoseCycle(DiagnosticEngine &diags) const { }
  void noteCycleStep(DiagnosticEngine &diags) const { }
};

template<typename Derived, CacheKind Caching>
bool EvaluationRule<Derived, Caching>::brokeCycle = false;

struct InternallyCachedEvaluationRule :
EvaluationRule<InternallyCachedEvaluationRule, CacheKind::Cached>
{
  using EvaluationRule::EvaluationRule;

  bool isCached() const {
    auto expr = std::get<0>(getStorage());
    switch (expr->kind) {
    case ArithmeticExpr::Kind::Literal:
      return false;

    case ArithmeticExpr::Kind::Binary:
      return true;
    }
  }
};

struct UncachedEvaluationRule
    : EvaluationRule<UncachedEvaluationRule, CacheKind::Uncached> {
  using EvaluationRule::EvaluationRule;
};

struct ExternallyCachedEvaluationRule
    : EvaluationRule<ExternallyCachedEvaluationRule,
                     CacheKind::SeparatelyCached> {
  using EvaluationRule::EvaluationRule;

  bool isCached() const {
    auto expr = std::get<0>(getStorage());

    switch (expr->kind) {
    case ArithmeticExpr::Kind::Literal:
      return false;

    case ArithmeticExpr::Kind::Binary:
      return true;
    }
  }

  Optional<double> getCachedResult() const {
    auto expr = std::get<0>(getStorage());

    return expr->cachedValue;
  }

  void cacheResult(double value) const {
    auto expr = std::get<0>(getStorage());

    expr->cachedValue = value;
  }
};

// Define the arithmetic evaluator's zone.
namespace swift {
#define SWIFT_ARITHMETIC_EVALUATOR_ZONE 255
#define SWIFT_TYPEID_ZONE SWIFT_ARITHMETIC_EVALUATOR_ZONE
#define SWIFT_TYPEID_HEADER "ArithmeticEvaluatorTypeIDZone.def"
#include "swift/Basic/DefineTypeIDZone.h"

#define SWIFT_TYPEID_ZONE SWIFT_ARITHMETIC_EVALUATOR_ZONE
#define SWIFT_TYPEID_HEADER "ArithmeticEvaluatorTypeIDZone.def"
#include "swift/Basic/ImplementTypeIDZone.h"

}

/// All of the arithmetic request functions.
static AbstractRequestFunction *arithmeticRequestFunctions[] = {
#define SWIFT_TYPEID(Name)                                    \
  reinterpret_cast<AbstractRequestFunction *>(&Name::evaluateRequest),
#include "ArithmeticEvaluatorTypeIDZone.def"
#undef SWIFT_TYPEID
};

TEST(ArithmeticEvaluator, Simple) {
  // (3.14159 + 2.71828) * 42
  ArithmeticExpr *pi = new Literal(3.14159);
  ArithmeticExpr *e = new Literal(2.71828);
  ArithmeticExpr *sum = new Binary(Binary::OperatorKind::Sum, pi, e);
  ArithmeticExpr *lifeUniverseEverything = new Literal(42.0);
  ArithmeticExpr *product = new Binary(Binary::OperatorKind::Product, sum,
                                       lifeUniverseEverything);

  SourceManager sourceMgr;
  DiagnosticEngine diags(sourceMgr);
  Evaluator evaluator(diags, CycleDiagnosticKind::FullDiagnose);
  evaluator.registerRequestFunctions(SWIFT_ARITHMETIC_EVALUATOR_ZONE,
                                     arithmeticRequestFunctions);

  const double expectedResult = (3.14159 + 2.71828) * 42.0;
  EXPECT_EQ(evaluator(InternallyCachedEvaluationRule(product)),
            expectedResult);

  // Cached response.
  EXPECT_EQ(evaluator(InternallyCachedEvaluationRule(product)),
            expectedResult);

  // Uncached
  evaluator.clearCache();
  EXPECT_EQ(evaluator(UncachedEvaluationRule(product)),
            expectedResult);
  EXPECT_EQ(evaluator(UncachedEvaluationRule(product)),
            expectedResult);

  // External cached query.
  evaluator.clearCache();
  EXPECT_EQ(evaluator(ExternallyCachedEvaluationRule(product)),
            expectedResult);
  EXPECT_EQ(*sum->cachedValue, 3.14159 + 2.71828);
  EXPECT_EQ(*product->cachedValue, expectedResult);
  EXPECT_EQ(evaluator(ExternallyCachedEvaluationRule(product)),
            expectedResult);
  EXPECT_EQ(*sum->cachedValue, 3.14159 + 2.71828);
  EXPECT_EQ(*product->cachedValue, expectedResult);

  // Dependency printing.

  // Cache some values first, so they show up in the result.
  (void)evaluator(InternallyCachedEvaluationRule(product));

  std::string productDependencies;
  {
    llvm::raw_string_ostream out(productDependencies);
    evaluator.printDependencies(InternallyCachedEvaluationRule(product), out);
  }

  EXPECT_EQ(productDependencies,
    " `--InternallyCachedEvaluationRule(Binary: product) -> 2.461145e+02\n"
    "     `--InternallyCachedEvaluationRule(Binary: sum) -> 5.859870e+00\n"
    "     |   `--InternallyCachedEvaluationRule(Literal: 3.141590e+00)\n"
    "     |   `--InternallyCachedEvaluationRule(Literal: 2.718280e+00)\n"
    "     `--InternallyCachedEvaluationRule(Literal: 4.200000e+01)\n");

  std::string sumDependencies;
  {
    llvm::raw_string_ostream out(sumDependencies);
    evaluator.printDependencies(ExternallyCachedEvaluationRule(product), out);
  }

  EXPECT_EQ(sumDependencies,
    " `--ExternallyCachedEvaluationRule(Binary: product)\n"
    "     `--ExternallyCachedEvaluationRule(Binary: sum)\n"
    "     |   `--ExternallyCachedEvaluationRule(Literal: 3.141590e+00)\n"
    "     |   `--ExternallyCachedEvaluationRule(Literal: 2.718280e+00)\n"
    "     `--ExternallyCachedEvaluationRule(Literal: 4.200000e+01)\n");

  // Graphviz printing.
  std::string allDependencies;
  {
    llvm::raw_string_ostream out(allDependencies);
    evaluator.printDependenciesGraphviz(out);
  }

  EXPECT_EQ(allDependencies,
    "digraph Dependencies {\n"
    "  request_0 -> request_1;\n"
    "  request_0 -> request_4;\n"
    "  request_1 -> request_3;\n"
    "  request_1 -> request_2;\n"
    "  request_5 -> request_6;\n"
    "  request_5 -> request_9;\n"
    "  request_6 -> request_8;\n"
    "  request_6 -> request_7;\n"
    "  request_10 -> request_11;\n"
    "  request_10 -> request_14;\n"
    "  request_11 -> request_13;\n"
    "  request_11 -> request_12;\n"
    "\n"
    "  request_0 [label=\"ExternallyCachedEvaluationRule(Binary: product)\"];\n"
    "  request_1 [label=\"ExternallyCachedEvaluationRule(Binary: sum)\"];\n"
    "  request_2 [label=\"ExternallyCachedEvaluationRule(Literal: 2.718280e+00)\"];\n"
    "  request_3 [label=\"ExternallyCachedEvaluationRule(Literal: 3.141590e+00)\"];\n"
    "  request_4 [label=\"ExternallyCachedEvaluationRule(Literal: 4.200000e+01)\"];\n"
    "  request_5 [label=\"InternallyCachedEvaluationRule(Binary: product) -> 2.461145e+02\"];\n"
    "  request_6 [label=\"InternallyCachedEvaluationRule(Binary: sum) -> 5.859870e+00\"];\n"
    "  request_7 [label=\"InternallyCachedEvaluationRule(Literal: 2.718280e+00)\"];\n"
    "  request_8 [label=\"InternallyCachedEvaluationRule(Literal: 3.141590e+00)\"];\n"
    "  request_9 [label=\"InternallyCachedEvaluationRule(Literal: 4.200000e+01)\"];\n"
    "  request_10 [label=\"UncachedEvaluationRule(Binary: product)\"];\n"
    "  request_11 [label=\"UncachedEvaluationRule(Binary: sum)\"];\n"
    "  request_12 [label=\"UncachedEvaluationRule(Literal: 2.718280e+00)\"];\n"
    "  request_13 [label=\"UncachedEvaluationRule(Literal: 3.141590e+00)\"];\n"
    "  request_14 [label=\"UncachedEvaluationRule(Literal: 4.200000e+01)\"];\n"
    "}\n");

  // Cleanup
  delete pi;
  delete e;
  delete sum;
  delete lifeUniverseEverything;
  delete product;
}

TEST(ArithmeticEvaluator, Cycle) {
  // (3.14159 + 2.71828) * 42
  ArithmeticExpr *pi = new Literal(3.14159);
  ArithmeticExpr *e = new Literal(2.71828);
  Binary *sum = new Binary(Binary::OperatorKind::Sum, pi, e);
  ArithmeticExpr *lifeUniverseEverything = new Literal(42.0);
  ArithmeticExpr *product = new Binary(Binary::OperatorKind::Product, sum,
                                       lifeUniverseEverything);

  // Introduce a cycle.
  sum->rhs = product;

  SourceManager sourceMgr;
  DiagnosticEngine diags(sourceMgr);
  Evaluator evaluator(diags, CycleDiagnosticKind::FullDiagnose);
  evaluator.registerRequestFunctions(SWIFT_ARITHMETIC_EVALUATOR_ZONE,
                                     arithmeticRequestFunctions);

  // Evaluate when there is a cycle.
  UncachedEvaluationRule::brokeCycle = false;
  EXPECT_TRUE(std::isnan(evaluator(UncachedEvaluationRule(product))));
  EXPECT_TRUE(UncachedEvaluationRule::brokeCycle);

  // Cycle-breaking result is cached.
  EXPECT_TRUE(std::isnan(evaluator(UncachedEvaluationRule(product))));
  UncachedEvaluationRule::brokeCycle = false;
  EXPECT_FALSE(UncachedEvaluationRule::brokeCycle);

  // Evaluate when there is a cycle.
  evaluator.clearCache();
  InternallyCachedEvaluationRule::brokeCycle = false;
  EXPECT_TRUE(std::isnan(evaluator(InternallyCachedEvaluationRule(product))));
  EXPECT_TRUE(InternallyCachedEvaluationRule::brokeCycle);

  // Cycle-breaking result is cached.
  InternallyCachedEvaluationRule::brokeCycle = false;
  EXPECT_TRUE(std::isnan(evaluator(InternallyCachedEvaluationRule(product))));
  EXPECT_FALSE(InternallyCachedEvaluationRule::brokeCycle);

  // Evaluate when there is a cycle.
  evaluator.clearCache();
  ExternallyCachedEvaluationRule::brokeCycle = false;
  EXPECT_TRUE(std::isnan(evaluator(ExternallyCachedEvaluationRule(product))));
  EXPECT_TRUE(ExternallyCachedEvaluationRule::brokeCycle);

  // Cycle-breaking result is cached.
  ExternallyCachedEvaluationRule::brokeCycle = false;
  EXPECT_TRUE(std::isnan(evaluator(ExternallyCachedEvaluationRule(product))));
  EXPECT_FALSE(ExternallyCachedEvaluationRule::brokeCycle);

  // Dependency printing.
  std::string productDependencies;
  {
    llvm::raw_string_ostream out(productDependencies);
    evaluator.printDependencies(InternallyCachedEvaluationRule(product), out);
  }

  EXPECT_EQ(productDependencies,
    " `--InternallyCachedEvaluationRule(Binary: product)\n"
    "     `--InternallyCachedEvaluationRule(Binary: sum)\n"
    "     |   `--InternallyCachedEvaluationRule(Literal: 3.141590e+00)\n"
    "     |   `--InternallyCachedEvaluationRule(Binary: product) "
      "(cyclic dependency)\n"
    "     `--InternallyCachedEvaluationRule(Literal: 4.200000e+01)\n");

  // Cleanup
  delete pi;
  delete e;
  delete sum;
  delete lifeUniverseEverything;
  delete product;
}
