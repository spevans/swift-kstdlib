#!/bin/sh


PKGDIR=/home/spse/swift-build/kstdlib
BRANCH=${BRANCH:=`(cd swift ; git rev-parse --abbrev-ref HEAD)`}
BUILDDIR=/home/spse/swift-build/kstdlib/${BRANCH}
_DATE=${DATE:=`date '+%Y%m%d'`}

mkdir -p $BUILDDIR
echo "Branch: ${BRANCH}"
echo _DATE=$_DATE

BASENAME=${BRANCH}
PREFIX=/$BASENAME/usr
PKGNAME=$BASENAME.tgz
JOBS=${JOBS:=4}
echo PREFIX=$PREFIX


swift/utils/build-script --build-swift-static-stdlib $@ \
                         --release \
                         --build-subdir=$BUILDDIR \
                         --swift-stdlib-assertions \
                         --verbose-build \
                         --skip-build-benchmarks=1 \
                         --skip-build-compiler-rt=false \
                         -- --swift-stdlib-disable-red-zone=1 \
                         --swift-enable-ast-verifier=0 \
                         --build-swift-dynamic-stdlib=0 \
                         --install-swift --install-prefix=$PREFIX \
                         --install-destdir=$HOME \
                         --installable-package=$PKGDIR/$PKGNAME \
                         '--swift-install-components=compiler;clang-builtin-headers;clang-resource-dir-symlink;stdlib' \
                         '--llvm-install-components=clang;clang-headers;compiler-rt' \
                         --build-swift-static-stdlib=1 \
                         --build-swift-dynamic-stdlib=0 \
                         --build-swift-dynamic-sdk-overlay=0 \
                         --build-swift-static-sdk-overlay=0 \
                         --swift-include-tests=0 \
                         --jobs=$JOBS 2>&1|tee kernel-lib-build.log

#'--llvm-cmake-options=-DLLVM_ENABLE_ASSERTIONS=TRUE -DLLVM_TARGETS_TO_BUILD=X86' \
#                         --stdlib-deployment-targets=baremetal-x86_64 \
#                         --cross-compile-hosts=baremetal-x86_64 \


# 			 --skip-build-cmark=1 \
