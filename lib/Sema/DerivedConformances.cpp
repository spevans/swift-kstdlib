//===--- DerivedConformances.cpp - Derived conformance utilities ----------===//
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

#include "TypeChecker.h"
#include "swift/AST/Decl.h"
#include "swift/AST/Stmt.h"
#include "swift/AST/Expr.h"
#include "swift/AST/Pattern.h"
#include "swift/AST/ParameterList.h"
#include "swift/AST/ProtocolConformance.h"
#include "swift/AST/Types.h"
#include "swift/ClangImporter/ClangModule.h"
#include "DerivedConformances.h"

using namespace swift;

DerivedConformance::DerivedConformance(TypeChecker &tc, Decl *conformanceDecl,
                                       NominalTypeDecl *nominal,
                                       ProtocolDecl *protocol)
    : TC(tc), ConformanceDecl(conformanceDecl), Nominal(nominal),
      Protocol(protocol) {
  assert(getConformanceContext()
             ->getAsNominalTypeOrNominalTypeExtensionContext() == nominal);
}

DeclContext *DerivedConformance::getConformanceContext() const {
  return cast<DeclContext>(ConformanceDecl);
}

void DerivedConformance::addMembersToConformanceContext(
    ArrayRef<Decl *> children) {
  auto IDC = cast<IterableDeclContext>(ConformanceDecl);
  for (auto child : children) {
    IDC->addMember(child);
  }
}

Type DerivedConformance::getProtocolType() const {
  return Protocol->getDeclaredType();
}

bool DerivedConformance::derivesProtocolConformance(TypeChecker &TC,
                                                    DeclContext *DC,
                                                    NominalTypeDecl *Nominal,
                                                    ProtocolDecl *Protocol) {
  // Only known protocols can be derived.
  auto knownProtocol = Protocol->getKnownProtocolKind();
  if (!knownProtocol)
    return false;

  if (*knownProtocol == KnownProtocolKind::Hashable) {
    // We can always complete a partial Hashable implementation, and we can
    // synthesize a full Hashable implementation for structs and enums with
    // Hashable components.
    return canDeriveHashable(TC, Nominal);
  }

  if (auto *enumDecl = dyn_cast<EnumDecl>(Nominal)) {
    switch (*knownProtocol) {
        // The presence of a raw type is an explicit declaration that
        // the compiler should derive a RawRepresentable conformance.
      case KnownProtocolKind::RawRepresentable:
        return enumDecl->hasRawType();

        // Enums without associated values can implicitly derive Equatable
        // conformance.
      case KnownProtocolKind::Equatable:
        return canDeriveEquatable(TC, DC, Nominal);

        // "Simple" enums without availability attributes can explicitly derive
        // a CaseIterable conformance.
        //
        // FIXME: Lift the availability restriction.
      case KnownProtocolKind::CaseIterable:
        return !enumDecl->hasPotentiallyUnavailableCaseValue()
            && enumDecl->hasOnlyCasesWithoutAssociatedValues();

        // @objc enums can explicitly derive their _BridgedNSError conformance.
      case KnownProtocolKind::BridgedNSError:
        return enumDecl->isObjC() && enumDecl->hasCases()
            && enumDecl->hasOnlyCasesWithoutAssociatedValues();

        // Enums without associated values and enums with a raw type of String
        // or Int can explicitly derive CodingKey conformance.
      case KnownProtocolKind::CodingKey: {
        Type rawType = enumDecl->getRawType();
        if (rawType) {
          auto parentDC = enumDecl->getDeclContext();
          ASTContext &C = parentDC->getASTContext();

          auto nominal = rawType->getAnyNominal();
          return nominal == C.getStringDecl() || nominal == C.getIntDecl();
        }

        // hasOnlyCasesWithoutAssociatedValues will return true for empty enums;
        // empty enumas are allowed to conform as well.
        return enumDecl->hasOnlyCasesWithoutAssociatedValues();
      }

      default:
        return false;
    }
  } else if (isa<StructDecl>(Nominal) || isa<ClassDecl>(Nominal)) {
    // Structs and classes can explicitly derive Encodable and Decodable
    // conformance (explicitly meaning we can synthesize an implementation if
    // a type conforms manually).
    if (*knownProtocol == KnownProtocolKind::Encodable ||
        *knownProtocol == KnownProtocolKind::Decodable) {
      // FIXME: This is not actually correct. We cannot promise to always
      // provide a witness here for all structs and classes. Unfortunately,
      // figuring out whether this is actually possible requires much more
      // context -- a TypeChecker and the parent decl context at least -- and is
      // tightly coupled to the logic within DerivedConformance.
      // This unfortunately means that we expect a witness even if one will not
      // be produced, which requires DerivedConformance::deriveCodable to output
      // its own diagnostics.
      return true;
    }

    // Structs can explicitly derive Equatable conformance.
    if (auto structDecl = dyn_cast<StructDecl>(Nominal)) {
      switch (*knownProtocol) {
        case KnownProtocolKind::Equatable:
          return canDeriveEquatable(TC, DC, Nominal);
        default:
          return false;
      }
    }
  }
  return false;
}

ValueDecl *DerivedConformance::getDerivableRequirement(TypeChecker &tc,
                                                       NominalTypeDecl *nominal,
                                                       ValueDecl *requirement) {
  // Note: whenever you update this function, also update
  // TypeChecker::deriveProtocolRequirement.
  ASTContext &ctx = nominal->getASTContext();
  auto name = requirement->getFullName();

  // Local function that retrieves the requirement with the same name as
  // the provided requirement, but within the given known protocol.
  auto getRequirement = [&](KnownProtocolKind kind) -> ValueDecl * {
    // Dig out the protocol.
    auto proto = ctx.getProtocol(kind);
    if (!proto) return nullptr;

    if (auto conformance = tc.conformsToProtocol(
            nominal->getDeclaredInterfaceType(), proto, nominal,
            ConformanceCheckFlags::SkipConditionalRequirements)) {
      auto DC = conformance->getConcrete()->getDeclContext();
      // Check whether this nominal type derives conformances to the protocol.
      if (!DerivedConformance::derivesProtocolConformance(tc, DC, nominal,
                                                          proto))
        return nullptr;
    }

    // Retrieve the requirement.
    auto results = proto->lookupDirect(name);
    return results.empty() ? nullptr : results.front();
  };

  // Properties.
  if (isa<VarDecl>(requirement)) {
    // RawRepresentable.rawValue
    if (name.isSimpleName(ctx.Id_rawValue))
      return getRequirement(KnownProtocolKind::RawRepresentable);

    // Hashable.hashValue
    if (name.isSimpleName(ctx.Id_hashValue))
      return getRequirement(KnownProtocolKind::Hashable);

    // CaseIterable.allValues
    if (name.isSimpleName(ctx.Id_allCases))
      return getRequirement(KnownProtocolKind::CaseIterable);

    // _BridgedNSError._nsErrorDomain
    if (name.isSimpleName(ctx.Id_nsErrorDomain))
      return getRequirement(KnownProtocolKind::BridgedNSError);

    // CodingKey.stringValue
    if (name.isSimpleName(ctx.Id_stringValue))
      return getRequirement(KnownProtocolKind::CodingKey);

    // CodingKey.intValue
    if (name.isSimpleName(ctx.Id_intValue))
      return getRequirement(KnownProtocolKind::CodingKey);

    return nullptr;
  }

  // Functions.
  if (auto func = dyn_cast<FuncDecl>(requirement)) {
    if (func->isOperator() && name.getBaseName() == "==")
      return getRequirement(KnownProtocolKind::Equatable);

    // Encodable.encode(to: Encoder)
    if (name.isCompoundName() && name.getBaseName() == ctx.Id_encode) {
      auto argumentNames = name.getArgumentNames();
      if (argumentNames.size() == 1 && argumentNames[0] == ctx.Id_to)
        return getRequirement(KnownProtocolKind::Encodable);
    }

    // Hashable.hash(into: inout Hasher)
    if (name.isCompoundName() && name.getBaseName() == ctx.Id_hash) {
      auto argumentNames = name.getArgumentNames();
      if (argumentNames.size() == 1 && argumentNames[0] == ctx.Id_into)
        return getRequirement(KnownProtocolKind::Hashable);
    }

    return nullptr;
  }

  // Initializers.
  if (auto ctor = dyn_cast<ConstructorDecl>(requirement)) {
    auto argumentNames = name.getArgumentNames();
    if (argumentNames.size() == 1) {
      if (argumentNames[0] == ctx.Id_rawValue)
        return getRequirement(KnownProtocolKind::RawRepresentable);

      // CodingKey.init?(stringValue:), CodingKey.init?(intValue:)
      if (ctor->getFailability() == OTK_Optional &&
          (argumentNames[0] == ctx.Id_stringValue ||
           argumentNames[0] == ctx.Id_intValue))
        return getRequirement(KnownProtocolKind::CodingKey);

      // Decodable.init(from: Decoder)
      if (argumentNames[0] == ctx.Id_from)
        return getRequirement(KnownProtocolKind::Decodable);
    }

    return nullptr;
  }

  // Associated types.
  if (isa<AssociatedTypeDecl>(requirement)) {
    // RawRepresentable.RawValue
    if (name.isSimpleName(ctx.Id_RawValue))
      return getRequirement(KnownProtocolKind::RawRepresentable);

    // CaseIterable.AllCases
    if (name.isSimpleName(ctx.Id_AllCases))
      return getRequirement(KnownProtocolKind::CaseIterable);

    return nullptr;
  }

  return nullptr;
}

DeclRefExpr *
DerivedConformance::createSelfDeclRef(AbstractFunctionDecl *fn) {
  ASTContext &C = fn->getASTContext();

  auto selfDecl = fn->getImplicitSelfDecl();
  return new (C) DeclRefExpr(selfDecl, DeclNameLoc(), /*implicit*/true);
}

AccessorDecl *DerivedConformance::
addGetterToReadOnlyDerivedProperty(TypeChecker &tc,
                                   VarDecl *property,
                                   Type propertyContextType) {
  auto getter =
    declareDerivedPropertyGetter(tc, property, propertyContextType);

  property->setAccessors(StorageImplInfo::getImmutableComputed(),
                         SourceLoc(), {getter}, SourceLoc());

  return getter;
}

AccessorDecl *
DerivedConformance::declareDerivedPropertyGetter(TypeChecker &tc,
                                                 VarDecl *property,
                                                 Type propertyContextType) {
  bool isStatic = property->isStatic();
  bool isFinal = property->isFinal();

  auto &C = tc.Context;
  auto parentDC = property->getDeclContext();
  auto selfDecl = ParamDecl::createSelf(SourceLoc(), parentDC, isStatic);
  ParameterList *params[] = {
    ParameterList::createWithoutLoc(selfDecl),
    ParameterList::createEmpty(C)
  };

  Type propertyInterfaceType = property->getInterfaceType();
  
  auto getterDecl = AccessorDecl::create(C,
    /*FuncLoc=*/SourceLoc(), /*AccessorKeywordLoc=*/SourceLoc(),
    AccessorKind::Get, AddressorKind::NotAddressor, property,
    /*StaticLoc=*/SourceLoc(), StaticSpellingKind::None,
    /*Throws=*/false, /*ThrowsLoc=*/SourceLoc(),
    /*GenericParams=*/nullptr, params,
    TypeLoc::withoutLoc(propertyInterfaceType), parentDC);
  getterDecl->setImplicit();
  getterDecl->setStatic(isStatic);

  // If this is supposed to be a final method, mark it as such.
  assert(isFinal || !parentDC->getAsClassOrClassExtensionContext());
  if (isFinal && parentDC->getAsClassOrClassExtensionContext() &&
      !getterDecl->isFinal())
    getterDecl->getAttrs().add(new (C) FinalAttr(/*IsImplicit=*/true));

  // Compute the interface type of the getter.
  Type interfaceType = FunctionType::get(TupleType::getEmpty(C),
                                         propertyInterfaceType);
  auto selfParam = computeSelfParam(getterDecl);
  if (auto sig = parentDC->getGenericSignatureOfContext()) {
    getterDecl->setGenericEnvironment(
        parentDC->getGenericEnvironmentOfContext());
    interfaceType = GenericFunctionType::get(sig, {selfParam},
                                             interfaceType,
                                             FunctionType::ExtInfo());
  } else
    interfaceType = FunctionType::get({selfParam}, interfaceType,
                                      FunctionType::ExtInfo());
  getterDecl->setInterfaceType(interfaceType);
  getterDecl->copyFormalAccessFrom(property);
  getterDecl->setValidationStarted();

  tc.Context.addSynthesizedDecl(getterDecl);

  return getterDecl;
}

std::pair<VarDecl *, PatternBindingDecl *>
DerivedConformance::declareDerivedProperty(Identifier name,
                                           Type propertyInterfaceType,
                                           Type propertyContextType,
                                           bool isStatic, bool isFinal) {
  auto &C = TC.Context;
  auto parentDC = getConformanceContext();

  VarDecl *propDecl = new (C) VarDecl(/*IsStatic*/isStatic, VarDecl::Specifier::Var,
                                      /*IsCaptureList*/false, SourceLoc(), name,
                                      propertyContextType, parentDC);
  propDecl->setImplicit();
  propDecl->copyFormalAccessFrom(Nominal, /*sourceIsParentContext*/ true);
  propDecl->setInterfaceType(propertyInterfaceType);
  propDecl->setValidationStarted();

  // If this is supposed to be a final property, mark it as such.
  assert(isFinal || !parentDC->getAsClassOrClassExtensionContext());
  if (isFinal && parentDC->getAsClassOrClassExtensionContext() &&
      !propDecl->isFinal())
    propDecl->getAttrs().add(new (C) FinalAttr(/*IsImplicit=*/true));

  Pattern *propPat = new (C) NamedPattern(propDecl, /*implicit*/ true);
  propPat->setType(propertyContextType);

  propPat = new (C) TypedPattern(propPat,
                                 TypeLoc::withoutLoc(propertyContextType),
                                 /*implicit*/ true);
  propPat->setType(propertyContextType);

  auto pbDecl = PatternBindingDecl::create(C, SourceLoc(),
                                           StaticSpellingKind::None,
                                           SourceLoc(), propPat, nullptr,
                                           parentDC);
  pbDecl->setImplicit();

  return {propDecl, pbDecl};
}

bool DerivedConformance::checkAndDiagnoseDisallowedContext(
    ValueDecl *synthesizing) const {
  // In general, conformances can't be synthesized in extensions across files;
  // but we have to allow it as a special case for Equatable and Hashable on
  // enums with no associated values to preserve source compatibility.
  bool allowCrossfileExtensions = false;
  if (Protocol->isSpecificProtocol(KnownProtocolKind::Equatable) ||
      Protocol->isSpecificProtocol(KnownProtocolKind::Hashable)) {
    auto ED = dyn_cast<EnumDecl>(Nominal);
    allowCrossfileExtensions = ED && ED->hasOnlyCasesWithoutAssociatedValues();
  }

  if (TC.Context.isSwiftVersion3()) {
    // In Swift 3, a 'private' property can't be accessed in any extensions, so
    // we can't synthesize anything that uses them. Thus, we stick to the old
    // rule for synthesis, which is never in an extension except for the
    // Equatable/Hashable cases mentioned above.
    if (!allowCrossfileExtensions && Nominal != ConformanceDecl) {
      TC.diagnose(ConformanceDecl->getLoc(),
                  diag::swift3_cannot_synthesize_in_extension,
                  getProtocolType());
      return true;
    }
  }

  if (!allowCrossfileExtensions &&
      Nominal->getModuleScopeContext() !=
          getConformanceContext()->getModuleScopeContext()) {
    TC.diagnose(ConformanceDecl->getLoc(),
                diag::cannot_synthesize_in_crossfile_extension,
                getProtocolType());
    TC.diagnose(Nominal->getLoc(), diag::kind_declared_here,
                DescriptiveDeclKind::Type);
    return true;
  }

  // A non-final class can't have an protocol-witnesss initializer in an
  // extension.
  if (auto CD = dyn_cast<ClassDecl>(Nominal)) {
    if (!CD->isFinal() && isa<ConstructorDecl>(synthesizing) &&
        isa<ExtensionDecl>(ConformanceDecl)) {
      TC.diagnose(ConformanceDecl->getLoc(),
                  diag::cannot_synthesize_init_in_extension_of_nonfinal,
                  getProtocolType(), synthesizing->getFullName());
      return true;
    }
  }

  return false;
}
