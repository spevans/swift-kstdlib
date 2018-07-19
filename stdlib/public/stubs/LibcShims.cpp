//===--- LibcShims.cpp ----------------------------------------------------===//
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

#if defined(__APPLE__)
#define _REENTRANT
#include <math.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib, "Bcrypt.lib")
#else
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <cmath>
#include <errno.h>
#include <fcntl.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __has_include(<sys/random.h>)
#include <sys/random.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <type_traits>

#include "llvm/Support/DataTypes.h"
#include "swift/Basic/Lazy.h"
#include "swift/Runtime/Config.h"
#include "swift/Runtime/Debug.h"
#include "swift/Runtime/Mutex.h"
#include "../SwiftShims/LibcShims.h"

using namespace swift;

static_assert(std::is_same<ssize_t, swift::__swift_ssize_t>::value,
              "__swift_ssize_t must be defined as equivalent to ssize_t in LibcShims.h");
#if !defined(_WIN32) || defined(__CYGWIN__)
static_assert(std::is_same<mode_t, swift::__swift_mode_t>::value,
              "__swift_mode_t must be defined as equivalent to mode_t in LibcShims.h");
#endif

SWIFT_RUNTIME_STDLIB_INTERFACE
void swift::_stdlib_free(void *ptr) {
  free(ptr);
}

#if !KERNELLIB
SWIFT_RUNTIME_STDLIB_INTERFACE
int swift::_stdlib_putchar_unlocked(int c) {
#if defined(_WIN32)
  return _putc_nolock(c, stdout);
#else
  return putchar_unlocked(c);
#endif
}

SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_size_t swift::_stdlib_fwrite_stdout(const void *ptr,
                                         __swift_size_t size,
                                         __swift_size_t nitems) {
    return fwrite(ptr, size, nitems, stdout);
}
#endif // !KERNELLIB


SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_size_t swift::_stdlib_strlen(const char *s) {
    return strlen(s);
}

SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_size_t swift::_stdlib_strlen_unsigned(const unsigned char *s) {
  return strlen(reinterpret_cast<const char *>(s));
}

SWIFT_RUNTIME_STDLIB_INTERFACE
int swift::_stdlib_memcmp(const void *s1, const void *s2,
                       __swift_size_t n) {
  return memcmp(s1, s2, n);
}

#if !KERNELLIB
SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_ssize_t
swift::_stdlib_read(int fd, void *buf, __swift_size_t nbyte) {
#if defined(_WIN32)
  return _read(fd, buf, nbyte);
#else
  return read(fd, buf, nbyte);
#endif
}

SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_ssize_t
swift::_stdlib_write(int fd, const void *buf, __swift_size_t nbyte) {
#if defined(_WIN32)
  return _write(fd, buf, nbyte);
#else
  return write(fd, buf, nbyte);
#endif
}

SWIFT_RUNTIME_STDLIB_INTERFACE
int swift::_stdlib_close(int fd) {
#if defined(_WIN32)
  return _close(fd);
#else
  return close(fd);
#endif
}


#if defined(_WIN32) && !defined(__CYGWIN__)
// Windows

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_open(const char *path, int oflag, __swift_mode_t mode) {
  return _open(path, oflag, static_cast<int>(mode));
}

#else
// not Windows

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_open(const char *path, int oflag, __swift_mode_t mode) {
  return open(path, oflag, mode);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_openat(int fd, const char *path, int oflag,
                          __swift_mode_t mode) {
  return openat(fd, path, oflag, mode);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
void *swift::_stdlib_sem_open2(const char *name, int oflag) {
  return sem_open(name, oflag);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
void *swift::_stdlib_sem_open4(const char *name, int oflag,
                               __swift_mode_t mode, unsigned int value) {
  return sem_open(name, oflag, mode, value);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_fcntl(int fd, int cmd, int value) {
  return fcntl(fd, cmd, value);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_fcntlPtr(int fd, int cmd, void* ptr) {
  return fcntl(fd, cmd, ptr);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_ioctl(int fd, unsigned long int request, int value) {
  return ioctl(fd, request, value);
}

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_ioctlPtr(int fd, unsigned long int request, void* ptr) {
  return ioctl(fd, request, ptr);
}

#if defined(__FreeBSD__)
SWIFT_RUNTIME_STDLIB_INTERNAL
char * _Nullable * _Null_unspecified swift::_stdlib_getEnviron() {
  extern char **environ;
  return environ;
}
#elif defined(__APPLE__)
SWIFT_RUNTIME_STDLIB_INTERNAL
char * _Nullable *swift::_stdlib_getEnviron() {
  extern char * _Nullable **_NSGetEnviron(void);
  return *_NSGetEnviron();
}
#endif

#endif // !(defined(_WIN32) && !defined(__CYGWIN__))

SWIFT_RUNTIME_STDLIB_INTERNAL
int swift::_stdlib_getErrno() {
  return errno;
}

SWIFT_RUNTIME_STDLIB_INTERNAL
void swift::_stdlib_setErrno(int value) {
  errno = value;
}

#endif  // KERNELLIB

#if defined(_WIN32)
static_assert(std::is_same<__swift_thread_key_t, DWORD>::value,
              "__swift_thread_key_t is not a DWORD");

SWIFT_CC(swift) SWIFT_RUNTIME_STDLIB_INTERFACE
void _stdlib_destroyTLS(void *);

static void
#if defined(_M_IX86)
__stdcall
#endif
destroyTLS_CCAdjustmentThunk(void *ptr) {
  _stdlib_destroyTLS(ptr);
}

SWIFT_RUNTIME_STDLIB_INTERFACE
int
swift::_stdlib_thread_key_create(__swift_thread_key_t * _Nonnull key,
                              void (* _Nullable destructor)(void *)) {
  *key = FlsAlloc(destroyTLS_CCAdjustmentThunk);
  return *key != FLS_OUT_OF_INDEXES;
}

SWIFT_RUNTIME_STDLIB_INTERFACE
void * _Nullable
swift::_stdlib_thread_getspecific(__swift_thread_key_t key) {
  return FlsGetValue(key);
}

SWIFT_RUNTIME_STDLIB_INTERFACE
int swift::_stdlib_thread_setspecific(__swift_thread_key_t key,
                                   const void * _Nullable value) {
  return FlsSetValue(key, const_cast<void *>(value)) == TRUE;
}
#else
// Guard compilation on the typedef for __swift_thread_key_t in LibcShims.h
// being identical to the platform's pthread_key_t
static_assert(std::is_same<__swift_thread_key_t, pthread_key_t>::value,
              "This platform's pthread_key_t differs. If you hit this assert, "
              "fix __swift_pthread_key_t's typedef in LibcShims.h by adding an "
              "#if guard and definition for your platform");

SWIFT_RUNTIME_STDLIB_INTERFACE
int
swift::_stdlib_thread_key_create(__swift_thread_key_t * _Nonnull key,
                              void (* _Nullable destructor)(void *)) {
  return pthread_key_create(key, destructor);
}

SWIFT_RUNTIME_STDLIB_INTERFACE
void * _Nullable
swift::_stdlib_thread_getspecific(__swift_thread_key_t key) {
  return pthread_getspecific(key);
}

SWIFT_RUNTIME_STDLIB_INTERFACE
int swift::_stdlib_thread_setspecific(__swift_thread_key_t key,
                                      const void * _Nullable value) {
  return pthread_setspecific(key, value);
}
#endif

#if defined(__APPLE__)
#include <malloc/malloc.h>
SWIFT_RUNTIME_STDLIB_INTERFACE
size_t swift::_stdlib_malloc_size(const void *ptr) {
  return malloc_size(ptr);
}
#elif defined(__GNU_LIBRARY__) || defined(__CYGWIN__) || defined(__ANDROID__) || defined(__HAIKU__)
#if defined(__HAIKU__)
#define _GNU_SOURCE
#endif
#include <malloc.h>
SWIFT_RUNTIME_STDLIB_INTERFACE
size_t swift::_stdlib_malloc_size(const void *ptr) {
  return malloc_usable_size(const_cast<void *>(ptr));
}
#elif defined(_WIN32)
#include <malloc.h>
SWIFT_RUNTIME_STDLIB_INTERFACE
size_t swift::_stdlib_malloc_size(const void *ptr) {
  return _msize(const_cast<void *>(ptr));
}
#elif defined(__FreeBSD__)
#include <malloc_np.h>
SWIFT_RUNTIME_STDLIB_INTERFACE
size_t swift::_stdlib_malloc_size(const void *ptr) {
  return malloc_usable_size(const_cast<void *>(ptr));
}
#else
#error No malloc_size analog known for this platform/libc.
#endif

#if KERNELLIB // KERNNELIB has not support for _stdlib_random

SWIFT_RUNTIME_STDLIB_INTERNAL
void swift::_stdlib_random(void *buf, __swift_size_t nbytes) {
        // TODO: some sort of random number thing
}

#else // !KERNELLIB

static Lazy<std::mt19937> theGlobalMT19937;

static std::mt19937 &getGlobalMT19937() {
  return theGlobalMT19937.get();
}

SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_uint32_t swift::_stdlib_cxx11_mt19937() {
  return getGlobalMT19937()();
}

SWIFT_RUNTIME_STDLIB_INTERFACE
__swift_uint32_t
swift::_stdlib_cxx11_mt19937_uniform(__swift_uint32_t upper_bound) {
  if (upper_bound > 0)
    upper_bound--;
  std::uniform_int_distribution<__swift_uint32_t> RandomUniform(0, upper_bound);
  return RandomUniform(getGlobalMT19937());
}

// _stdlib_random
//
// Should the implementation of this function add a new platform/change for a
// platform, make sure to also update the documentation regarding platform
// implementation of this function.
// This can be found at: /docs/Random.md

#if defined(__APPLE__)

SWIFT_RUNTIME_STDLIB_INTERNAL
void swift::_stdlib_random(void *buf, __swift_size_t nbytes) {
  arc4random_buf(buf, nbytes);
}

#elif defined(_WIN32) && !defined(__CYGWIN__)
#warning TODO: Test _stdlib_random on Windows

SWIFT_RUNTIME_STDLIB_INTERNAL
void swift::_stdlib_random(void *buf, __swift_size_t nbytes) {
  NTSTATUS status = BCryptGenRandom(nullptr,
                                    static_cast<PUCHAR>(buf),
                                    static_cast<ULONG>(nbytes),
                                    BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  if (!BCRYPT_SUCCESS(status)) {
    fatalError(0, "Fatal error: 0x%.8X in '%s'\n", status, __func__);
  }
}

#else

#undef  WHILE_EINTR
#define WHILE_EINTR(expression) ({                                             \
  decltype(expression) result = -1;                                            \
  do { result = (expression); } while (result == -1 && errno == EINTR);        \
  result;                                                                      \
})

SWIFT_RUNTIME_STDLIB_INTERNAL
void swift::_stdlib_random(void *buf, __swift_size_t nbytes) {
  while (nbytes > 0) {
    __swift_ssize_t actual_nbytes = -1;
#if defined(GRND_RANDOM)
    static const bool getrandom_available =
      !(getrandom(nullptr, 0, 0) == -1 && errno == ENOSYS);
    if (getrandom_available) {
      actual_nbytes = WHILE_EINTR(getrandom(buf, nbytes, 0));
    }
#elif defined(__Fuchsia__)
    __swift_size_t getentropy_nbytes = std::min(nbytes, __swift_size_t{256});
    if (0 == getentropy(buf, getentropy_nbytes)) {
      actual_nbytes = getentropy_nbytes;
    }
#endif
    if (actual_nbytes == -1) {
      static const int fd =
        WHILE_EINTR(_stdlib_open("/dev/urandom", O_RDONLY | O_CLOEXEC, 0));
      if (fd != -1) {
        static StaticMutex mutex;
        mutex.withLock([&] {
          actual_nbytes = WHILE_EINTR(_stdlib_read(fd, buf, nbytes));
        });
      }
    }
    if (actual_nbytes == -1) {
      fatalError(0, "Fatal error: %d in '%s'\n", errno, __func__);
    }
    buf = static_cast<uint8_t *>(buf) + actual_nbytes;
    nbytes -= actual_nbytes;
  }
}

#endif

#endif // !KERNELLIB
