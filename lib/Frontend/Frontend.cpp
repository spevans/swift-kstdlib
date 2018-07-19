//===--- Frontend.cpp - frontend utility methods --------------------------===//
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
// This file contains utility methods for parsing and performing semantic
// on modules.
//
//===----------------------------------------------------------------------===//

#include "swift/Frontend/Frontend.h"
#include "swift/AST/ASTContext.h"
#include "swift/AST/DiagnosticsFrontend.h"
#include "swift/AST/DiagnosticsSema.h"
#include "swift/AST/Module.h"
#include "swift/Basic/SourceManager.h"
#include "swift/Basic/Statistic.h"
#include "swift/Parse/DelayedParsingCallbacks.h"
#include "swift/Parse/Lexer.h"
#include "swift/SIL/SILModule.h"
#include "swift/Serialization/SerializedModuleLoader.h"
#include "swift/Strings.h"
#include "swift/Subsystems.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

using namespace swift;

CompilerInstance::CompilerInstance() = default;
CompilerInstance::~CompilerInstance() = default;

std::string CompilerInvocation::getPCHHash() const {
  using llvm::hash_code;
  using llvm::hash_value;
  using llvm::hash_combine;

  auto Code = hash_value(LangOpts.getPCHHashComponents());
  Code = hash_combine(Code, FrontendOpts.getPCHHashComponents());
  Code = hash_combine(Code, ClangImporterOpts.getPCHHashComponents());
  Code = hash_combine(Code, SearchPathOpts.getPCHHashComponents());
  Code = hash_combine(Code, DiagnosticOpts.getPCHHashComponents());
  Code = hash_combine(Code, SILOpts.getPCHHashComponents());
  Code = hash_combine(Code, IRGenOpts.getPCHHashComponents());

  return llvm::APInt(64, Code).toString(36, /*Signed=*/false);
}

const PrimarySpecificPaths &
CompilerInvocation::getPrimarySpecificPathsForAtMostOnePrimary() const {
  return getFrontendOptions().getPrimarySpecificPathsForAtMostOnePrimary();
}

const PrimarySpecificPaths &
CompilerInvocation::getPrimarySpecificPathsForPrimary(
    StringRef filename) const {
  return getFrontendOptions().getPrimarySpecificPathsForPrimary(filename);
}

const PrimarySpecificPaths &
CompilerInvocation::getPrimarySpecificPathsForSourceFile(
    const SourceFile &SF) const {
  return getPrimarySpecificPathsForPrimary(SF.getFilename());
}

std::string CompilerInvocation::getOutputFilenameForAtMostOnePrimary() const {
  return getPrimarySpecificPathsForAtMostOnePrimary().OutputFilename;
}
std::string
CompilerInvocation::getMainInputFilenameForDebugInfoForAtMostOnePrimary()
    const {
  return getPrimarySpecificPathsForAtMostOnePrimary()
      .MainInputFilenameForDebugInfo;
}
std::string
CompilerInvocation::getObjCHeaderOutputPathForAtMostOnePrimary() const {
  return getPrimarySpecificPathsForAtMostOnePrimary()
      .SupplementaryOutputs.ObjCHeaderOutputPath;
}
std::string CompilerInvocation::getModuleOutputPathForAtMostOnePrimary() const {
  return getPrimarySpecificPathsForAtMostOnePrimary()
      .SupplementaryOutputs.ModuleOutputPath;
}
std::string CompilerInvocation::getReferenceDependenciesFilePathForPrimary(
    StringRef filename) const {
  return getPrimarySpecificPathsForPrimary(filename)
      .SupplementaryOutputs.ReferenceDependenciesFilePath;
}
std::string
CompilerInvocation::getSerializedDiagnosticsPathForAtMostOnePrimary() const {
  return getPrimarySpecificPathsForAtMostOnePrimary()
      .SupplementaryOutputs.SerializedDiagnosticsPath;
}
std::string CompilerInvocation::getTBDPathForWholeModule() const {
  assert(getFrontendOptions().InputsAndOutputs.isWholeModule() &&
         "TBDPath only makes sense in WMO mode");
  return getPrimarySpecificPathsForAtMostOnePrimary()
      .SupplementaryOutputs.TBDPath;
}

void CompilerInstance::createSILModule() {
  assert(MainModule && "main module not created yet");
  // Assume WMO if a -primary-file option was not provided.
  TheSILModule = SILModule::createEmptyModule(
      getMainModule(), Invocation.getSILOptions(),
      Invocation.getFrontendOptions().InputsAndOutputs.isWholeModule());
}

void CompilerInstance::setSILModule(std::unique_ptr<SILModule> M) {
  TheSILModule = std::move(M);
}

void CompilerInstance::recordPrimaryInputBuffer(unsigned BufID) {
  PrimaryBufferIDs.insert(BufID);
}

void CompilerInstance::recordPrimarySourceFile(SourceFile *SF) {
  assert(MainModule && "main module not created yet");
  PrimarySourceFiles.push_back(SF);
  SF->createReferencedNameTracker();
  if (SF->getBufferID().hasValue())
    recordPrimaryInputBuffer(SF->getBufferID().getValue());
}

bool CompilerInstance::setup(const CompilerInvocation &Invok) {
  Invocation = Invok;

  setUpLLVMArguments();
  setUpDiagnosticOptions();

  // If we are asked to emit a module documentation file, configure lexing and
  // parsing to remember comments.
  if (Invocation.getFrontendOptions().InputsAndOutputs.hasModuleDocOutputPath())
    Invocation.getLangOptions().AttachCommentsToDecls = true;

  // If we are doing index-while-building, configure lexing and parsing to
  // remember comments.
  if (!Invocation.getFrontendOptions().IndexStorePath.empty()) {
    Invocation.getLangOptions().AttachCommentsToDecls = true;
  }

  Context.reset(ASTContext::get(Invocation.getLangOptions(),
                                Invocation.getSearchPathOptions(), SourceMgr,
                                Diagnostics));
  registerTypeCheckerRequestFunctions(Context->evaluator);

  if (setUpModuleLoaders())
    return true;

  assert(Lexer::isIdentifier(Invocation.getModuleName()));

  if (isInSILMode())
    Invocation.getLangOptions().EnableAccessControl = false;

  return setUpInputs();
}

void CompilerInstance::setUpLLVMArguments() {
  // Honor -Xllvm.
  if (!Invocation.getFrontendOptions().LLVMArgs.empty()) {
    llvm::SmallVector<const char *, 4> Args;
    Args.push_back("swift (LLVM option parsing)");
    for (unsigned i = 0, e = Invocation.getFrontendOptions().LLVMArgs.size();
         i != e; ++i)
      Args.push_back(Invocation.getFrontendOptions().LLVMArgs[i].c_str());
    Args.push_back(nullptr);
    llvm::cl::ParseCommandLineOptions(Args.size()-1, Args.data());
  }
}

void CompilerInstance::setUpDiagnosticOptions() {
  if (Invocation.getDiagnosticOptions().ShowDiagnosticsAfterFatalError) {
    Diagnostics.setShowDiagnosticsAfterFatalError();
  }
  if (Invocation.getDiagnosticOptions().SuppressWarnings) {
    Diagnostics.setSuppressWarnings(true);
  }
  if (Invocation.getDiagnosticOptions().WarningsAsErrors) {
    Diagnostics.setWarningsAsErrors(true);
  }
}

bool CompilerInstance::setUpModuleLoaders() {
  if (hasSourceImport()) {
    bool immediate = FrontendOptions::isActionImmediate(
        Invocation.getFrontendOptions().RequestedAction);
    bool enableResilience = Invocation.getFrontendOptions().EnableResilience;
    Context->addModuleLoader(SourceLoader::create(*Context,
                                                  !immediate,
                                                  enableResilience,
                                                  getDependencyTracker()));
  }
  {
    auto SML = SerializedModuleLoader::create(*Context, getDependencyTracker());
    this->SML = SML.get();
    Context->addModuleLoader(std::move(SML));
  }
  {
    // Wire up the Clang importer. If the user has specified an SDK, use it.
    // Otherwise, we just keep it around as our interface to Clang's ABI
    // knowledge.
    auto clangImporter =
        ClangImporter::create(*Context, Invocation.getClangImporterOptions(),
                              Invocation.getPCHHash(), getDependencyTracker());
    if (!clangImporter) {
      Diagnostics.diagnose(SourceLoc(), diag::error_clang_importer_create_fail);
      return true;
    }

    Context->addModuleLoader(std::move(clangImporter), /*isClang*/ true);
  }
  return false;
}

Optional<unsigned> CompilerInstance::setUpCodeCompletionBuffer() {
  Optional<unsigned> codeCompletionBufferID;
  auto codeCompletePoint = Invocation.getCodeCompletionPoint();
  if (codeCompletePoint.first) {
    auto memBuf = codeCompletePoint.first;
    // CompilerInvocation doesn't own the buffers, copy to a new buffer.
    codeCompletionBufferID = SourceMgr.addMemBufferCopy(memBuf);
    InputSourceCodeBufferIDs.push_back(*codeCompletionBufferID);
    SourceMgr.setCodeCompletionPoint(*codeCompletionBufferID,
                                     codeCompletePoint.second);
  }
  return codeCompletionBufferID;
}

bool CompilerInstance::setUpInputs() {
  // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  // per-input setup.
  const Optional<unsigned> codeCompletionBufferID = setUpCodeCompletionBuffer();

  for (const InputFile &input :
       Invocation.getFrontendOptions().InputsAndOutputs.getAllInputs())
    if (setUpForInput(input))
      return true;

  // Set the primary file to the code-completion point if one exists.
  if (codeCompletionBufferID.hasValue() &&
      !isPrimaryInput(*codeCompletionBufferID)) {
    assert(PrimaryBufferIDs.empty() && "re-setting PrimaryBufferID");
    recordPrimaryInputBuffer(*codeCompletionBufferID);
  }

  if (isInputSwift() && MainBufferID == NO_SUCH_BUFFER &&
      InputSourceCodeBufferIDs.size() == 1)
    MainBufferID = InputSourceCodeBufferIDs.front();

  return false;
}

bool CompilerInstance::setUpForInput(const InputFile &input) {
  bool failed = false;
  Optional<unsigned> bufferID = getRecordedBufferID(input, failed);
  if (failed)
    return true;
  if (!bufferID)
    return false;

  if (isInSILMode() ||
      (isInputSwift() &&
       llvm::sys::path::filename(input.file()) == "main.swift")) {
    assert(MainBufferID == NO_SUCH_BUFFER && "re-setting MainBufferID");
    MainBufferID = *bufferID;
  }

  if (input.isPrimary()) {
    recordPrimaryInputBuffer(*bufferID);
  }
  return false;
}

Optional<unsigned> CompilerInstance::getRecordedBufferID(const InputFile &input,
                                                         bool &failed) {
  if (!input.buffer()) {
    if (Optional<unsigned> existingBufferID =
            SourceMgr.getIDForBufferIdentifier(input.file())) {
      return existingBufferID;
    }
  }
  std::pair<std::unique_ptr<llvm::MemoryBuffer>,
            std::unique_ptr<llvm::MemoryBuffer>>
      buffers = getInputBufferAndModuleDocBufferIfPresent(input);

  if (!buffers.first) {
    failed = true;
    return None;
  }

  // FIXME: The fact that this test happens twice, for some cases,
  // suggests that setupInputs could use another round of refactoring.
  if (serialization::isSerializedAST(buffers.first->getBuffer())) {
    PartialModules.push_back(
        {std::move(buffers.first), std::move(buffers.second)});
    return None;
  }
  assert(buffers.second.get() == nullptr);
  // Transfer ownership of the MemoryBuffer to the SourceMgr.
  unsigned bufferID = SourceMgr.addNewSourceBuffer(std::move(buffers.first));

  InputSourceCodeBufferIDs.push_back(bufferID);
  return bufferID;
}

std::pair<std::unique_ptr<llvm::MemoryBuffer>,
          std::unique_ptr<llvm::MemoryBuffer>>
CompilerInstance::getInputBufferAndModuleDocBufferIfPresent(
    const InputFile &input) {
  if (auto b = input.buffer()) {
    return std::make_pair(llvm::MemoryBuffer::getMemBufferCopy(
                              b->getBuffer(), b->getBufferIdentifier()),
                          nullptr);
  }
  // FIXME: Working with filenames is fragile, maybe use the real path
  // or have some kind of FileManager.
  using FileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  FileOrError inputFileOrErr = llvm::MemoryBuffer::getFileOrSTDIN(input.file());
  if (!inputFileOrErr) {
    Diagnostics.diagnose(SourceLoc(), diag::error_open_input_file, input.file(),
                         inputFileOrErr.getError().message());
    return std::make_pair(nullptr, nullptr);
  }
  if (!serialization::isSerializedAST((*inputFileOrErr)->getBuffer()))
    return std::make_pair(std::move(*inputFileOrErr), nullptr);

  if (Optional<std::unique_ptr<llvm::MemoryBuffer>> moduleDocBuffer =
          openModuleDoc(input)) {
    return std::make_pair(std::move(*inputFileOrErr),
                          std::move(*moduleDocBuffer));
  }
  return std::make_pair(nullptr, nullptr);
}

Optional<std::unique_ptr<llvm::MemoryBuffer>>
CompilerInstance::openModuleDoc(const InputFile &input) {
  llvm::SmallString<128> moduleDocFilePath(input.file());
  llvm::sys::path::replace_extension(moduleDocFilePath,
                                     SERIALIZED_MODULE_DOC_EXTENSION);
  using FileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  FileOrError moduleDocFileOrErr =
      llvm::MemoryBuffer::getFileOrSTDIN(moduleDocFilePath);
  if (moduleDocFileOrErr)
    return std::move(*moduleDocFileOrErr);

  if (moduleDocFileOrErr.getError() == std::errc::no_such_file_or_directory)
    return std::unique_ptr<llvm::MemoryBuffer>();

  Diagnostics.diagnose(SourceLoc(), diag::error_open_input_file,
                       moduleDocFilePath,
                       moduleDocFileOrErr.getError().message());
  return None;
}

ModuleDecl *CompilerInstance::getMainModule() {
  if (!MainModule) {
    Identifier ID = Context->getIdentifier(Invocation.getModuleName());
    MainModule = ModuleDecl::create(ID, *Context);
    if (Invocation.getFrontendOptions().EnableTesting)
      MainModule->setTestingEnabled();

    if (Invocation.getFrontendOptions().EnableResilience)
      MainModule->setResilienceStrategy(ResilienceStrategy::Resilient);
  }
  return MainModule;
}

static void addAdditionalInitialImportsTo(
    SourceFile *SF, const CompilerInstance::ImplicitImports &implicitImports) {
  using ImportPair =
      std::pair<ModuleDecl::ImportedModule, SourceFile::ImportOptions>;
  SmallVector<ImportPair, 4> additionalImports;

  if (implicitImports.objCModuleUnderlyingMixedFramework)
    additionalImports.push_back(
        {{/*accessPath=*/{},
          implicitImports.objCModuleUnderlyingMixedFramework},
         SourceFile::ImportFlags::Exported});
  if (implicitImports.headerModule)
    additionalImports.push_back(
        {{/*accessPath=*/{}, implicitImports.headerModule},
         SourceFile::ImportFlags::Exported});
  if (!implicitImports.modules.empty()) {
    for (auto &importModule : implicitImports.modules) {
      additionalImports.push_back({{/*accessPath=*/{}, importModule}, {}});
    }
  }

  SF->addImports(additionalImports);
}

static bool shouldImportSwiftOnoneModuleIfNoneOrImplicitOptimization(
    FrontendOptions::ActionType RequestedAction) {
  return RequestedAction == FrontendOptions::ActionType::EmitObject ||
         RequestedAction == FrontendOptions::ActionType::Immediate ||
         RequestedAction == FrontendOptions::ActionType::EmitSIL;
}

/// Implicitly import the SwiftOnoneSupport module in non-optimized
/// builds. This allows for use of popular specialized functions
/// from the standard library, which makes the non-optimized builds
/// execute much faster.
static bool
shouldImplicityImportSwiftOnoneSupportModule(CompilerInvocation &Invocation) {
  if (Invocation.getImplicitModuleImportKind() !=
      SourceFile::ImplicitModuleImportKind::Stdlib)
    return false;
  if (Invocation.getSILOptions().shouldOptimize())
    return false;

  if (shouldImportSwiftOnoneModuleIfNoneOrImplicitOptimization(
          Invocation.getFrontendOptions().RequestedAction)) {
    return true;
  }
  return Invocation.getFrontendOptions().isCreatingSIL();
}

void CompilerInstance::performSema() {
  FrontendStatsTracer tracer(Context->Stats, "perform-sema");
  Context->LoadedModules[MainModule->getName()] = getMainModule();

  if (Invocation.getInputKind() == InputFileKind::IFK_SIL) {
    assert(!InputSourceCodeBufferIDs.empty());
    assert(InputSourceCodeBufferIDs.size() == 1);
    assert(MainBufferID != NO_SUCH_BUFFER);
    createSILModule();
  }

  if (Invocation.getImplicitModuleImportKind() ==
      SourceFile::ImplicitModuleImportKind::Stdlib) {
    if (!loadStdlib())
      return;
  }
  if (shouldImplicityImportSwiftOnoneSupportModule(Invocation)) {
    Invocation.getFrontendOptions().ImplicitImportModuleNames.push_back(
        SWIFT_ONONE_SUPPORT);
  }

  const ImplicitImports implicitImports(*this);

  if (Invocation.getInputKind() == InputFileKind::IFK_Swift_REPL) {
    createREPLFile(implicitImports);
    return;
  }

  // Make sure the main file is the first file in the module, so do this now.
  if (MainBufferID != NO_SUCH_BUFFER)
    addMainFileToModule(implicitImports);

  parseAndCheckTypes(implicitImports);
}

CompilerInstance::ImplicitImports::ImplicitImports(CompilerInstance &compiler) {
  kind = compiler.Invocation.getImplicitModuleImportKind();

  objCModuleUnderlyingMixedFramework =
      compiler.Invocation.getFrontendOptions().ImportUnderlyingModule
          ? compiler.importUnderlyingModule()
          : nullptr;

  compiler.getImplicitlyImportedModules(modules);

  headerModule = compiler.importBridgingHeader();
}

bool CompilerInstance::loadStdlib() {
  FrontendStatsTracer tracer(Context->Stats, "load-stdlib");
  ModuleDecl *M = Context->getStdlibModule(true);

  if (!M) {
    Diagnostics.diagnose(SourceLoc(), diag::error_stdlib_not_found,
                         Invocation.getTargetTriple());
    return false;
  }

  // If we failed to load, we should have already diagnosed
  if (M->failedToLoad()) {
    assert(Diagnostics.hadAnyError() &&
           "Module failed to load but nothing was diagnosed?");
    return false;
  }
  return true;
}

ModuleDecl *CompilerInstance::importUnderlyingModule() {
  FrontendStatsTracer tracer(Context->Stats, "import-underlying-module");
  ModuleDecl *objCModuleUnderlyingMixedFramework =
      static_cast<ClangImporter *>(Context->getClangModuleLoader())
          ->loadModule(SourceLoc(),
                       std::make_pair(MainModule->getName(), SourceLoc()));
  if (objCModuleUnderlyingMixedFramework)
    return objCModuleUnderlyingMixedFramework;
  Diagnostics.diagnose(SourceLoc(), diag::error_underlying_module_not_found,
                       MainModule->getName());
  return nullptr;
}

ModuleDecl *CompilerInstance::importBridgingHeader() {
  FrontendStatsTracer tracer(Context->Stats, "import-bridging-header");
  const StringRef implicitHeaderPath =
      Invocation.getFrontendOptions().ImplicitObjCHeaderPath;
  auto clangImporter =
      static_cast<ClangImporter *>(Context->getClangModuleLoader());
  if (implicitHeaderPath.empty() ||
      clangImporter->importBridgingHeader(implicitHeaderPath, MainModule))
    return nullptr;
  ModuleDecl *importedHeaderModule = clangImporter->getImportedHeaderModule();
  assert(importedHeaderModule);
  return importedHeaderModule;
}

void CompilerInstance::getImplicitlyImportedModules(
    SmallVectorImpl<ModuleDecl *> &importModules) {
  FrontendStatsTracer tracer(Context->Stats, "get-implicitly-imported-modules");
  for (auto &ImplicitImportModuleName :
       Invocation.getFrontendOptions().ImplicitImportModuleNames) {
    if (Lexer::isIdentifier(ImplicitImportModuleName)) {
      auto moduleID = Context->getIdentifier(ImplicitImportModuleName);
      ModuleDecl *importModule =
          Context->getModule(std::make_pair(moduleID, SourceLoc()));
      if (importModule) {
        importModules.push_back(importModule);
      } else {
        Diagnostics.diagnose(SourceLoc(), diag::sema_no_import,
                             ImplicitImportModuleName);
        if (Invocation.getSearchPathOptions().SDKPath.empty() &&
            llvm::Triple(llvm::sys::getProcessTriple()).isMacOSX()) {
          Diagnostics.diagnose(SourceLoc(), diag::sema_no_import_no_sdk);
          Diagnostics.diagnose(SourceLoc(), diag::sema_no_import_no_sdk_xcrun);
        }
      }
    } else {
      Diagnostics.diagnose(SourceLoc(), diag::error_bad_module_name,
                           ImplicitImportModuleName, false);
    }
  }
}

void CompilerInstance::createREPLFile(const ImplicitImports &implicitImports) {
  auto *SingleInputFile = createSourceFileForMainModule(
      Invocation.getSourceFileKind(), implicitImports.kind, None);
  addAdditionalInitialImportsTo(SingleInputFile, implicitImports);
}

std::unique_ptr<DelayedParsingCallbacks>
CompilerInstance::computeDelayedParsingCallback(bool isPrimary) {
  if (Invocation.isCodeCompletion())
    return llvm::make_unique<CodeCompleteDelayedCallbacks>(
        SourceMgr.getCodeCompletionLoc());
  if (!isPrimary)
    return llvm::make_unique<AlwaysDelayedCallbacks>();
  return nullptr;
}

void CompilerInstance::addMainFileToModule(
    const ImplicitImports &implicitImports) {
  const InputFileKind Kind = Invocation.getInputKind();
  assert(Kind == InputFileKind::IFK_Swift || Kind == InputFileKind::IFK_SIL);

  if (Kind == InputFileKind::IFK_Swift)
    SourceMgr.setHashbangBufferID(MainBufferID);

  auto *MainFile = createSourceFileForMainModule(
      Invocation.getSourceFileKind(), implicitImports.kind, MainBufferID);
  addAdditionalInitialImportsTo(MainFile, implicitImports);
}

void CompilerInstance::parseAndCheckTypes(
    const ImplicitImports &implicitImports) {
  FrontendStatsTracer tracer(Context->Stats, "parse-and-check-types");
  // Delayed parsing callback for the primary file, or all files
  // in non-WMO mode.
  std::unique_ptr<DelayedParsingCallbacks> PrimaryDelayedCB{
      computeDelayedParsingCallback(true)};

  // Delayed parsing callback for non-primary files. Not used in
  // WMO mode.
  std::unique_ptr<DelayedParsingCallbacks> SecondaryDelayedCB{
      computeDelayedParsingCallback(false)};

  PersistentParserState PersistentState;

  bool hadLoadError = parsePartialModulesAndLibraryFiles(
      implicitImports, PersistentState,
      PrimaryDelayedCB.get(),
      SecondaryDelayedCB.get());
  if (Invocation.isCodeCompletion()) {
    // When we are doing code completion, make sure to emit at least one
    // diagnostic, so that ASTContext is marked as erroneous.  In this case
    // various parts of the compiler (for example, AST verifier) have less
    // strict assumptions about the AST.
    Diagnostics.diagnose(SourceLoc(), diag::error_doing_code_completion);
  }
  if (hadLoadError)
    return;

  OptionSet<TypeCheckingFlags> TypeCheckOptions = computeTypeCheckingOptions();

  // Type-check main file after parsing all other files so that
  // it can use declarations from other files.
  // In addition, the main file has parsing and type-checking
  // interwined.
  if (MainBufferID != NO_SUCH_BUFFER) {
    parseAndTypeCheckMainFile(PersistentState, PrimaryDelayedCB.get(),
                              TypeCheckOptions);
  }

  assert(llvm::all_of(MainModule->getFiles(), [](const FileUnit *File) -> bool {
    auto *SF = dyn_cast<SourceFile>(File);
    if (!SF)
      return true;
    return SF->ASTStage >= SourceFile::NameBound;
  }) && "some files have not yet had their imports resolved");
  MainModule->setHasResolvedImports();

  const auto &options = Invocation.getFrontendOptions();
  forEachFileToTypeCheck([&](SourceFile &SF) {
    performTypeChecking(SF, PersistentState.getTopLevelContext(),
                        TypeCheckOptions, /*curElem*/ 0,
                        options.WarnLongFunctionBodies,
                        options.WarnLongExpressionTypeChecking,
                        options.SolverExpressionTimeThreshold,
                        options.SwitchCheckingInvocationThreshold);
  });

  // Even if there were no source files, we should still record known
  // protocols.
  if (auto *stdlib = Context->getStdlibModule())
    Context->recordKnownProtocols(stdlib);

  if (Invocation.isCodeCompletion()) {
    performDelayedParsing(MainModule, PersistentState,
                          Invocation.getCodeCompletionFactory());
  }
  finishTypeChecking(TypeCheckOptions);
}

void CompilerInstance::parseLibraryFile(
    unsigned BufferID, const ImplicitImports &implicitImports,
    PersistentParserState &PersistentState,
    DelayedParsingCallbacks *PrimaryDelayedCB,
    DelayedParsingCallbacks *SecondaryDelayedCB) {
  FrontendStatsTracer tracer(Context->Stats, "parse-library-file");

  auto *NextInput = createSourceFileForMainModule(
      SourceFileKind::Library, implicitImports.kind, BufferID);
  addAdditionalInitialImportsTo(NextInput, implicitImports);

  auto IsPrimary = isWholeModuleCompilation() || isPrimaryInput(BufferID);
  auto *DelayedCB = IsPrimary ? PrimaryDelayedCB : SecondaryDelayedCB;

  auto &Diags = NextInput->getASTContext().Diags;
  auto DidSuppressWarnings = Diags.getSuppressWarnings();
  Diags.setSuppressWarnings(DidSuppressWarnings || !IsPrimary);

  bool Done;
  do {
    // Parser may stop at some erroneous constructions like #else, #endif
    // or '}' in some cases, continue parsing until we are done
    parseIntoSourceFile(*NextInput, BufferID, &Done, nullptr, &PersistentState,
                        DelayedCB);
  } while (!Done);

  Diags.setSuppressWarnings(DidSuppressWarnings);

  performNameBinding(*NextInput);
}

OptionSet<TypeCheckingFlags> CompilerInstance::computeTypeCheckingOptions() {
  OptionSet<TypeCheckingFlags> TypeCheckOptions;
  if (isWholeModuleCompilation()) {
    TypeCheckOptions |= TypeCheckingFlags::DelayWholeModuleChecking;
  }
  const auto &options = Invocation.getFrontendOptions();
  if (options.DebugTimeFunctionBodies) {
    TypeCheckOptions |= TypeCheckingFlags::DebugTimeFunctionBodies;
  }
  if (FrontendOptions::isActionImmediate(options.RequestedAction)) {
    TypeCheckOptions |= TypeCheckingFlags::ForImmediateMode;
  }
  if (options.DebugTimeExpressionTypeChecking) {
    TypeCheckOptions |= TypeCheckingFlags::DebugTimeExpressions;
  }
  return TypeCheckOptions;
}

bool CompilerInstance::parsePartialModulesAndLibraryFiles(
    const ImplicitImports &implicitImports,
    PersistentParserState &PersistentState,
    DelayedParsingCallbacks *PrimaryDelayedCB,
    DelayedParsingCallbacks *SecondaryDelayedCB) {
  FrontendStatsTracer tracer(Context->Stats,
                             "parse-partial-modules-and-library-files");
  bool hadLoadError = false;
  // Parse all the partial modules first.
  for (auto &PM : PartialModules) {
    assert(PM.ModuleBuffer);
    if (!SML->loadAST(*MainModule, SourceLoc(), std::move(PM.ModuleBuffer),
                      std::move(PM.ModuleDocBuffer)))
      hadLoadError = true;
  }

  // Then parse all the library files.
  for (auto BufferID : InputSourceCodeBufferIDs) {
    if (BufferID != MainBufferID) {
      parseLibraryFile(BufferID, implicitImports, PersistentState,
                       PrimaryDelayedCB, SecondaryDelayedCB);
    }
  }
  return hadLoadError;
}

void CompilerInstance::parseAndTypeCheckMainFile(
    PersistentParserState &PersistentState,
    DelayedParsingCallbacks *DelayedParseCB,
    OptionSet<TypeCheckingFlags> TypeCheckOptions) {
  FrontendStatsTracer tracer(Context->Stats,
                             "parse-and-typecheck-main-file");
  bool mainIsPrimary =
      (isWholeModuleCompilation() || isPrimaryInput(MainBufferID));

  SourceFile &MainFile =
      MainModule->getMainSourceFile(Invocation.getSourceFileKind());

  auto &Diags = MainFile.getASTContext().Diags;
  auto DidSuppressWarnings = Diags.getSuppressWarnings();
  Diags.setSuppressWarnings(DidSuppressWarnings || !mainIsPrimary);

  SILParserState SILContext(TheSILModule.get());
  unsigned CurTUElem = 0;
  bool Done;
  do {
    // Pump the parser multiple times if necessary.  It will return early
    // after parsing any top level code in a main module, or in SIL mode when
    // there are chunks of swift decls (e.g. imports and types) interspersed
    // with 'sil' definitions.
    parseIntoSourceFile(MainFile, MainFile.getBufferID().getValue(), &Done,
                        TheSILModule ? &SILContext : nullptr, &PersistentState,
                        DelayedParseCB);
    if (mainIsPrimary) {
      const auto &options = Invocation.getFrontendOptions();
      performTypeChecking(MainFile, PersistentState.getTopLevelContext(),
                          TypeCheckOptions, CurTUElem,
                          options.WarnLongFunctionBodies,
                          options.WarnLongExpressionTypeChecking,
                          options.SolverExpressionTimeThreshold,
                          options.SwitchCheckingInvocationThreshold);
    }
    CurTUElem = MainFile.Decls.size();
  } while (!Done);

  Diags.setSuppressWarnings(DidSuppressWarnings);

  if (mainIsPrimary && !Context->hadError() &&
      Invocation.getFrontendOptions().DebuggerTestingTransform) {
    performDebuggerTestingTransform(MainFile);
  }

  if (mainIsPrimary && !Context->hadError() &&
      Invocation.getFrontendOptions().PCMacro) {
    performPCMacro(MainFile, PersistentState.getTopLevelContext());
  }

  // Playground transform knows to look out for PCMacro's changes and not
  // to playground log them.
  if (mainIsPrimary && !Context->hadError() &&
      Invocation.getFrontendOptions().PlaygroundTransform)
    performPlaygroundTransform(
        MainFile, Invocation.getFrontendOptions().PlaygroundHighPerformance);
  if (!mainIsPrimary) {
    performNameBinding(MainFile);
  }
}

static void
forEachSourceFileIn(ModuleDecl *module,
                    llvm::function_ref<void(SourceFile &)> fn) {
  for (auto fileName : module->getFiles()) {
    if (auto SF = dyn_cast<SourceFile>(fileName))
      fn(*SF);
  }
}

void CompilerInstance::forEachFileToTypeCheck(
    llvm::function_ref<void(SourceFile &)> fn) {
  if (isWholeModuleCompilation()) {
    forEachSourceFileIn(MainModule, [&](SourceFile &SF) { fn(SF); });
  } else {
    for (auto *SF : PrimarySourceFiles) {
      fn(*SF);
    }
  }
}

void CompilerInstance::finishTypeChecking(
    OptionSet<TypeCheckingFlags> TypeCheckOptions) {
  if (TypeCheckOptions & TypeCheckingFlags::DelayWholeModuleChecking) {
    forEachSourceFileIn(MainModule, [&](SourceFile &SF) {
      performWholeModuleTypeChecking(SF);
    });
  }
}

SourceFile *CompilerInstance::createSourceFileForMainModule(
    SourceFileKind fileKind, SourceFile::ImplicitModuleImportKind importKind,
    Optional<unsigned> bufferID) {
  ModuleDecl *mainModule = getMainModule();
  SourceFile *inputFile = new (*Context)
      SourceFile(*mainModule, fileKind, bufferID, importKind,
                 Invocation.getLangOptions().CollectParsedToken,
                 Invocation.getLangOptions().BuildSyntaxTree);
  MainModule->addFile(*inputFile);

  if (bufferID && isPrimaryInput(*bufferID)) {
    recordPrimarySourceFile(inputFile);
  }

  return inputFile;
}

void CompilerInstance::performParseOnly(bool EvaluateConditionals) {
  const InputFileKind Kind = Invocation.getInputKind();
  ModuleDecl *const MainModule = getMainModule();
  Context->LoadedModules[MainModule->getName()] = MainModule;

  assert((Kind == InputFileKind::IFK_Swift ||
          Kind == InputFileKind::IFK_Swift_Library) &&
         "only supports parsing .swift files");
  (void)Kind;

  // Make sure the main file is the first file in the module but parse it last,
  // to match the parsing logic used when performing Sema.
  if (MainBufferID != NO_SUCH_BUFFER) {
    assert(Kind == InputFileKind::IFK_Swift);
    SourceMgr.setHashbangBufferID(MainBufferID);
    createSourceFileForMainModule(Invocation.getSourceFileKind(),
                                  SourceFile::ImplicitModuleImportKind::None,
                                  MainBufferID);
  }

  PersistentParserState PersistentState;
  PersistentState.PerformConditionEvaluation = EvaluateConditionals;
  // Parse all the library files.
  for (auto BufferID : InputSourceCodeBufferIDs) {
    if (BufferID == MainBufferID)
      continue;

    SourceFile *NextInput = createSourceFileForMainModule(
        SourceFileKind::Library, SourceFile::ImplicitModuleImportKind::None,
        BufferID);

    bool Done;
    do {
      // Parser may stop at some erroneous constructions like #else, #endif
      // or '}' in some cases, continue parsing until we are done
      parseIntoSourceFile(*NextInput, BufferID, &Done, nullptr,
                          &PersistentState, nullptr);
    } while (!Done);
  }

  // Now parse the main file.
  if (MainBufferID != NO_SUCH_BUFFER) {
    SourceFile &MainFile =
        MainModule->getMainSourceFile(Invocation.getSourceFileKind());
    MainFile.SyntaxParsingCache = Invocation.getMainFileSyntaxParsingCache();

    bool Done;
    do {
      parseIntoSourceFile(MainFile, MainFile.getBufferID().getValue(), &Done,
                          nullptr, &PersistentState, nullptr);
    } while (!Done);
  }

  assert(Context->LoadedModules.size() == 1 &&
         "Loaded a module during parse-only");
}

void CompilerInstance::freeASTContext() {
  Context.reset();
  MainModule = nullptr;
  SML = nullptr;
  PrimaryBufferIDs.clear();
  PrimarySourceFiles.clear();
}

void CompilerInstance::freeSILModule() { TheSILModule.reset(); }

const PrimarySpecificPaths &
CompilerInstance::getPrimarySpecificPathsForWholeModuleOptimizationMode()
    const {
  return getPrimarySpecificPathsForAtMostOnePrimary();
}
const PrimarySpecificPaths &
CompilerInstance::getPrimarySpecificPathsForAtMostOnePrimary() const {
  return Invocation.getPrimarySpecificPathsForAtMostOnePrimary();
}
const PrimarySpecificPaths &
CompilerInstance::getPrimarySpecificPathsForPrimary(StringRef filename) const {
  return Invocation.getPrimarySpecificPathsForPrimary(filename);
}
const PrimarySpecificPaths &
CompilerInstance::getPrimarySpecificPathsForSourceFile(
    const SourceFile &SF) const {
  return Invocation.getPrimarySpecificPathsForSourceFile(SF);
}
