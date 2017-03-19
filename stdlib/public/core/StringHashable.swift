//===----------------------------------------------------------------------===//
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

import SwiftShims

#if _runtime(_ObjC)
@_inlineable // FIXME(sil-serialize-all)
@_versioned // FIXME(sil-serialize-all)
@_silgen_name("swift_stdlib_NSStringHashValue")
internal func _stdlib_NSStringHashValue(
  _ str: AnyObject, _ isASCII: Bool) -> Int

@_inlineable // FIXME(sil-serialize-all)
@_versioned // FIXME(sil-serialize-all)
@_silgen_name("swift_stdlib_NSStringHashValuePointer")
internal func _stdlib_NSStringHashValuePointer(
  _ str: OpaquePointer, _ isASCII: Bool) -> Int

@_inlineable // FIXME(sil-serialize-all)
@_versioned // FIXME(sil-serialize-all)
@_silgen_name("swift_stdlib_CFStringHashCString")
internal func _stdlib_CFStringHashCString(
  _ str: OpaquePointer, _ len: Int) -> Int
#endif

fileprivate let collationTable: [UInt32] = [
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x03040505, 0x03060505, 0x03080505,
    0x030a0505, 0x030c0505, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x04000505, 0x07500505, 0x09760505, 0x0a8e0505,
    0x0d260505, 0x0a900505, 0x0a8a0505, 0x09680505,
    0x098c0505, 0x098e0505, 0x0a7c0505, 0x0c630505,
    0x06000505, 0x050e0505, 0x08000505, 0x0a860505,
    0x12000505, 0x14000505, 0x16000505, 0x18000505,
    0x1a000505, 0x1c000505, 0x1e000505, 0x20000505,
    0x22000505, 0x24000505, 0x072c0505, 0x07220505,
    0x0c6b0505, 0x0c6d0505, 0x0c6f0505, 0x07580505,
    0x0a7a0505, 0x2900059c, 0x2b00059c, 0x2d00059c,
    0x2f00059c, 0x3100059c, 0x3300059c, 0x3500059c,
    0x3700059c, 0x3900059c, 0x3b00059c, 0x3d00059c,
    0x3f00059c, 0x4100059c, 0x4300059c, 0x4500059c,
    0x4700059c, 0x490005a0, 0x4b00059c, 0x4d00059c,
    0x4f00059c, 0x5100059c, 0x5300059c, 0x550005a0,
    0x5700059c, 0x5900059c, 0x5b00059c, 0x09900505,
    0x0a880505, 0x09920505, 0x0c0a0505, 0x050a0505,
    0x0c040505, 0x29000505, 0x2b000505, 0x2d000505,
    0x2f000505, 0x31000505, 0x33000505, 0x35000505,
    0x37000505, 0x39000505, 0x3b000505, 0x3d000505,
    0x3f000505, 0x41000505, 0x43000505, 0x45000505,
    0x47000505, 0x49000505, 0x4b000505, 0x4d000505,
    0x4f000505, 0x51000505, 0x53000505, 0x55000505,
    0x57000505, 0x59000505, 0x5b000505, 0x09940505,
    0x0c730505, 0x09960505, 0x0c770505, 0x00000000,
]

extension Unicode {
  // FIXME: cannot be marked @_versioned. See <rdar://problem/34438258>
  // @_inlineable // FIXME(sil-serialize-all)
  // @_versioned // FIXME(sil-serialize-all)
  internal static func hashASCII(
    _ string: UnsafeBufferPointer<UInt8>
  ) -> Int {
    //let collationTable = _swift_stdlib_unicode_getASCIICollationTable()
    var hasher = _SipHash13Context(key: _Hashing.secretKey)
    for c in string {
      _precondition(c <= 127)
      let element = collationTable[Int(c)]
      // Ignore zero valued collation elements. They don't participate in the
      // ordering relation.
      if element != 0 {
        hasher.append(element)
      }
    }
    return hasher._finalizeAndReturnIntHash()
  }

  // FIXME: cannot be marked @_versioned. See <rdar://problem/34438258>
  // @_inlineable // FIXME(sil-serialize-all)
  // @_versioned // FIXME(sil-serialize-all)
#if !KERNELLIB
  internal static func hashUTF16(
    _ string: UnsafeBufferPointer<UInt16>
  ) -> Int {
    let collationIterator = _swift_stdlib_unicodeCollationIterator_create(
      string.baseAddress!,
      UInt32(string.count))
    defer { _swift_stdlib_unicodeCollationIterator_delete(collationIterator) }

    var hasher = _SipHash13Context(key: _Hashing.secretKey)
    while true {
      var hitEnd = false
      let element =
        _swift_stdlib_unicodeCollationIterator_next(collationIterator, &hitEnd)
      if hitEnd {
        break
      }
      // Ignore zero valued collation elements. They don't participate in the
      // ordering relation.
      if element != 0 {
        hasher.append(element)
      }
    }
    return hasher._finalizeAndReturnIntHash()
  }
#endif // !KERNELLIB
}

@_versioned // FIXME(sil-serialize-all)
@inline(never) // Hide the CF dependency
internal func _hashString(_ string: String) -> Int {
  let core = string._core
#if _runtime(_ObjC)
    // Mix random bits into NSString's hash so that clients don't rely on
    // Swift.String.hashValue and NSString.hash being the same.
#if arch(i386) || arch(arm)
    let hashOffset = Int(bitPattern: 0x88dd_cc21)
#else
    let hashOffset = Int(bitPattern: 0x429b_1266_88dd_cc21)
#endif
  // If we have a contiguous string then we can use the stack optimization.
  let isASCII = core.isASCII
  if core.hasContiguousStorage {
    if isASCII {
      return hashOffset ^ _stdlib_CFStringHashCString(
                              OpaquePointer(core.startASCII), core.count)
    } else {
      let stackAllocated = _NSContiguousString(core)
      return hashOffset ^ stackAllocated._unsafeWithNotEscapedSelfPointer {
        return _stdlib_NSStringHashValuePointer($0, false)
      }
    }
  } else {
    let cocoaString = unsafeBitCast(
      string._bridgeToObjectiveCImpl(), to: _NSStringCore.self)
    return hashOffset ^ _stdlib_NSStringHashValue(cocoaString, isASCII)
  }
#else
  if let asciiBuffer = core.asciiBuffer {
    return Unicode.hashASCII(UnsafeBufferPointer(
      start: asciiBuffer.baseAddress!,
      count: asciiBuffer.count))
  } else {
    fatalError("Cant hash a non-ASCII string")
//    return Unicode.hashUTF16(
//      UnsafeBufferPointer(start: core.startUTF16, count: core.count))
  }
#endif
}


extension String : Hashable {
  /// The string's hash value.
  ///
  /// Hash values are not guaranteed to be equal across different executions of
  /// your program. Do not save hash values to use during a future execution.
  @_inlineable // FIXME(sil-serialize-all)
  public var hashValue: Int {
    return _hashString(self)
  }
}

