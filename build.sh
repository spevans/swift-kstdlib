#!/bin/sh

BUILDDIR=/mnt/build/swift-kernel-lib-build
PKGDIR=$HOME/src/swift-kernel-lib

mkdir -p $BUILDDIR
#ln -s $BUILDDIR/ build

#BRANCH=`git rev-parse --abbrev-ref HEAD`


BASENAME=swift-kernel-`date '+%Y%m%d'`

PREFIX=$BASENAME/usr
PKGNAME=$BASENAME.tgz
DESTDIR=$HOME/$PKGNAME
JOBS=${JOBS:=4}

echo $DESTDIR


swift/utils/build-script --build-swift-static-stdlib $@ \
                         --release --build-subdir=$BUILDDIR \
                         --no-swift-stdlib-assertions \
                         --verbose-build \
                         --skip-build-benchmarks=1 \
                         -- --swift-stdlib-disable-red-zone=1 \
                         --swift-enable-ast-verifier=0 \
                         --build-swift-dynamic-stdlib=0 \
                         --install-swift --install-prefix=$PREFIX \
                         --install-destdir=$HOME \
                         --installable-package=$PKGDIR/$PKGNAME \
                         '--swift-install-components=compiler;clang-builtin-headers;stdlib' \
                         --build-swift-static-stdlib \
                         --build-swift-dynamic-sdk-overlay=0 \
                         --build-swift-static-sdk-overlay=0 \
                         --jobs=$JOBS 2>&1|tee kernel-lib-build.log

#'--llvm-cmake-options=-DLLVM_ENABLE_ASSERTIONS=TRUE -DLLVM_TARGETS_TO_BUILD=X86' \
#                         --stdlib-deployment-targets=baremetal-x86_64 \
#                         --cross-compile-hosts=baremetal-x86_64 \


# 			 --skip-build-cmark=1 \
