//===----------------------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2018 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

// This file contains only support for types deprecated from previous versions
// of Swift

@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "BidirectionalCollection", message: "it will be removed in Swift 5.0.  Please use 'BidirectionalCollection' instead")
public typealias BidirectionalIndexable = BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Collection", message: "it will be removed in Swift 5.0.  Please use 'Collection' instead")
public typealias IndexableBase = Collection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Collection", message: "it will be removed in Swift 5.0.  Please use 'Collection' instead")
public typealias Indexable = Collection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "MutableCollection", message: "it will be removed in Swift 5.0.  Please use 'MutableCollection' instead")
public typealias MutableIndexable = MutableCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "RandomAccessCollection", message: "it will be removed in Swift 5.0.  Please use 'RandomAccessCollection' instead")
public typealias RandomAccessIndexable = RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "RangeReplaceableIndexable", message: "it will be removed in Swift 5.0.  Please use 'RangeReplaceableCollection' instead")
public typealias RangeReplaceableIndexable = RangeReplaceableCollection
@available(*, deprecated: 4.2, renamed: "EnumeratedSequence.Iterator")
public typealias EnumeratedIterator<T: Sequence> = EnumeratedSequence<T>.Iterator
@available(*,deprecated: 4.2/*, obsoleted: 5.0*/, renamed: "CollectionOfOne.Iterator")
public typealias IteratorOverOne<T> = CollectionOfOne<T>.Iterator
@available(*, deprecated: 4.2/*, obsoleted: 5.0*/, renamed: "EmptyCollection.Iterator")
public typealias EmptyIterator<T> = EmptyCollection<T>.Iterator
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyFilterSequence.Iterator")
public typealias LazyFilterIterator<T: Sequence> = LazyFilterSequence<T>.Iterator
@available(swift, deprecated: 3.1/*, obsoleted: 5.0*/, message: "Use Base.Index")
public typealias LazyFilterIndex<Base: Collection> = Base.Index
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyDropWhileSequence.Iterator")
public typealias LazyDropWhileIterator<T> = LazyDropWhileSequence<T>.Iterator where T: Sequence
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyDropWhileCollection.Index")
public typealias LazyDropWhileIndex<T> = LazyDropWhileCollection<T>.Index where T: Collection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyDropWhileCollection")
public typealias LazyDropWhileBidirectionalCollection<T> = LazyDropWhileCollection<T> where T: BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyFilterCollection")
public typealias LazyFilterBidirectionalCollection<T> = LazyFilterCollection<T> where T : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyMapSequence.Iterator")
public typealias LazyMapIterator<T, E> = LazyMapSequence<T, E>.Iterator where T: Sequence
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyMapCollection")
public typealias LazyMapBidirectionalCollection<T, E> = LazyMapCollection<T, E> where T : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyMapCollection")
public typealias LazyMapRandomAccessCollection<T, E> = LazyMapCollection<T, E> where T : RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyCollection")
public typealias LazyBidirectionalCollection<T> = LazyCollection<T> where T : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyCollection")
public typealias LazyRandomAccessCollection<T> = LazyCollection<T> where T : RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "FlattenCollection.Index")
public typealias FlattenCollectionIndex<T> = FlattenCollection<T>.Index where T : Collection, T.Element : Collection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "FlattenCollection.Index")
public typealias FlattenBidirectionalCollectionIndex<T> = FlattenCollection<T>.Index where T : BidirectionalCollection, T.Element : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "FlattenCollection")
public typealias FlattenBidirectionalCollection<T> = FlattenCollection<T> where T : BidirectionalCollection, T.Element : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "JoinedSequence.Iterator")
public typealias JoinedIterator<T: Sequence> = JoinedSequence<T>.Iterator where T.Element: Sequence
@available(*, deprecated: 4.2/*, obsoleted: 5.0*/, renamed: "Zip2Sequence.Iterator")
public typealias Zip2Iterator<T, U> = Zip2Sequence<T, U>.Iterator where T: Sequence, U: Sequence
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyDropWhileSequence.Iterator")
public typealias LazyPrefixWhileIterator<T> = LazyPrefixWhileSequence<T>.Iterator where T: Sequence
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyDropWhileCollection.Index")
public typealias LazyPrefixWhileIndex<T> = LazyPrefixWhileCollection<T>.Index where T: Collection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "LazyPrefixWhileCollection")
public typealias LazyPrefixWhileBidirectionalCollection<T> = LazyPrefixWhileCollection<T> where T: BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ReversedCollection")
public typealias ReversedRandomAccessCollection<T: RandomAccessCollection> = ReversedCollection<T>
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ReversedCollection.Index")
public typealias ReversedIndex<T: BidirectionalCollection> = ReversedCollection<T>
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias BidirectionalSlice<T> = Slice<T> where T : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias RandomAccessSlice<T> = Slice<T> where T : RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias RangeReplaceableSlice<T> = Slice<T> where T : RangeReplaceableCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias RangeReplaceableBidirectionalSlice<T> = Slice<T> where T : RangeReplaceableCollection & BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias RangeReplaceableRandomAccessSlice<T> = Slice<T> where T : RangeReplaceableCollection & RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableSlice<T> = Slice<T> where T : MutableCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableBidirectionalSlice<T> = Slice<T> where T : MutableCollection & BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableRandomAccessSlice<T> = Slice<T> where T : MutableCollection & RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableRangeReplaceableSlice<T> = Slice<T> where T : MutableCollection & RangeReplaceableCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableRangeReplaceableBidirectionalSlice<T> = Slice<T> where T : MutableCollection & RangeReplaceableCollection & BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Slice")
public typealias MutableRangeReplaceableRandomAccessSlice<T> = Slice<T> where T : MutableCollection & RangeReplaceableCollection & RandomAccessCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "DefaultIndices")
public typealias DefaultBidirectionalIndices<T> = DefaultIndices<T> where T : BidirectionalCollection
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "DefaultIndices")
public typealias DefaultRandomAccessIndices<T> = DefaultIndices<T> where T : RandomAccessCollection

// Deprecated by SE-0115.
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByNilLiteral")
public typealias NilLiteralConvertible = ExpressibleByNilLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinIntegerLiteral")
public typealias _BuiltinIntegerLiteralConvertible = _ExpressibleByBuiltinIntegerLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByIntegerLiteral")
public typealias IntegerLiteralConvertible = ExpressibleByIntegerLiteral
#if !KERNELLIB

@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinFloatLiteral")
public typealias _BuiltinFloatLiteralConvertible = _ExpressibleByBuiltinFloatLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByFloatLiteral")
public typealias FloatLiteralConvertible = ExpressibleByFloatLiteral
#endif

@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinBooleanLiteral")
public typealias _BuiltinBooleanLiteralConvertible = _ExpressibleByBuiltinBooleanLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByBooleanLiteral")
public typealias BooleanLiteralConvertible = ExpressibleByBooleanLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinUnicodeScalarLiteral")
public typealias _BuiltinUnicodeScalarLiteralConvertible = _ExpressibleByBuiltinUnicodeScalarLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByUnicodeScalarLiteral")
public typealias UnicodeScalarLiteralConvertible = ExpressibleByUnicodeScalarLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinExtendedGraphemeClusterLiteral")
public typealias _BuiltinExtendedGraphemeClusterLiteralConvertible = _ExpressibleByBuiltinExtendedGraphemeClusterLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByExtendedGraphemeClusterLiteral")
public typealias ExtendedGraphemeClusterLiteralConvertible = ExpressibleByExtendedGraphemeClusterLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinStringLiteral")
public typealias _BuiltinStringLiteralConvertible = _ExpressibleByBuiltinStringLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByBuiltinUTF16StringLiteral")
public typealias _BuiltinUTF16StringLiteralConvertible = _ExpressibleByBuiltinUTF16StringLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByStringLiteral")
public typealias StringLiteralConvertible = ExpressibleByStringLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByArrayLiteral")
public typealias ArrayLiteralConvertible = ExpressibleByArrayLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "ExpressibleByDictionaryLiteral")
public typealias DictionaryLiteralConvertible = ExpressibleByDictionaryLiteral
@available(*, deprecated, message: "it will be replaced or redesigned in Swift 4.0.  Instead of conforming to 'StringInterpolationConvertible', consider adding an 'init(_:String)'")
public typealias StringInterpolationConvertible = ExpressibleByStringInterpolation

#if !KERNELLIB

@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByColorLiteral")
public typealias _ColorLiteralConvertible = _ExpressibleByColorLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByImageLiteral")
public typealias _ImageLiteralConvertible = _ExpressibleByImageLiteral
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "_ExpressibleByFileReferenceLiteral")
public typealias _FileReferenceLiteralConvertible = _ExpressibleByFileReferenceLiteral

#endif

@available(*, deprecated, obsoleted: 5.0, renamed: "ClosedRange.Index")
public typealias ClosedRangeIndex<T> = ClosedRange<T>.Index where T: Strideable, T.Stride: SignedInteger

/// An optional type that allows implicit member access.
///
/// The `ImplicitlyUnwrappedOptional` type is deprecated. To create an optional
/// value that is implicitly unwrapped, place an exclamation mark (`!`) after
/// the type that you want to denote as optional.
///
///     // An implicitly unwrapped optional integer
///     let guaranteedNumber: Int! = 6
///
///     // An optional integer
///     let possibleNumber: Int? = 5
@available(*, unavailable, renamed: "Optional")
public typealias ImplicitlyUnwrappedOptional<Wrapped> = Optional<Wrapped>

@available(swift, deprecated: 3.1, obsoleted: 4.0, message: "Use FixedWidthInteger protocol instead")
public typealias BitwiseOperations = _BitwiseOperations

public protocol _BitwiseOperations {
  static func & (lhs: Self, rhs: Self) -> Self
  static func | (lhs: Self, rhs: Self) -> Self
  static func ^ (lhs: Self, rhs: Self) -> Self
  static prefix func ~ (x: Self) -> Self
  static var allZeros: Self { get }
}

extension _BitwiseOperations {  
    @available(swift, obsoleted: 4.1)
    public static func |= (lhs: inout Self, rhs: Self) {
      lhs = lhs | rhs
    }

    @available(swift, obsoleted: 4.1)
    public static func &= (lhs: inout Self, rhs: Self) {
      lhs = lhs & rhs
    }

    @available(swift, obsoleted: 4.1)
    public static func ^= (lhs: inout Self, rhs: Self) {
      lhs = lhs ^ rhs
    }
}
#if !KERNELLIB
extension FloatingPoint {
  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public func negated() -> Self {
    return -self
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public func adding(_ other: Self) -> Self {
    return self + other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public mutating func add(_ other: Self) {
    self += other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public func subtracting(_ other: Self) -> Self {
    return self - other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public mutating func subtract(_ other: Self) {
    self -= other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public func multiplied(by other: Self) -> Self {
    return self * other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public mutating func multiply(by other: Self) {
    self *= other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public func divided(by other: Self) -> Self {
    return self / other
  }

  @available(swift, obsoleted: 4, message: "Please use operators instead.")
  public mutating func divide(by other: Self) {
    self /= other
  }
}

extension FloatingPoint {
  @available(*, unavailable, message: "Use bitPattern property instead")
  public func _toBitPattern() -> UInt {
    fatalError("unavailable")
  }

  @available(*, unavailable, message: "Use init(bitPattern:) instead")
  public static func _fromBitPattern(_ bits: UInt) -> Self {
    fatalError("unavailable")
  }
}

extension BinaryFloatingPoint {
  @available(*, unavailable, renamed: "isSignalingNaN")
  public var isSignaling: Bool {
    fatalError("unavailable")
  }

  @available(*, unavailable, renamed: "nan")
  public var NaN: Bool {
    fatalError("unavailable")
  }
  @available(*, unavailable, renamed: "nan")
  public var quietNaN: Bool {
    fatalError("unavailable")
  }
}

@available(*, unavailable, renamed: "FloatingPoint")
public typealias FloatingPointType = FloatingPoint
#endif // !KERNELLIB

// Swift 3 compatibility APIs
@available(swift, obsoleted: 4, renamed: "BinaryInteger")
public typealias Integer = BinaryInteger

@available(swift, obsoleted: 4, renamed: "BinaryInteger")
public typealias IntegerArithmetic = BinaryInteger

@available(swift, obsoleted: 4, message: "Please use 'SignedNumeric & Comparable' instead.")
public typealias SignedNumber = SignedNumeric & Comparable

@available(swift, obsoleted: 4, message: "Please use 'SignedNumeric & Comparable' instead.")
public typealias AbsoluteValuable = SignedNumeric & Comparable

@available(swift, obsoleted: 4, renamed: "SignedInteger")
public typealias _SignedInteger = SignedInteger

extension SignedNumeric where Self : Comparable {
  @available(swift, obsoleted: 4, message: "Please use the 'abs(_:)' free function.")
  @_transparent
  public static func abs(_ x: Self) -> Self {
    return Swift.abs(x)
  }
}

extension BinaryInteger {
  @available(swift, obsoleted: 4)
  public func toIntMax() -> Int64 {
    return Int64(self)
  }
}

extension UnsignedInteger {
  @available(swift, obsoleted: 4)
  public func toUIntMax() -> UInt64 {
    return UInt64(self)
  }
}

extension Range where Bound: Strideable, Bound.Stride : SignedInteger {
  /// Now that Range is conditionally a collection when Bound: Strideable,
  /// CountableRange is no longer needed. This is a deprecated initializer
  /// for any remaining uses of Range(countableRange).
  @available(*, deprecated: 4.2/*, obsoleted: 5.0*/, message: "CountableRange is now a Range. No need to convert any more.")
  public init(_ other: Range<Bound>) {
    self = other
  }  
}

extension ClosedRange where Bound: Strideable, Bound.Stride : SignedInteger {
  /// Now that Range is conditionally a collection when Bound: Strideable,
  /// CountableRange is no longer needed. This is a deprecated initializer
  /// for any remaining uses of Range(countableRange).
  @available(*, deprecated: 4.2/*, obsoleted: 5.0*/, message: "CountableClosedRange is now a ClosedRange. No need to convert any more.")
  public init(_ other: ClosedRange<Bound>) {
    self = other
  }  
}

#if !KERNELLIB
extension _ExpressibleByColorLiteral {
  @available(swift, deprecated: 3.2, obsoleted: 4.0, message: "This initializer is only meant to be used by color literals")
  public init(colorLiteralRed red: Float, green: Float, blue: Float, alpha: Float) {
    self.init(_colorLiteralRed: red, green: green, blue: blue, alpha: alpha)
  }
}
#endif

extension LazySequenceProtocol {
  /// Returns the non-`nil` results of mapping the given transformation over
  /// this sequence.
  ///
  /// Use this method to receive a sequence of nonoptional values when your
  /// transformation produces an optional value.
  ///
  /// - Parameter transform: A closure that accepts an element of this sequence
  ///   as its argument and returns an optional value.
  ///
  /// - Complexity: O(1)
  @available(swift, deprecated: 4.1, renamed: "compactMap(_:)", message: "Please use compactMap(_:) for the case where closure returns an optional value")
  public func flatMap<ElementOfResult>(
    _ transform: @escaping (Elements.Element) -> ElementOfResult?
  ) -> LazyMapSequence<
    LazyFilterSequence<
      LazyMapSequence<Elements, ElementOfResult?>>,
    ElementOfResult
  > {
    return self.compactMap(transform)
  }
}

extension LazyMapCollection {
  // This overload is needed to re-enable Swift 3 source compatibility related
  // to a bugfix in ranking behavior of the constraint solver.
  @available(swift, obsoleted: 4.0)
  public static func + <
    Other : LazyCollectionProtocol
  >(lhs: LazyMapCollection, rhs: Other) -> [Element]
  where Other.Element == Element {
    var result: [Element] = []
    result.reserveCapacity(numericCast(lhs.count + rhs.count))
    result.append(contentsOf: lhs)
    result.append(contentsOf: rhs)
    return result
  }
}

#if !KERNELLIB
@available(*, unavailable, message: "use += 1")
@discardableResult
public prefix func ++ <F: FloatingPoint>(rhs: inout F) -> F {
  fatalError("++ is not available")
}
@available(*, unavailable, message: "use -= 1")
@discardableResult
public prefix func -- <F: FloatingPoint>(rhs: inout F) -> F {
  fatalError("-- is not available")
}
@available(*, unavailable, message: "use += 1")
@discardableResult
public postfix func ++ <F: FloatingPoint>(lhs: inout F) -> F {
  fatalError("++ is not available")
}
@available(*, unavailable, message: "use -= 1")
@discardableResult
public postfix func -- <F: FloatingPoint>(lhs: inout F) -> F {
  fatalError("-- is not available")
}

extension FloatingPoint {
  @available(swift, deprecated: 3.1, obsoleted: 4.0, message: "Please use the `abs(_:)` free function")
  public static func abs(_ x: Self) -> Self {
    return x.magnitude
  }
}
#endif

extension FixedWidthInteger {
  /// The empty bitset.
  @available(swift, deprecated: 3.1, obsoleted: 4.0, message: "Use 0")
  public static var allZeros: Self { return 0 }
}

extension LazyCollectionProtocol {
  /// Returns the non-`nil` results of mapping the given transformation over
  /// this collection.
  ///
  /// Use this method to receive a collection of nonoptional values when your
  /// transformation produces an optional value.
  ///
  /// - Parameter transform: A closure that accepts an element of this
  ///   collection as its argument and returns an optional value.
  ///
  /// - Complexity: O(1)
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "compactMap(_:)",
    message: "Please use compactMap(_:) for the case where closure returns an optional value")
  public func flatMap<ElementOfResult>(
    _ transform: @escaping (Elements.Element) -> ElementOfResult?
  ) -> LazyMapCollection<
    LazyFilterCollection<
      LazyMapCollection<Elements, ElementOfResult?>>,
    ElementOfResult
  > {
    return self.map(transform).filter { $0 != nil }.map { $0! }
  }
}

extension String {
  /// A view of a string's contents as a collection of characters.
  ///
  /// Previous versions of Swift provided this view since String
  /// itself was not a collection. String is now a collection of
  /// characters, so this type is now just an alias for String.
  @available(swift, deprecated: 3.2, obsoleted: 5.0, message: "Please use String directly")
  public typealias CharacterView = String

  /// A view of the string's contents as a collection of characters.
  ///
  /// Previous versions of Swift provided this view since String
  /// itself was not a collection. String is now a collection of
  /// characters, so this type is now just an alias for String.
  @available(swift, deprecated: 3.2, obsoleted: 5.0, message: "Please use String directly")
  public var characters: String {
    get {
      return self
    }
    set {
      self = newValue
    }
  }

  /// Applies the given closure to a mutable view of the string's characters.
  ///
  /// Previous versions of Swift provided this view since String
  /// itself was not a collection. String is now a collection of
  /// characters, so this type is now just an alias for String.
  @available(swift, deprecated: 3.2/*, obsoleted: 5.0*/, message: "Please mutate the String directly")
  public mutating func withMutableCharacters<R>(
    _ body: (inout String) -> R
  ) -> R {
    return body(&self)
  }

  @available(swift, deprecated: 3.2, obsoleted: 4.0)
  public init?(_ utf16: UTF16View) {
    // Attempt to recover the whole string, the better to implement the actual
    // Swift 3.1 semantics, which are not as documented above!  Full Swift 3.1
    // semantics may be impossible to preserve in the case of string literals,
    // since we no longer have access to the length of the original string when
    // there is no owner and elements are dropped from the end.
    let wholeString = String(utf16._guts)
    guard
      let start = UTF16Index(encodedOffset: utf16._offset)
        .samePosition(in: wholeString),
      let end = UTF16Index(encodedOffset: utf16._offset + utf16._length)
        .samePosition(in: wholeString)
      else
    {
        return nil
    }
    self = String(wholeString[start..<end])
  }
  
  @available(swift, deprecated: 3.2, message: "Failable initializer was removed in Swift 4. When upgrading to Swift 4, please use non-failable String.init(_:UTF8View)")
  @available(swift, obsoleted: 4.0, message: "Please use non-failable String.init(_:UTF8View) instead")
  public init?(_ utf8: UTF8View) {
    if utf8.startIndex.transcodedOffset != 0
      || utf8.endIndex.transcodedOffset != 0
      || utf8._legacyPartialCharacters.start
      || utf8._legacyPartialCharacters.end {
      return nil
    }
    self = String(utf8._guts)
  }
}

extension String { // RangeReplaceableCollection
  // The defaulted argument prevents this initializer from satisfies the
  // LosslessStringConvertible conformance.  You can satisfy a protocol
  // requirement with something that's not yet available, but not with
  // something that has become unavailable. Without this, the code won't
  // compile as Swift 4.
  @available(swift, obsoleted: 4, message: "String.init(_:String) is no longer failable")
  public init?(_ other: String, obsoletedInSwift4: () = ()) {
    self.init(other._guts)
  }
}

extension String.UnicodeScalarView : _CustomPlaygroundQuickLookable {
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UnicodeScalarView.customPlaygroundQuickLook will be removed in Swift 5.0")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return .text(description)
  }
}

// backward compatibility for index interchange.
extension String.UnicodeScalarView {
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(after i: Index?) -> Index {
    return index(after: i!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(_ i: Index?,  offsetBy n: Int) -> Index {
    return index(i!, offsetBy: n)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional indices")
  public func distance(from i: Index?, to j: Index?) -> Int {
    return distance(from: i!, to: j!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public subscript(i: Index?) -> Unicode.Scalar {
    return self[i!]
  }
}

// backward compatibility for index interchange.
extension String.UTF16View {
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(after i: Index?) -> Index {
    return index(after: i!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(_ i: Index?, offsetBy n: Int) -> Index {
    return index(i!, offsetBy: n)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional indices")
  public func distance(from i: Index?, to j: Index?) -> Int {
    return distance(from: i!, to: j!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public subscript(i: Index?) -> Unicode.UTF16.CodeUnit {
    return self[i!]
  }
}

// backward compatibility for index interchange.
extension String.UTF8View {
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(after i: Index?) -> Index {
    return index(after: i!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public func index(_ i: Index?, offsetBy n: Int) -> Index {
    return index(i!, offsetBy: n)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional indices")
  public func distance(from i: Index?, to j: Index?) -> Int {
    return distance(from: i!, to: j!)
  }
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional index")
  public subscript(i: Index?) -> Unicode.UTF8.CodeUnit {
    return self[i!]
  }
}

//===--- String/Substring Slicing Support ---------------------------------===//
/// In Swift 3.2, in the absence of type context,
///
///     someString[someString.startIndex..<someString.endIndex]
///
/// was deduced to be of type `String`.  Therefore have a more-specific
/// Swift-3-only `subscript` overload on `String` (and `Substring`) that
/// continues to produce `String`.
extension String {
  @available(swift, obsoleted: 4)
  public subscript(bounds: Range<Index>) -> String {
    _boundsCheck(bounds)
    return String(Substring(_slice: Slice(base: self, bounds: bounds)))
  }

  @available(swift, obsoleted: 4)
  public subscript(bounds: ClosedRange<Index>) -> String {
    let r = bounds.relative(to: self)
    _boundsCheck(r)
    return String(Substring(_slice: Slice(
          base: self,
          bounds: r)))
  }
}


//===--- Slicing Support --------------------------------------------------===//
// In Swift 3.2, in the absence of type context,
//
//   someString.unicodeScalars[
//     someString.unicodeScalars.startIndex
//     ..< someString.unicodeScalars.endIndex]
//
// was deduced to be of type `String.UnicodeScalarView`.  Provide a
// more-specific Swift-3-only `subscript` overload that continues to produce
// `String.UnicodeScalarView`.
extension String.UnicodeScalarView {
  private subscript(_bounds bounds: Range<Index>) -> String.UnicodeScalarView {
    let rawSubRange: Range<Int> =
      _toCoreIndex(bounds.lowerBound)..<_toCoreIndex(bounds.upperBound)
    return String.UnicodeScalarView(
      _guts._extractSlice(rawSubRange),
      coreOffset: bounds.lowerBound.encodedOffset)
  }

  @available(swift, obsoleted: 4)
  public subscript(bounds: Range<Index>) -> String.UnicodeScalarView {
    return self[_bounds: bounds]
  }

  @available(swift, obsoleted: 4)
  public subscript(bounds: ClosedRange<Index>) -> String.UnicodeScalarView {
    return self[_bounds: bounds.relative(to: self)]
  }
}

//===--- Slicing Support --------------------------------------------------===//
// In Swift 3.2, in the absence of type context,
//
//   someString.utf16[someString.utf16.startIndex..<someString.utf16.endIndex]
//
// was deduced to be of type `String.UTF16View`.  Provide a more-specific
// Swift-3-only `subscript` overload that continues to produce
// `String.UTF16View`.
extension String.UTF16View {
  private subscript(_bounds bounds: Range<Index>) -> String.UTF16View {
    return String.UTF16View(
      _guts,
      offset: _internalIndex(at: bounds.lowerBound.encodedOffset),
      length: bounds.upperBound.encodedOffset - bounds.lowerBound.encodedOffset)
  }

  @available(swift, obsoleted: 4)
  public subscript(bounds: Range<Index>) -> String.UTF16View {
    return self[_bounds: bounds]
  }

  @available(swift, obsoleted: 4)
  public subscript(bounds: ClosedRange<Index>) -> String.UTF16View {
    return self[_bounds: bounds.relative(to: self)]
  }
}

//===--- Slicing Support --------------------------------------------------===//
/// In Swift 3.2, in the absence of type context,
///
///   someString.utf8[someString.utf8.startIndex..<someString.utf8.endIndex]
///
/// was deduced to be of type `String.UTF8View`.  Provide a more-specific
/// Swift-3-only `subscript` overload that continues to produce
/// `String.UTF8View`.
extension String.UTF8View {
  private subscript(_bounds bounds: Range<Index>) -> String.UTF8View {
    let wholeString = String(_guts)
    let legacyPartialCharacters = (
      (self._legacyPartialCharacters.start &&
        bounds.lowerBound.encodedOffset == 0) ||
      bounds.lowerBound.samePosition(in: wholeString) == nil,
      (self._legacyPartialCharacters.end &&
        bounds.upperBound.encodedOffset == _guts.count) ||
      bounds.upperBound.samePosition(in: wholeString) == nil)

    if bounds.upperBound.transcodedOffset == 0 {
      return String.UTF8View(
        _guts._extractSlice(
        bounds.lowerBound.encodedOffset..<bounds.upperBound.encodedOffset),
        legacyOffsets: (bounds.lowerBound.transcodedOffset, 0),
        legacyPartialCharacters: legacyPartialCharacters)
    }

    let b0 = bounds.upperBound.utf8Buffer!.first!
    let scalarLength8 = (~b0).leadingZeroBitCount
    let scalarLength16 = scalarLength8 == 4 ? 2 : 1
    let coreEnd = bounds.upperBound.encodedOffset + scalarLength16
    return String.UTF8View(
      _guts._extractSlice(bounds.lowerBound.encodedOffset..<coreEnd),
      legacyOffsets: (
        bounds.lowerBound.transcodedOffset,
        bounds.upperBound.transcodedOffset - scalarLength8),
      legacyPartialCharacters: legacyPartialCharacters)
  }
  
  @available(swift, obsoleted: 4)
  public subscript(bounds: Range<Index>) -> String.UTF8View {
    return self[_bounds: bounds]
  }
  

  @available(swift, obsoleted: 4)
  public subscript(bounds: ClosedRange<Index>) -> String.UTF8View {
    return self[_bounds: bounds.relative(to: self)]
  }
}

@available(swift,deprecated: 5.0, renamed: "Unicode.UTF8")
public typealias UTF8 = Unicode.UTF8
@available(swift, deprecated: 5.0, renamed: "Unicode.UTF16")
public typealias UTF16 = Unicode.UTF16
@available(swift, deprecated: 5.0, renamed: "Unicode.UTF32")
public typealias UTF32 = Unicode.UTF32
@available(swift, obsoleted: 5.0, renamed: "Unicode.Scalar")
public typealias UnicodeScalar = Unicode.Scalar


// popFirst() is only present when a collection is its own subsequence. This was
// dropped in Swift 4.
extension String {
  @available(swift, deprecated: 3.2, obsoleted: 4, message: "Please use 'first', 'dropFirst()', or 'Substring.popFirst()'.")
  public mutating func popFirst() -> String.Element? {
    guard !isEmpty else { return nil }
    let element = first!
    let nextIdx = self.index(after: self.startIndex)
    self = String(self[nextIdx...])
    return element
  }
}

extension String.UnicodeScalarView {
  @available(swift, deprecated: 3.2, obsoleted: 4, message: "Please use 'first', 'dropFirst()', or 'Substring.UnicodeScalarView.popFirst()'.")
  public mutating func popFirst() -> String.UnicodeScalarView.Element? {
    guard !isEmpty else { return nil }
    let element = first!
    let nextIdx = self.index(after: self.startIndex)
    self = String(self[nextIdx...]).unicodeScalars
    return element
  }
}

extension String.UTF16View : _CustomPlaygroundQuickLookable {
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UTF16View.customPlaygroundQuickLook will be removed in Swift 5.0")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return .text(description)
  }
}

extension String.UTF8View : _CustomPlaygroundQuickLookable {
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UTF8View.customPlaygroundQuickLook will be removed in Swift 5.0")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return .text(description)
  }
}

extension StringProtocol {
  @available(swift, deprecated: 3.2, obsoleted: 4.0, renamed: "UTF8View.Index")
  public typealias UTF8Index = UTF8View.Index
  @available(swift, deprecated: 3.2, obsoleted: 4.0, renamed: "UTF16View.Index")
  public typealias UTF16Index = UTF16View.Index
  @available(swift, deprecated: 3.2, obsoleted: 4.0, renamed: "UnicodeScalarView.Index")
  public typealias UnicodeScalarIndex = UnicodeScalarView.Index
}

extension Substring {
  /// A view of a string's contents as a collection of characters.
  ///
  /// Previous versions of Swift provided this view since String
  /// itself was not a collection. String is now a collection of
  /// characters, so this type is now just an alias for String.
  @available(swift, deprecated: 3.2, obsoleted: 5.0, message: "Please use Substring directly")
  public typealias CharacterView = Substring

  /// A view of the string's contents as a collection of characters.
  @available(swift, deprecated: 3.2, obsoleted: 5.0, message: "Please use Substring directly")
  public var characters: Substring {
    get {
      return self
    }
    set {
      self = newValue
    }
  }

  /// Applies the given closure to a mutable view of the string's characters.
  ///
  /// Previous versions of Swift provided this view since String
  /// itself was not a collection. String is now a collection of
  /// characters, so this type is now just an alias for String.
  @available(swift, deprecated: 3.2/*, obsoleted: 5.0*/, message: "Please mutate the Substring directly")
  public mutating func withMutableCharacters<R>(
    _ body: (inout Substring) -> R
  ) -> R {
    return body(&self)
  }
  
  private func _boundsCheck(_ range: Range<Index>) {
    _precondition(range.lowerBound >= startIndex,
      "String index range is out of bounds")
    _precondition(range.upperBound <= endIndex,
      "String index range is out of bounds")
  }
  
  @available(swift, obsoleted: 4)
  public subscript(bounds: ClosedRange<Index>) -> String {
    return String(self[bounds.relative(to: self)])
  }
}

extension Substring : _CustomPlaygroundQuickLookable {
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "Substring.customPlaygroundQuickLook will be removed in Swift 5.0")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return String(self).customPlaygroundQuickLook
  }
}

extension Collection {
  // FIXME: <rdar://problem/34142121>
  // This typealias should be removed as it predates the source compatibility
  // guarantees of Swift 3, but it cannot due to a bug.
  @available(*, unavailable, renamed: "Iterator")
  public typealias Generator = Iterator

  @available(swift, deprecated: 3.2, obsoleted: 5.0, renamed: "Element")
  public typealias _Element = Element

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "all index distances are now of type Int")
  public func index<T: BinaryInteger>(_ i: Index, offsetBy n: T) -> Index {
    return index(i, offsetBy: Int(n))
  }
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "all index distances are now of type Int")
  public func formIndex<T: BinaryInteger>(_ i: inout Index, offsetBy n: T) {
    return formIndex(&i, offsetBy: Int(n))
  }
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "all index distances are now of type Int")
  public func index<T: BinaryInteger>(_ i: Index, offsetBy n: T, limitedBy limit: Index) -> Index? {
    return index(i, offsetBy: Int(n), limitedBy: limit)
  }
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "all index distances are now of type Int")
  public func formIndex<T: BinaryInteger>(_ i: inout Index, offsetBy n: T, limitedBy limit: Index) -> Bool {
    return formIndex(&i, offsetBy: Int(n), limitedBy: limit)
  }
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "all index distances are now of type Int")
  public func distance<T: BinaryInteger>(from start: Index, to end: Index) -> T {
    return numericCast(distance(from: start, to: end) as Int)
  }
}


extension UnsafeMutablePointer {
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "initialize(repeating:count:)")
  public func initialize(to newValue: Pointee, count: Int = 1) { 
    initialize(repeating: newValue, count: count)
  }

  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, message: "the default argument to deinitialize(count:) has been removed, please specify the count explicitly") 
  @discardableResult
  public func deinitialize() -> UnsafeMutableRawPointer {
    return deinitialize(count: 1)
  }
  
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, message: "Swift currently only supports freeing entire heap blocks, use deallocate() instead")
  public func deallocate(capacity _: Int) { 
    self.deallocate()
  }

  /// Initializes memory starting at this pointer's address with the elements
  /// of the given collection.
  ///
  /// The region of memory starting at this pointer and covering `source.count`
  /// instances of the pointer's `Pointee` type must be uninitialized or
  /// `Pointee` must be a trivial type. After calling `initialize(from:)`, the
  /// region is initialized.
  ///
  /// - Parameter source: A collection of elements of the pointer's `Pointee`
  ///   type.
  // This is fundamentally unsafe since collections can underreport their count.
  @available(*, deprecated/*, obsoleted: 5.0*/, message: "it will be removed in Swift 5.0.  Please use 'UnsafeMutableBufferPointer.initialize(from:)' instead")
  public func initialize<C : Collection>(from source: C)
    where C.Element == Pointee {
    let buf = UnsafeMutableBufferPointer(start: self, count: numericCast(source.count))
    var (remainders,writtenUpTo) = source._copyContents(initializing: buf)
    // ensure that exactly rhs.count elements were written
    _precondition(remainders.next() == nil, "rhs underreported its count")
    _precondition(writtenUpTo == buf.endIndex, "rhs overreported its count")
  }
}

extension UnsafeRawPointer : _CustomPlaygroundQuickLookable {
  internal var summary: String {
    let ptrValue = UInt64(
      bitPattern: Int64(Int(Builtin.ptrtoint_Word(_rawValue))))
    return ptrValue == 0
    ? "UnsafeRawPointer(nil)"
    : "UnsafeRawPointer(0x\(_uint64ToString(ptrValue, radix:16, uppercase:true)))"
  }

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UnsafeRawPointer.customPlaygroundQuickLook will be removed in a future Swift version")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return .text(summary)
  }
}

extension UnsafeMutableRawPointer : _CustomPlaygroundQuickLookable {
  private var summary: String {
    let ptrValue = UInt64(
      bitPattern: Int64(Int(Builtin.ptrtoint_Word(_rawValue))))
    return ptrValue == 0
    ? "UnsafeMutableRawPointer(nil)"
    : "UnsafeMutableRawPointer(0x\(_uint64ToString(ptrValue, radix:16, uppercase:true)))"
  }

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UnsafeMutableRawPointer.customPlaygroundQuickLook will be removed in a future Swift version")
  public var customPlaygroundQuickLook: _PlaygroundQuickLook {
    return .text(summary)
  }
}

extension UnsafePointer: _CustomPlaygroundQuickLookable {
  private var summary: String {
    let ptrValue = UInt64(bitPattern: Int64(Int(Builtin.ptrtoint_Word(_rawValue))))
    return ptrValue == 0 
    ? "UnsafePointer(nil)" 
    : "UnsafePointer(0x\(_uint64ToString(ptrValue, radix:16, uppercase:true)))"
  }

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UnsafePointer.customPlaygroundQuickLook will be removed in a future Swift version")
  public var customPlaygroundQuickLook: PlaygroundQuickLook {
    return .text(summary)
  }
}

extension UnsafeMutablePointer: _CustomPlaygroundQuickLookable {
  private var summary: String {
    let ptrValue = UInt64(bitPattern: Int64(Int(Builtin.ptrtoint_Word(_rawValue))))
    return ptrValue == 0 
    ? "UnsafeMutablePointer(nil)" 
    : "UnsafeMutablePointer(0x\(_uint64ToString(ptrValue, radix:16, uppercase:true)))"
  }

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "UnsafeMutablePointer.customPlaygroundQuickLook will be removed in a future Swift version")
  public var customPlaygroundQuickLook: PlaygroundQuickLook {
    return .text(summary)
  }
}

@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "UnsafeBufferPointer.Iterator")
public typealias UnsafeBufferPointerIterator<T> = UnsafeBufferPointer<T>.Iterator
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "UnsafeRawBufferPointer.Iterator")
public typealias UnsafeRawBufferPointerIterator<T> = UnsafeBufferPointer<T>.Iterator
@available(*, deprecated/*, obsoleted: 5.0*/, renamed: "UnsafeRawBufferPointer.Iterator")
public typealias UnsafeMutableRawBufferPointerIterator<T> = UnsafeBufferPointer<T>.Iterator

extension UnsafeMutableRawPointer {
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "allocate(byteCount:alignment:)")
  public static func allocate(
    bytes size: Int, alignedTo alignment: Int
  ) -> UnsafeMutableRawPointer {
    return UnsafeMutableRawPointer.allocate(byteCount: size, alignment: alignment)
  }
  
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "deallocate()", message: "Swift currently only supports freeing entire heap blocks, use deallocate() instead")
  public func deallocate(bytes _: Int, alignedTo _: Int) { 
    self.deallocate()
  }

  @available(swift, deprecated: 4.1, obsoleted: 5.0, renamed: "copyMemory(from:byteCount:)")
  public func copyBytes(from source: UnsafeRawPointer, count: Int) {
    copyMemory(from: source, byteCount: count)
  }

  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "initializeMemory(as:repeating:count:)")
  @discardableResult
  public func initializeMemory<T>(
    as type: T.Type, at offset: Int = 0, count: Int = 1, to repeatedValue: T
  ) -> UnsafeMutablePointer<T> { 
    return (self + offset * MemoryLayout<T>.stride).initializeMemory(
      as: type, repeating: repeatedValue, count: count)
  }

  @available(*, deprecated/*, obsoleted: 5.0*/, message: "it will be removed in Swift 5.0.  Please use 'UnsafeMutableRawBufferPointer.initialize(from:)' instead")
  @discardableResult
  public func initializeMemory<C : Collection>(
    as type: C.Element.Type, from source: C
  ) -> UnsafeMutablePointer<C.Element> {
    // TODO: Optimize where `C` is a `ContiguousArrayBuffer`.
    // Initialize and bind each element of the container.
    var ptr = self
    for element in source {
      ptr.initializeMemory(as: C.Element.self, repeating: element, count: 1)
      ptr += MemoryLayout<C.Element>.stride
    }
    return UnsafeMutablePointer(_rawValue)
  }
}

extension UnsafeMutableRawBufferPointer {
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "allocate(byteCount:alignment:)")
  public static func allocate(count: Int) -> UnsafeMutableRawBufferPointer { 
    return UnsafeMutableRawBufferPointer.allocate(
      byteCount: count, alignment: MemoryLayout<UInt>.alignment)
  }

  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "copyMemory(from:)")
  public func copyBytes(from source: UnsafeRawBufferPointer) {
    copyMemory(from: source)
  }
}

//===----------------------------------------------------------------------===//
// The following overloads of flatMap are carefully crafted to allow the code
// like the following:
//   ["hello"].flatMap { $0 }
// return an array of strings without any type context in Swift 3 mode, at the
// same time allowing the following code snippet to compile:
//   [0, 1].flatMap { x in
//     if String(x) == "foo" { return "bar" } else { return nil }
//   }
// Note that the second overload is declared on a more specific protocol.
// See: test/stdlib/StringFlatMap.swift for tests.
extension Sequence {
  @available(swift, deprecated: 4.1/*, obsoleted: 5.0*/, renamed: "compactMap(_:)",
    message: "Please use compactMap(_:) for the case where closure returns an optional value")
  public func flatMap<ElementOfResult>(
    _ transform: (Element) throws -> ElementOfResult?
  ) rethrows -> [ElementOfResult] {
    return try _compactMap(transform)
  }

  @available(swift, obsoleted: 4)
  public func flatMap(
    _ transform: (Element) throws -> String
  ) rethrows -> [String] {
    return try map(transform)
  }
}

extension Collection {
  @available(swift, deprecated: 4.1, obsoleted: 5.0, renamed: "compactMap(_:)", message: "Please use compactMap(_:) for the case where closure returns an optional value")
  public func flatMap(
    _ transform: (Element) throws -> String?
  ) rethrows -> [String] {
    return try _compactMap(transform)
  }
}

extension String.Index {
  @available(swift, deprecated: 3.2, obsoleted: 4.0)
  public init(_position: Int) {
    self.init(encodedOffset: _position)
  }

  @available(swift, deprecated: 3.2, obsoleted: 4.0)
  public init(_codeUnitOffset: Int) {
    self.init(encodedOffset: _codeUnitOffset)
  }

  @available(swift, deprecated: 3.2, obsoleted: 4.0)
  public var _utf16Index: Int {
    return self.encodedOffset
  }

  @available(swift, deprecated: 3.2, obsoleted: 4.0)
  public var _offset: Int {
    return self.encodedOffset
  }
}

// backward compatibility for index interchange.
extension Optional where Wrapped == String.Index {
  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional indices")
  public static func ..<(
    lhs: String.Index?, rhs: String.Index?
  ) -> Range<String.Index> {
    return lhs! ..< rhs!
  }

  @available(swift, obsoleted: 4.0, message: "Any String view index conversion can fail in Swift 4; please unwrap the optional indices")
  public static func ...(
    lhs: String.Index?, rhs: String.Index?
  ) -> ClosedRange<String.Index> {
    return lhs! ... rhs!
  }
}

extension Zip2Sequence {
  @available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Sequence1.Iterator")
  public typealias Stream1 = Sequence1.Iterator
  @available(*, deprecated/*, obsoleted: 5.0*/, renamed: "Sequence2.Iterator")
  public typealias Stream2 = Sequence2.Iterator
}


//===--- QuickLooks -------------------------------------------------------===//

/// The sum of types that can be used as a Quick Look representation.
///
/// The `PlaygroundQuickLook` protocol is deprecated, and will be removed from
/// the standard library in a future Swift release. To customize the logging of
/// your type in a playground, conform to the
/// `CustomPlaygroundDisplayConvertible` protocol, which does not use the
/// `PlaygroundQuickLook` enum.
///
/// If you need to provide a customized playground representation in Swift 4.0
/// or Swift 3.2 or earlier, use a conditional compilation block:
///
///     #if swift(>=4.1) || (swift(>=3.3) && !swift(>=4.0))
///         // With Swift 4.1 and later (including Swift 3.3 and later), use
///         // the CustomPlaygroundDisplayConvertible protocol.
///     #else
///         // With Swift 4.0 and Swift 3.2 and earlier, use PlaygroundQuickLook
///         // and the CustomPlaygroundQuickLookable protocol.
///     #endif
@available(*, deprecated, message: "PlaygroundQuickLook will be removed in a future Swift version. For customizing how types are presented in playgrounds, use CustomPlaygroundDisplayConvertible instead.")
public typealias PlaygroundQuickLook = _PlaygroundQuickLook

@_frozen // rdar://problem/38719739 - needed by LLDB
public enum _PlaygroundQuickLook {
  case text(String)
  case int(Int64)
  case uInt(UInt64)
//  case float(Float32)
//  case double(Float64)
  case image(Any)
  case sound(Any)
  case color(Any)
  case bezierPath(Any)
  case attributedString(Any)
//  case rectangle(Float64, Float64, Float64, Float64)
//  case point(Float64, Float64)
//  case size(Float64, Float64)
  case bool(Bool)
  case range(Int64, Int64)
  case view(Any)
  case sprite(Any)
  case url(String)
  case _raw([UInt8], String)
}

// Maintain old `keys` and `values` types in Swift 3 mode.
extension Dictionary {
  /// A collection containing just the keys of the dictionary.
  ///
  /// When iterated over, keys appear in this collection in the same order as
  /// they occur in the dictionary's key-value pairs. Each key in the keys
  /// collection has a unique value.
  ///
  ///     let countryCodes = ["BR": "Brazil", "GH": "Ghana", "JP": "Japan"]
  ///     print(countryCodes)
  ///     // Prints "["BR": "Brazil", "JP": "Japan", "GH": "Ghana"]"
  ///
  ///     for k in countryCodes.keys {
  ///         print(k)
  ///     }
  ///     // Prints "BR"
  ///     // Prints "JP"
  ///     // Prints "GH"
  @available(swift, obsoleted: 4.0)
  public var keys: LazyMapCollection<[Key: Value], Key> {
    return self.lazy.map { $0.key }
  }

  /// A collection containing just the values of the dictionary.
  ///
  /// When iterated over, values appear in this collection in the same order as
  /// they occur in the dictionary's key-value pairs.
  ///
  ///     let countryCodes = ["BR": "Brazil", "GH": "Ghana", "JP": "Japan"]
  ///     print(countryCodes)
  ///     // Prints "["BR": "Brazil", "JP": "Japan", "GH": "Ghana"]"
  ///
  ///     for v in countryCodes.values {
  ///         print(v)
  ///     }
  ///     // Prints "Brazil"
  ///     // Prints "Japan"
  ///     // Prints "Ghana"
  @available(swift, obsoleted: 4.0)
  public var values: LazyMapCollection<[Key: Value], Value> {
    return self.lazy.map { $0.value }
  }

  @available(swift, obsoleted: 4.0)
  public func filter(
    _ isIncluded: (Element) throws -> Bool, obsoletedInSwift4: () = ()
  ) rethrows -> [Element] {
    var result: [Element] = []
    for x in self {
      if try isIncluded(x) {
        result.append(x)
      }
    }
    return result
  }
}

extension Set {
  @available(swift, obsoleted: 4.0)
  public func filter(
    _ isIncluded: (Element) throws -> Bool, obsoletedInSwift4: () = ()
  ) rethrows -> [Element] {
    var result: [Element] = []
    for x in self {
      if try isIncluded(x) {
        result.append(x)
      }
    }
    return result
  }  
}

extension _PlaygroundQuickLook {
  /// Creates a new Quick Look for the given instance.
  ///
  /// If the dynamic type of `subject` conforms to
  /// `CustomPlaygroundQuickLookable`, the result is found by calling its
  /// `customPlaygroundQuickLook` property. Otherwise, the result is
  /// synthesized by the language. In some cases, the synthesized result may
  /// be `.text(String(reflecting: subject))`.
  ///
  /// - Note: If the dynamic type of `subject` has value semantics, subsequent
  ///   mutations of `subject` will not observable in the Quick Look. In
  ///   general, though, the observability of such mutations is unspecified.
  ///
  /// - Parameter subject: The instance to represent with the resulting Quick
  ///   Look.
  @available(*, deprecated, message: "PlaygroundQuickLook will be removed in a future Swift version.")
  public init(reflecting subject: Any) {
    if let customized = subject as? CustomPlaygroundQuickLookable {
      self = customized.customPlaygroundQuickLook
    }
    else if let customized = subject as? _DefaultCustomPlaygroundQuickLookable {
      self = customized._defaultCustomPlaygroundQuickLook
    }
    else {
      if let q = Mirror.quickLookObject(subject) {
        self = q
      }
      else {
        self = .text(String(reflecting: subject))
      }
    }
  }
}

/// A type that explicitly supplies its own playground Quick Look.
///
/// The `CustomPlaygroundQuickLookable` protocol is deprecated, and will be
/// removed from the standard library in a future Swift release. To customize
/// the logging of your type in a playground, conform to the
/// `CustomPlaygroundDisplayConvertible` protocol.
///
/// If you need to provide a customized playground representation in Swift 4.0
/// or Swift 3.2 or earlier, use a conditional compilation block:
///
///     #if swift(>=4.1) || (swift(>=3.3) && !swift(>=4.0))
///         // With Swift 4.1 and later (including Swift 3.3 and later),
///         // conform to CustomPlaygroundDisplayConvertible.
///         extension MyType: CustomPlaygroundDisplayConvertible { /*...*/ }
///     #else
///         // Otherwise, on Swift 4.0 and Swift 3.2 and earlier,
///         // conform to CustomPlaygroundQuickLookable.
///         extension MyType: CustomPlaygroundQuickLookable { /*...*/ }
///     #endif
@available(*, deprecated/*, obsoleted: 5.0*/, message: "CustomPlaygroundQuickLookable will be removed in a future Swift version. For customizing how types are presented in playgrounds, use CustomPlaygroundDisplayConvertible instead.")
public typealias CustomPlaygroundQuickLookable = _CustomPlaygroundQuickLookable

public protocol _CustomPlaygroundQuickLookable {
  /// A custom playground Quick Look for this instance.
  ///
  /// If this type has value semantics, the `PlaygroundQuickLook` instance
  /// should be unaffected by subsequent mutations.
  var customPlaygroundQuickLook: _PlaygroundQuickLook { get }
}

// Double-underscored real version allows us to keep using this in AppKit while
// warning for non-SDK use. This is probably overkill but it doesn't cost
// anything.
@available(*, deprecated/*, obsoleted: 5.0*/, message: "_DefaultCustomPlaygroundQuickLookable will be removed in a future Swift version. For customizing how types are presented in playgrounds, use CustomPlaygroundDisplayConvertible instead.")
public typealias _DefaultCustomPlaygroundQuickLookable = __DefaultCustomPlaygroundQuickLookable

public protocol __DefaultCustomPlaygroundQuickLookable {
  var _defaultCustomPlaygroundQuickLook: _PlaygroundQuickLook { get }
}
