#!/usr/bin/env bash

CURDIR=$(cd $(dirname $0)/..; pwd)

set -ue pipefail

export LC_ALL=C

. $CURDIR/VERSIONS

# rnnoise
rm -rf $CURDIR/third_party/rnnoise
mkdir -p $CURDIR/third_party/rnnoise
if [ ! -e $CURDIR/third_party/rnnoise-${THIRD_PARTY_VERSION}.tar.gz ]; then
  curl -Lo $CURDIR/third_party/rnnoise-${THIRD_PARTY_VERSION}.tar.gz https://chromium.googlesource.com/chromium/src/third_party/+archive/${THIRD_PARTY_VERSION}/rnnoise.tar.gz
fi
pushd $CURDIR/third_party/rnnoise
  tar xf $CURDIR/third_party/rnnoise-${THIRD_PARTY_VERSION}.tar.gz
popd

# pffft
rm -rf $CURDIR/third_party/pffft
mkdir -p $CURDIR/third_party/pffft
if [ ! -e $CURDIR/third_party/pffft-${THIRD_PARTY_VERSION}.tar.gz ]; then
  curl -Lo $CURDIR/third_party/pffft-${THIRD_PARTY_VERSION}.tar.gz https://chromium.googlesource.com/chromium/src/third_party/+archive/${THIRD_PARTY_VERSION}/pffft.tar.gz
fi
pushd $CURDIR/third_party/pffft
  tar xf $CURDIR/third_party/pffft-${THIRD_PARTY_VERSION}.tar.gz
popd
