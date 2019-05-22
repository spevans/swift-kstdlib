//===--- LibcShims.h - Access to POSIX for Swift's core stdlib --*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2019 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  Using the Darwin (or Glibc) module in the core stdlib would create a
//  circular dependency, so instead we import these declarations as part of
//  SwiftShims.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_STDLIB_SHIMS_LIBCSHIMS_H
#define SWIFT_STDLIB_SHIMS_LIBCSHIMS_H

#include <stdio.h>
#include <stdint.h>
#include "SwiftStdint.h"
#include "SwiftStddef.h"
#include "Visibility.h"

#if __has_feature(nullability)
#pragma clang assume_nonnull begin
#endif

#ifdef __cplusplus
namespace swift { extern "C" {
#endif

// This declaration might not be universally correct.
// We verify its correctness for the current platform in the runtime code.
#if defined(__linux__)
# if defined(__ANDROID__) && !(defined(__aarch64__) || defined(__x86_64__))
typedef __swift_uint16_t __swift_mode_t;
# else
typedef __swift_uint32_t __swift_mode_t;
# endif
#elif defined(__APPLE__)
typedef __swift_uint16_t __swift_mode_t;
#elif defined(_WIN32)
typedef __swift_int32_t __swift_mode_t;
#else  // just guessing
typedef __swift_uint16_t __swift_mode_t;
#endif


// Input/output <stdio.h>
SWIFT_RUNTIME_STDLIB_INTERNAL
int _swift_stdlib_putchar_unlocked(int c);
SWIFT_RUNTIME_STDLIB_INTERNAL
__swift_size_t _swift_stdlib_fwrite_stdout(const void *ptr, __swift_size_t size,
                                           __swift_size_t nitems);

// General utilities <stdlib.h>
// Memory management functions
static inline void _swift_stdlib_free(void *_Nullable ptr) {
  extern void free(void * _Nullable ptr);
  free(ptr);
}

// <unistd.h>
SWIFT_RUNTIME_STDLIB_SPI
__swift_ssize_t _swift_stdlib_read(int fd, void *buf, __swift_size_t nbyte);
SWIFT_RUNTIME_STDLIB_SPI
__swift_ssize_t _swift_stdlib_write(int fd, const void *buf, __swift_size_t nbyte);
SWIFT_RUNTIME_STDLIB_SPI
int _swift_stdlib_close(int fd);

// String handling <string.h>
SWIFT_READONLY
static inline __swift_size_t _swift_stdlib_strlen(const char *s) {
  extern __swift_size_t strlen(const char *);
  return strlen(s);
}

SWIFT_READONLY
static inline __swift_size_t _swift_stdlib_strlen_unsigned(const unsigned char *s) {
  return _swift_stdlib_strlen((const char *)s);
}

SWIFT_READONLY
static inline int _swift_stdlib_memcmp(const void *s1, const void *s2,
                                       __swift_size_t n) {
  extern int memcmp(const void *, const void *, __swift_size_t);
  return memcmp(s1, s2, n);
}

// Casting helper. This code needs to work when included from C or C++.
// Casting away const with a C-style cast warns in C++. Use a const_cast
// there.
#ifdef __cplusplus
#define CONST_CAST(type, value) const_cast<type>(value)
#else
#define CONST_CAST(type, value) (type)value
#endif

// Non-standard extensions
#if defined(__APPLE__)
static inline __swift_size_t _swift_stdlib_malloc_size(const void *ptr) {
  extern __swift_size_t malloc_size(const void *);
  return malloc_size(ptr);
}
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__ANDROID__) \
   || defined(__HAIKU__) || defined(__FreeBSD__)
static inline __swift_size_t _swift_stdlib_malloc_size(const void *ptr) {
#if defined(__ANDROID__)
#if __ANDROID_API__ >= 17
  extern __swift_size_t malloc_usable_size(const void *ptr);
#endif
#else
  extern __swift_size_t malloc_usable_size(void * _Nullable ptr);
#endif
  return malloc_usable_size(CONST_CAST(void *, ptr));
}
#elif defined(_WIN32)
static inline __swift_size_t _swift_stdlib_malloc_size(const void *ptr) {
  extern __swift_size_t _msize(void *ptr);
  return _msize(CONST_CAST(void *, ptr));
}
#else
#error No malloc_size analog known for this platform/libc.
#endif

// Math library functions
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_remainderf(float _self, float other) {
  return __builtin_remainderf(_self, other);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_squareRootf(float _self) {
  return __builtin_sqrtf(_self);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_tanf(float x) {
  return __builtin_tanf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_acosf(float x) {
  return __builtin_acosf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_asinf(float x) {
  return __builtin_asinf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_atanf(float x) {
  return __builtin_atanf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_atan2f(float y, float x) {
  return __builtin_atan2f(y, x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_coshf(float x) {
  return __builtin_coshf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_sinhf(float x) {
  return __builtin_sinhf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_tanhf(float x) {
  return __builtin_tanhf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_acoshf(float x) {
  return __builtin_acoshf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_asinhf(float x) {
  return __builtin_asinhf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_atanhf(float x) {
  return __builtin_atanhf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_exp10f(float x) {
#if defined __APPLE__
  extern float __exp10f(float);
  return __exp10f(x);
#else
  return __builtin_powf(10, x);
#endif
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_expm1f(float x) {
  return __builtin_expm1f(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_log1pf(float x) {
  return __builtin_log1pf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_hypotf(float x, float y) {
#if defined(_WIN32)
  extern float _hypotf(float, float);
  return _hypotf(x, y);
#else
  return __builtin_hypotf(x, y);
#endif
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_erff(float x) {
  return __builtin_erff(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_erfcf(float x) {
  return __builtin_erfcf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_tgammaf(float x) {
  return __builtin_tgammaf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
float _swift_stdlib_lgammaf(float x) {
  extern float lgammaf_r(float x, int *psigngam);
  int dontCare;
  return lgammaf_r(x, &dontCare);
}

static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_remainder(double _self, double other) {
  return __builtin_remainder(_self, other);
}

static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_squareRoot(double _self) {
  return __builtin_sqrt(_self);
}

static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_tan(double x) {
  return __builtin_tan(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_acos(double x) {
  return __builtin_acos(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_asin(double x) {
  return __builtin_asin(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_atan(double x) {
  return __builtin_atan(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_atan2(double y, double x) {
  return __builtin_atan2(y, x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_cosh(double x) {
  return __builtin_cosh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_sinh(double x) {
  return __builtin_sinh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_tanh(double x) {
  return __builtin_tanh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_acosh(double x) {
  return __builtin_acosh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_asinh(double x) {
  return __builtin_asinh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_atanh(double x) {
  return __builtin_atanh(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_exp10(double x) {
#if defined __APPLE__
  extern double __exp10(double);
  return __exp10(x);
#else
  return __builtin_pow(10, x);
#endif
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_expm1(double x) {
  return __builtin_expm1(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_log1p(double x) {
  return __builtin_log1p(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_hypot(double x, double y) {
  return __builtin_hypot(x, y);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_erf(double x) {
  return __builtin_erf(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_erfc(double x) {
  return __builtin_erfc(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_tgamma(double x) {
  return __builtin_tgamma(x);
}
  
static inline SWIFT_ALWAYS_INLINE
double _swift_stdlib_lgamma(double x) {
  extern double lgamma_r(double x, int *psigngam);
  int dontCare;
  return lgamma_r(x, &dontCare);
}

#if !defined _WIN32 && (defined __i386__ || defined __x86_64__)
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_remainderl(long double _self, long double other) {
  return __builtin_remainderl(_self, other);
}

static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_squareRootl(long double _self) {
  return __builtin_sqrtl(_self);
}

static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_tanl(long double x) {
  return __builtin_tanl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_acosl(long double x) {
  return __builtin_acosl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_asinl(long double x) {
  return __builtin_asinl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_atanl(long double x) {
  return __builtin_atanl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_atan2l(long double y, long double x) {
  return __builtin_atan2l(y, x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_coshl(long double x) {
  return __builtin_coshl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_sinhl(long double x) {
  return __builtin_sinhl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_tanhl(long double x) {
  return __builtin_tanhl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_acoshl(long double x) {
  return __builtin_acoshl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_asinhl(long double x) {
  return __builtin_asinhl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_atanhl(long double x) {
  return __builtin_atanhl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_exp10l(long double x) {
  return __builtin_powl(10, x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_expm1l(long double x) {
  return __builtin_expm1l(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_log1pl(long double x) {
  return __builtin_log1pl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_hypotl(long double x, long double y) {
  return __builtin_hypotl(x, y);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_erfl(long double x) {
  return __builtin_erfl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_erfcl(long double x) {
  return __builtin_erfcl(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_tgammal(long double x) {
  return __builtin_tgammal(x);
}
  
static inline SWIFT_ALWAYS_INLINE
long double _swift_stdlib_lgammal(long double x) {
  extern long double lgammal_r(long double x, int *psigngam);
  int dontCare;
  return lgammal_r(x, &dontCare);
}
#endif

#ifdef __cplusplus
}} // extern "C", namespace swift
#endif

#if __has_feature(nullability)
#pragma clang assume_nonnull end
#endif


void debugger_hook(void);

#define STRING_DEBUG 0
#define OTHER_DEBUG 0

static inline void _string_debug1(int count, uint64_t discriminator,
                                  uint16_t flags) {
#if STRING_DEBUG
        fprintf(stderr, "_StringObject.init count: %d\tdiscriminator: %16.16lx\tflags: %hx",
                count, discriminator, flags);
#endif
}


static inline void _string_debug2() {
#if STRING_DEBUG
        fprintf(stderr, "CountAndFlags._invariantCheck(): isASCII\n");
#endif
}


static inline void _string_debug3() {
#if STRING_DEBUG
        fprintf(stderr, "CountAndFlags._invariantCheck(): isNativelyStored\n");
        debugger_hook();
#endif
}


static inline void _string_debug4(const unsigned char * _Nullable ptr,
                                  long count, int64_t isASCII) {
#if STRING_DEBUG
        fprintf(stderr, "String.init(_builtinStringLiteral start: %p\tutf8CodeUnitCount: %ld\tisASCII: %ld\n",
                ptr, count, isASCII);
#endif
}


static inline void _string_debug5() {
#if STRING_DEBUG
        fprintf(stderr, "getSharedUTF8Start\n");
#endif
}

static inline void _string_debug6(uint64_t discriminatorbits) {
#if STRING_DEBUG
        fprintf(stderr, "discriminatorbits: %16.16lx\n", discriminatorbits);
        debugger_hook();
#endif
}


static inline void _string_debug7(int flag) {
#if STRING_DEBUG
        fprintf(stderr, "Flag: %d\n", flag);
#endif
}


static inline void _string_debug8(const unsigned char * _Nullable ptr,
                                  int isASCII) {
#if STRING_DEBUG
        fprintf(stderr, "_StringObject(immortal bufPtr: %p isASCII: %d\n",
                ptr, isASCII);
#endif
}


static inline void _string_debug9(uint64_t ptr, uint64_t discriminator) {
#if STRING_DEBUG
        fprintf(stderr, "StringObject(pointerBits: %16.16lx discriminator: %16.16lx)\n",
                ptr, discriminator);
#endif
}


static inline void _string_debug10(const unsigned char * _Nullable ptr,
                                   long count, long capacity, int isASCII) {
#if STRING_DEBUG
        fprintf(stderr, "StringStorage.create(initializingFrom bufPtr: %p count: %ld capacity: %ld isASCII: %d\n",
                ptr, count, capacity, isASCII);
#endif
}


static inline void _string_debug11(int message) {
#if STRING_DEBUG
        switch (message) {
        case 0: fprintf(stderr, "StringStorage.create checking invariant\n"); break;
        case 1: fprintf(stderr, "StringStorage.create invariant checked\n"); break;
        case 2: fprintf(stderr, "StringStorage.create initializing mutable start\n"); break;
        case 3: fprintf(stderr, "StringStorage.create 2nd invariant check\n"); break;                
        case 4: fprintf(stderr, "StringStorage.create finished\n"); break;
        case 5: fprintf(stderr, "init1 finished\n"); break;
        case 6: fprintf(stderr, "init2 finished\n"); break;
        case 7: fprintf(stderr, "Full check finished\n"); break;
        case 8: fprintf(stderr, "append(_ other: _StringGuts)\n"); break;
        case 9: fprintf(stderr, "append(_ slicedOther: _StringGutsSlice)\n"); break;
        case 10: fprintf(stderr, "StringRangeReplaceableCollection append(_ other String)\n"); break;
        case 11: fprintf(stderr, "StringRangeReplaceableCollection append(_ c: Character)\n"); break;
        case 12: fprintf(stderr, "self = other\n"); break;
        case 13: fprintf(stderr, "self._guts.append(other._guts)\n"); break;
        }
#endif
}

static inline void _string_debug12(uint64_t rawBits) {
#if STRING_DEBUG
        fprintf(stderr, "rawBits: %16.16lx\n", rawBits);
#endif
}


static inline void _string_debug13(int countOK, int asciiOK, int nfcOK,
                                   int nativelyStoredOK, int tailAllocatedOK) {
#if STRING_DEBUG
        fprintf(stderr, "countOK: %d asciiOK: %d nfcOK: %d nativelyStoredOK: %d tailAllocatedOK: %d\n",
                countOK, asciiOK, nfcOK, nativelyStoredOK, tailAllocatedOK);
#endif
}


static inline void _str_object_get(uint64_t ptr) {
#if STRING_DEBUG
        fprintf(stderr, "Getting _object: %16.16lx\n", ptr);
#endif
}

static inline void _str_object_set(uint64_t ptr) {
#if STRING_DEBUG
        fprintf(stderr, "Setting _object: %16.16lx\n", ptr);
#endif
}


static inline void _string_debug14(uint64_t bridgeObject, uint64_t countAndFlags) {
#if STRING_DEBUG
        fprintf(stderr, "StringObject.init(bridgeObject: %16.16lx, countAndFlags: %16.16lx\n",
                bridgeObject, countAndFlags);
#endif
}


static inline void _string_count_and_flags(
        long count, int isASCII, int isNFC, int isNativelyStored,
        int isTailAllocated, uint64_t rawBits, int rawBitsValid)
{
#if STRING_DEBUG
        fprintf(stderr, "CountAndFlags init count: %ld isASCII: %d isNFC: %d\n",
                count, isASCII, isNFC);
        fprintf(stderr, "isNativelyStored: %d isTailAllocated: %d rawBits: %16.16lx\n",
                isNativelyStored, isTailAllocated, rawBits);
        fprintf(stderr, "rawBitsValid: %d\n", rawBitsValid);
#endif
}


static inline void _debug_large_address_bits(
        unsigned long bits) {
#if STRING_DEBUG
        fprintf(stderr, "largeAddressBits: %16.16lx\n", bits);
#endif
}


static inline void _string_storage_check(
        const void * _Nonnull rawSelf,
        const void * _Nonnull rawStart,
        long unusedCapacity,
        long count,
        long _realCapacity,
        int nulTerminated,
        int isPreferredRepresentation,
        int allAsciiOK,
        int isNativelyStored,
        int isTailAllocated)
{
#if STRING_DEBUG
        fprintf(stderr, "StringStorage._invariantCheck rawSelf: %p rawStart: %p\n",
                rawSelf, rawStart);
        fprintf(stderr, " unusedCapacity: %ld count: %ld _realCapacity: %ld\n",
                unusedCapacity, count, _realCapacity);
        fprintf(stderr, "nulTerminated: %d isPreferredRepresentation: %d\n",
                nulTerminated, isPreferredRepresentation);
        fprintf(stderr, "allAsciiOK: %d isNativelyStored: %d isTailAllocated: %d\n",
                allAsciiOK, isNativelyStored, isTailAllocated);        
#endif
}

static inline void _string_invariant_check(
        uint64_t discriminator,
        int isSmall,
        int isImmortal,
        long smallCount,
        long largeCount,
        long count,
        int hasObjcBridgeableObject,
        int providesFastUTF8,
        int isLarge,
        int largeIsCocoa
        
        ) {
#if STRING_DEBUG
        fprintf(stderr, "Full invariant check discriminator: %16.16lx\n",
                discriminator);
        fprintf(stderr, "isSmall: %d isLarge: %d isImmortal: %d count: %ld smallCount: %ld largeCount: %ld\n",
                isSmall, isLarge, isImmortal, count, smallCount, largeCount);
        fprintf(stderr, "hasObjcBO: %d providesFastUTF8: %d largeIsCocoa: %d\n",
                hasObjcBridgeableObject, providesFastUTF8, largeIsCocoa);
        debugger_hook();
#endif
}



static inline void _klibc_print(const char * _Nonnull string) {
#if OTHER_DEBUG
        fprintf(stderr, "%s", string);
#endif
}

static inline void _klibc_print_uint64(unsigned long long number) {
#if OTHER_DEBUG
        fprintf(stderr, "%16.16llx\n", number);
#endif
}

static inline void _klibc_print2(const char * _Nonnull string,
                                 unsigned long long number) {
#if OTHER_DEBUG
        fprintf(stderr, "%s %16.16llx\n", string, number);
#endif
}


static inline void _klibc_print3(const char * _Nonnull string,
                                 long number) {
#if OTHER_DEBUG
        fprintf(stderr, "%s %ld\n", string, number);
#endif
}

static inline void _klibc_print4(const char * _Nonnull string,
                                 unsigned long number) {
#if OTHER_DEBUG
        fprintf(stderr, "%s %lu\n", string, number);
#endif
}

#endif // SWIFT_STDLIB_SHIMS_LIBCSHIMS_H
