//===--- RawSyntax.h - Swift Raw Syntax Interface ---------------*- C++ -*-===//
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
// This file defines the RawSyntax type.
//
// These are the "backbone or "skeleton" of the Syntax tree, providing
// the recursive structure, child relationships, kind of node, etc.
//
// They are reference-counted and strictly immutable, so can be shared freely
// among Syntax nodes and have no specific identity. They could even in theory
// be shared for expressions like 1 + 1 + 1 + 1 - you don't need 7 syntax nodes
// to express that at this layer.
//
// These are internal implementation ONLY - do not expose anything involving
// RawSyntax publicly. Clients of lib/Syntax should not be aware that they
// exist.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_SYNTAX_RAWSYNTAX_H
#define SWIFT_SYNTAX_RAWSYNTAX_H

#include "swift/Basic/InlineBitfield.h"
#include "swift/Syntax/References.h"
#include "swift/Syntax/SyntaxKind.h"
#include "swift/Syntax/TokenKinds.h"
#include "swift/Syntax/Trivia.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>

using llvm::cast;
using llvm::StringRef;

#ifndef NDEBUG
#define syntax_assert_child_kind(Raw, Cursor, ExpectedKind)                    \
  do {                                                                         \
    if (auto &__Child = Raw->getChild(Cursor))                                 \
      assert(__Child->getKind() == ExpectedKind);                              \
  } while (false)
#else
#define syntax_assert_child_kind(Raw, Cursor, ExpectedKind)
#endif

#ifndef NDEBUG
#define syntax_assert_child_token(Raw, CursorName, ...)                        \
  do {                                                                         \
    bool __Found = false;                                                      \
    if (auto &__Token = Raw->getChild(Cursor::CursorName)) {                   \
      assert(__Token->isToken());                                              \
      if (__Token->isPresent()) {                                              \
        for (auto Token : {__VA_ARGS__}) {                                     \
          if (__Token->getTokenKind() == Token) {                              \
            __Found = true;                                                    \
            break;                                                             \
          }                                                                    \
        }                                                                      \
        assert(__Found && "invalid token supplied for " #CursorName            \
                          ", expected one of {" #__VA_ARGS__ "}");             \
      }                                                                        \
    }                                                                          \
  } while (false)
#else
#define syntax_assert_child_token(Raw, CursorName, ...)
#endif

#ifndef NDEBUG
#define syntax_assert_child_token_text(Raw, CursorName, TokenKind, ...)        \
  do {                                                                         \
    bool __Found = false;                                                      \
    if (auto &__Child = Raw->getChild(Cursor::CursorName)) {                   \
      assert(__Child->isToken());                                              \
      if (__Child->isPresent()) {                                              \
        assert(__Child->getTokenKind() == TokenKind);                          \
        for (auto __Text : {__VA_ARGS__}) {                                    \
          if (__Child->getTokenText() == __Text) {                             \
            __Found = true;                                                    \
            break;                                                             \
          }                                                                    \
        }                                                                      \
        assert(__Found && "invalid text supplied for " #CursorName             \
                          ", expected one of {" #__VA_ARGS__ "}");             \
      }                                                                        \
    }                                                                          \
  } while (false)
#else
#define syntax_assert_child_token_text(Raw, CursorName, TokenKind, ...)
#endif

#ifndef NDEBUG
#define syntax_assert_token_is(Tok, Kind, Text)                                \
  do {                                                                         \
    assert(Tok.getTokenKind() == Kind);                                        \
    assert(Tok.getText() == Text);                                             \
  } while (false)
#else
#define syntax_assert_token_is(Tok, Kind, Text)
#endif

namespace swift {
namespace syntax {

class SyntaxArena;

using CursorIndex = size_t;

/// Get a numeric index suitable for array/vector indexing
/// from a syntax node's Cursor enum value.
template <typename CursorType> constexpr CursorIndex cursorIndex(CursorType C) {
  return static_cast<CursorIndex>(C);
}

/// An absolute position in a source file as text - the absolute offset from
/// the start, line, and column.
class AbsolutePosition {
  uintptr_t Offset = 0;
  uint32_t Line = 1;
  uint32_t Column = 1;

public:
  /// Add some number of columns to the position.
  void addColumns(uint32_t Columns) {
    Column += Columns;
    Offset += Columns;
  }

  /// Add some number of newlines to the position, resetting the column.
  /// Size is byte size of newline char.
  /// '\n' and '\r' are 1, '\r\n' is 2.
  void addNewlines(uint32_t NewLines, uint32_t Size) {
    Line += NewLines;
    Column = 1;
    Offset += NewLines * Size;
  }
  
  /// Use some text as a reference for adding to the absolute position,
  /// taking note of newlines, etc.
  void addText(StringRef Str) {
    const char * C = Str.begin();
    while (C != Str.end()) {
      switch (*C++) {
      case '\n':
        addNewlines(1, 1);
        break;
      case '\r':
        if (C != Str.end() && *C == '\n') {
          addNewlines(1, 2);
          ++C;
        } else {
          addNewlines(1, 1);
        }
        break;
      default:
        addColumns(1);
        break;
      }
    }
  }

  /// Get the line number of this position.
  uint32_t getLine() const { return Line; }

  /// Get the column number of this position.
  uint32_t getColumn() const { return Column; }

  /// Get the line and column number of this position.
  std::pair<uint32_t, uint32_t> getLineAndColumn() const {
    return {Line, Column};
  }

  /// Get the absolute offset of this position, suitable for indexing into a
  /// buffer if applicable.
  uintptr_t getOffset() const { return Offset; }

  /// Print the line and column as "l:c" to the given output stream.
  void printLineAndColumn(llvm::raw_ostream &OS) const;

  /// Dump a description of this position to the given output stream
  /// for debugging.
  void dump(llvm::raw_ostream &OS = llvm::errs()) const;
};

/// An indicator of whether a Syntax node was found or written in the source.
///
/// This is not an 'implicit' bit.
enum class SourcePresence {
  /// The syntax was authored by a human and found, or was generated.
  Present,

  /// The syntax was expected or optional, but not found in the source.
  Missing,
};

/// The print option to specify when printing a raw syntax node.
struct SyntaxPrintOptions {
  bool Visual = false;
  bool PrintSyntaxKind = false;
  bool PrintTrivialNodeKind = false;
};

/// RawSyntax - the strictly immutable, shared backing nodes for all syntax.
///
/// This is implementation detail - do not expose it in public API.
class RawSyntax final
    : public llvm::ThreadSafeRefCountedBase<RawSyntax>,
      private llvm::TrailingObjects<RawSyntax, RC<RawSyntax>, OwnedString,
                                    TriviaPiece> {
  friend TrailingObjects;

  union {
    uint64_t OpaqueBits;
    struct {
      /// The kind of syntax this node represents.
      unsigned Kind : bitmax(NumSyntaxKindBits, 8);
      /// Whether this piece of syntax was actually present in the source.
      unsigned Presence : 1;
      /// Whether this piece of syntax was constructed with manually managed
      /// memory.
      unsigned ManualMemory : 1;
    } Common;
    enum { NumRawSyntaxBits = bitmax(NumSyntaxKindBits, 8) + 1 + 1 };

    // For "layout" nodes.
    struct {
      static_assert(NumRawSyntaxBits <= 32,
                    "Only 32 bits reserved for standard syntax bits");
      uint64_t : bitmax(NumRawSyntaxBits, 32); // align to 32 bits
      /// Number of children this "layout" node has.
      unsigned NumChildren : 32;
      /// Number of bytes this node takes up spelled out in the source code
      unsigned TextLength : 32;
    } Layout;

    // For "token" nodes.
    struct {
      static_assert(NumRawSyntaxBits <= 16,
                    "Only 16 bits reserved for standard syntax bits");
      uint64_t : bitmax(NumRawSyntaxBits, 16); // align to 16 bits
      /// The kind of token this "token" node represents.
      unsigned TokenKind : 16;
      /// Number of leading  trivia pieces.
      unsigned NumLeadingTrivia : 16;
      /// Number of trailing trivia pieces.
      unsigned NumTrailingTrivia : 16;
    } Token;
  } Bits;

  size_t numTrailingObjects(OverloadToken<RC<RawSyntax>>) const {
    return isToken() ? 0 : Bits.Layout.NumChildren;
  }
  size_t numTrailingObjects(OverloadToken<OwnedString>) const {
    return isToken() ? 1 : 0;
  }
  size_t numTrailingObjects(OverloadToken<TriviaPiece>) const {
    return isToken()
             ? Bits.Token.NumLeadingTrivia + Bits.Token.NumTrailingTrivia
             : 0;
  }

  /// Constructor for creating layout nodes
  RawSyntax(SyntaxKind Kind, ArrayRef<RC<RawSyntax>> Layout,
            SourcePresence Presence, bool ManualMemory);
  /// Constructor for creating token nodes
  RawSyntax(tok TokKind, OwnedString Text,
            ArrayRef<TriviaPiece> LeadingTrivia,
            ArrayRef<TriviaPiece> TrailingTrivia,
            SourcePresence Presence, bool ManualMemory);

public:
  ~RawSyntax();

  void Release() const {
    if (Bits.Common.ManualMemory)
      return;
    return llvm::ThreadSafeRefCountedBase<RawSyntax>::Release();
  }
  void Retain() const {
    if (Bits.Common.ManualMemory)
      return;
    return llvm::ThreadSafeRefCountedBase<RawSyntax>::Retain();
  }

  /// \name Factory methods.
  /// @{
  
  /// Make a raw "layout" syntax node.
  static RC<RawSyntax> make(SyntaxKind Kind, ArrayRef<RC<RawSyntax>> Layout,
                            SourcePresence Presence,
                            SyntaxArena *Arena = nullptr);

  /// Make a raw "token" syntax node.
  static RC<RawSyntax> make(tok TokKind, OwnedString Text,
                            ArrayRef<TriviaPiece> LeadingTrivia,
                            ArrayRef<TriviaPiece> TrailingTrivia,
                            SourcePresence Presence,
                            SyntaxArena *Arena = nullptr);

  /// Make a missing raw "layout" syntax node.
  static RC<RawSyntax> missing(SyntaxKind Kind, SyntaxArena *Arena = nullptr) {
    return make(Kind, {}, SourcePresence::Missing, Arena);
  }

  /// Make a missing raw "token" syntax node.
  static RC<RawSyntax> missing(tok TokKind, OwnedString Text,
                               SyntaxArena *Arena = nullptr) {
    return make(TokKind, Text, {}, {}, SourcePresence::Missing, Arena);
  }

  static RC<RawSyntax> getToken(SyntaxArena &Arena, tok TokKind,
                                OwnedString Text,
                                ArrayRef<TriviaPiece> LeadingTrivia,
                                ArrayRef<TriviaPiece> TrailingTrivia);

  /// @}

  SourcePresence getPresence() const {
    return static_cast<SourcePresence>(Bits.Common.Presence);
  }

  SyntaxKind getKind() const {
    return static_cast<SyntaxKind>(Bits.Common.Kind);
  }

  /// Returns true if the node is "missing" in the source (i.e. it was
  /// expected (or optional) but not written.
  bool isMissing() const { return getPresence() == SourcePresence::Missing; }

  /// Returns true if the node is "present" in the source.
  bool isPresent() const { return getPresence() == SourcePresence::Present; }

  /// Returns true if this raw syntax node is some kind of declaration.
  bool isDecl() const { return isDeclKind(getKind()); }

  /// Returns true if this raw syntax node is some kind of type syntax.
  bool isType() const { return isTypeKind(getKind()); }

  /// Returns true if this raw syntax node is some kind of statement.
  bool isStmt() const { return isStmtKind(getKind()); }

  /// Returns true if this raw syntax node is some kind of expression.
  bool isExpr() const { return isExprKind(getKind()); }

  /// Returns true if this raw syntax node is some kind of pattern.
  bool isPattern() const { return isPatternKind(getKind()); }

  /// Return true is this raw syntax node is a unknown node.
  bool isUnknown() const { return isUnknownKind(getKind()); }

  /// Return true if this raw syntax node is a token.
  bool isToken() const { return isTokenKind(getKind()); }

  /// \name Getter routines for SyntaxKind::Token.
  /// @{

  /// Get the kind of the token.
  tok getTokenKind() const {
    assert(isToken());
    return static_cast<tok>(Bits.Token.TokenKind);
  }

  /// Return the text of the token.
  StringRef getTokenText() const {
    assert(isToken());
    return getTrailingObjects<OwnedString>()->str();
  }

  /// Return the leading trivia list of the token.
  ArrayRef<TriviaPiece> getLeadingTrivia() const {
    assert(isToken());
    return {getTrailingObjects<TriviaPiece>(), Bits.Token.NumLeadingTrivia};
  }
  /// Return the trailing trivia list of the token.
  ArrayRef<TriviaPiece> getTrailingTrivia() const {
    assert(isToken());
    return {getTrailingObjects<TriviaPiece>() + Bits.Token.NumLeadingTrivia,
            Bits.Token.NumTrailingTrivia};
  }

  /// Return \c true if this is the given kind of token.
  bool isToken(tok K) const { return isToken() && getTokenKind() == K; }

  /// @}

  /// \name Transform routines for "token" nodes.
  /// @{

  /// Return a new token like this one, but with the given leading
  /// trivia instead.
  RC<RawSyntax>
  withLeadingTrivia(ArrayRef<TriviaPiece> NewLeadingTrivia) const {
    return make(getTokenKind(), getTokenText(), NewLeadingTrivia,
                getTrailingTrivia(), getPresence());
  }

  RC<RawSyntax> withLeadingTrivia(Trivia NewLeadingTrivia) const {
    return withLeadingTrivia(NewLeadingTrivia.Pieces);
  }

  /// Return a new token like this one, but with the given trailing
  /// trivia instead.
  RC<RawSyntax>
  withTrailingTrivia(ArrayRef<TriviaPiece> NewTrailingTrivia) const {
    return make(getTokenKind(), getTokenText(), getLeadingTrivia(),
                NewTrailingTrivia, getPresence());
  }

  RC<RawSyntax> withTrailingTrivia(Trivia NewTrailingTrivia) const {
    return withTrailingTrivia(NewTrailingTrivia.Pieces);
  }

  /// @}

  /// \name Getter routines for "layout" nodes.
  /// @{

  /// Get the child nodes.
  ArrayRef<RC<RawSyntax>> getLayout() const {
    if (isToken())
      return {};
    return {getTrailingObjects<RC<RawSyntax>>(), Bits.Layout.NumChildren};
  }

  size_t getNumChildren() const {
    if (isToken())
      return 0;
    return Bits.Layout.NumChildren;
  }

  /// Get a child based on a particular node's "Cursor", indicating
  /// the position of the terms in the production of the Swift grammar.
  const RC<RawSyntax> &getChild(CursorIndex Index) const {
    return getLayout()[Index];
  }

  /// Return the number of bytes this node takes when spelled out in the source
  size_t getTextLength() {
    // For tokens the computation of the length is fast enough to justify the
    // space for caching it. For layout nodes, we cache the length to avoid
    // traversing the tree

    // FIXME: Or would it be sensible to cache the size of token nodes as well?
    if (isToken()) {
      AbsolutePosition Pos;
      accumulateAbsolutePosition(Pos);
      return Pos.getOffset();
    } else {
      return Bits.Layout.TextLength;
    }
  }

  /// @}

  /// \name Transform routines for "layout" nodes.
  /// @{

  /// Return a new raw syntax node with the given new layout element appended
  /// to the end of the node's layout.
  RC<RawSyntax> append(RC<RawSyntax> NewLayoutElement) const;

  /// Return a new raw syntax node with the given new layout element replacing
  /// another at some cursor position.
  RC<RawSyntax>
  replaceChild(CursorIndex Index, RC<RawSyntax> NewLayoutElement) const;

  /// @}

  /// Advance the provided AbsolutePosition by the full width of this node.
  ///
  /// If this is token node, returns the AbsolutePosition of the start of the
  /// token's nontrivial text. Otherwise, return the position of the first
  /// token. If this contains no tokens, return None.
  llvm::Optional<AbsolutePosition>
  accumulateAbsolutePosition(AbsolutePosition &Pos) const;

  /// Advance the provided AbsolutePosition by the first trivia of this node.
  /// Return true if we found this trivia; otherwise false.
  bool accumulateLeadingTrivia(AbsolutePosition &Pos) const;

  /// Print this piece of syntax recursively.
  void print(llvm::raw_ostream &OS, SyntaxPrintOptions Opts) const;

  /// Dump this piece of syntax recursively for debugging or testing.
  void dump() const;

  /// Dump this piece of syntax recursively.
  void dump(llvm::raw_ostream &OS, unsigned Indent = 0) const;

  static void Profile(llvm::FoldingSetNodeID &ID, tok TokKind, OwnedString Text,
                      ArrayRef<TriviaPiece> LeadingTrivia,
                      ArrayRef<TriviaPiece> TrailingTrivia);
};

} // end namespace syntax
} // end namespace swift

#endif // SWIFT_SYNTAX_RAWSYNTAX_H
