Swift and stdlib suitable for firmware / bare metal coding.


This repo is a fork of the [apple/swift](https://github.com/apple/swift) repo used for building [swift-project1](https://github.com/spevans/swift-project1) and contains three main changes:

1. A `-disable-red-zone` compiler option to `swiftc` to compile code without
   using the red zone


2. A `-mcode-model` compiler option so that the `kernel` code model can be
   used.

3. Stdlib has been modified to remove:

- All floating point types `(Float32/Float64)` and code

- Math functions and some extra IO calls that are only needed for unix userland
programs.

This allows stdlib to be compiled without using SSE registers or instructions
to while still conforming to the x86_64 ABI.

The stdlib has also been compiled with `-disable-red-zone`, `-mcode-model=kernel`
and `-fno-omit-frame-pointer`, `-mno-omit-leaf-frame-pointer` to enable easier
stack backtraces.

A new branch is created each time the changes are rebased onto the latest swift
upstream master so that the changes are kept at the top of the commits, and to
make it easier to use an older version. The build script creates a tgz based on
the date to maintain multiple versions.

This will probably have problems building on MacOS as Objective-C support will
be compiled in which will only get in the way

NOTE: The master branch is just the [swift master](https://github.com/apple/swift/)
branch that it was rebased on.


The [swift-kstdlib](https://github.com/spevans/swift-kstdlib)
branch can be cloned from https://github.com/spevans/swift-kstdlib.git The other
repos to clone are:

| repo                                       | branch                       |
|--------------------------------------------|------------------------------|
| https://github.com/spevans/swift-kstdlib   | latest kstdlib-YYYYMMDD      |
| https://github.com/apple/swift-llvm        | stable                       |
| https://github.com/apple/swift-clang       | stable                       |
| https://github.com/apple/swift-compiler-rt | stable                       |
| https://github.com/apple/swift-cmark       | master                       |


```
git clone -b <kstdlib-YYYYMMDD> https://github.com/spevans/swift-kstdlib swift
git clone -b stable https://github.com/apple/swift-llvm llvm
git clone -b stable https://github.com/apple/swift-clang clang
git clone -b stable https://github.com/apple/swift-compiler-rt swift-compiler-rt
git clone https://github.com/apple/swift-cmark cmark
cd llvm/runtimes
ln -s ../../swift-compiler-rt compiler-rt
cd ../../swift
./utils/build-script  --no-swift-stdlib-assertions --build-subdir=buildbot_linux --release -- --swift-enable-ast-verifier=0 --install-swift --install-prefix=/usr '--swift-install-components=autolink-driver;compiler;clang-builtin-headers;stdlib' --build-swift-static-stdlib --skip-test-lldb --install-destdir=$HOME/swift-kernel --reconfigure  --verbose-build --jobs=2 2>&1|tee build.log
```

This will install a compiler and stdlib in `~/swift-kernel`

Because of the code removed from Stdlib and compiling with the
`-mcode-model=kernel` options , no tests can be run on the build and Foundation
cannot be built either.

SDK overlays are also not built as these use OS provided functions that arent
applicable or would need to be emulated.

When compiling .swift files, use the following arguments to disable red zone and
SSE:

`-Xfrontend -disable-red-zone -Xcc -mno-red-zone -Xcc -mno-mmx -Xcc -mno-sse -Xcc -mno-sse2`

Then link with `~/swift-kernel/usr/lib/swift_static/linux/libswiftCore.a`
and `~/swift-kernel/usr/lib/swift/linux/x86_64/swiftrt.o`.
