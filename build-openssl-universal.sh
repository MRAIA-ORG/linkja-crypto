#!/bin/bash
# Build a universal (arm64 and x86_64) static OpenSSL 3 on macOS, for a
# universal liblinkjacrypto.dylib. Homebrew OpenSSL has one architecture only.
#
# Usage: ./build-openssl-universal.sh <openssl-version> <install-prefix>
# e.g.   ./build-openssl-universal.sh 3.6.5 "$HOME/openssl-universal"
#
# The release tarball comes from the openssl/openssl GitHub releases. Its
# SHA-256 must equal the value pinned for that version in openssl-versions.txt.
# A version with no pinned value is refused. The script builds each
# architecture in its own folder for the macOS in macos-deployment-target.txt,
# then joins the static libraries with lipo. Headers come from the arm64
# build, after a check that both architectures installed the same headers.
set -euo pipefail

if [ $# -ne 2 ] || [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 <openssl-version> <install-prefix>" >&2
  exit 2
fi
version=$1
prefix=$2
here=$(cd "$(dirname "$0")" && pwd)

if ! [[ $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  echo "error: '$version' is not an OpenSSL version like 3.6.5" >&2
  exit 2
fi
expected=$(awk -v v="$version" '$1 == v { print $2 }' "$here/openssl-versions.txt")
if [ -z "$expected" ]; then
  echo "error: OpenSSL $version has no pinned SHA-256 in openssl-versions.txt" >&2
  exit 1
fi
MACOSX_DEPLOYMENT_TARGET=$(grep -m1 -E '^[0-9]' "$here/macos-deployment-target.txt")
export MACOSX_DEPLOYMENT_TARGET

archs="arm64 x86_64"
tarball="openssl-$version.tar.gz"
url="https://github.com/openssl/openssl/releases/download/openssl-$version/$tarball"

work="$prefix/work"
mkdir -p "$work"
cd "$work"

curl -fsSL --proto '=https' --proto-redir '=https' -o "$tarball" "$url"
actual=$(shasum -a 256 "$tarball" | cut -d' ' -f1)
if [ "$actual" != "$expected" ]; then
  echo "error: $tarball has SHA-256 $actual, but openssl-versions.txt pins $expected" >&2
  exit 1
fi
echo "$tarball: SHA-256 matches the pinned value"

for arch in $archs; do
  rm -rf "src-$arch"
  mkdir "src-$arch"
  tar -xzf "$tarball" -C "src-$arch" --strip-components 1
  (
    cd "src-$arch"
    ./Configure "darwin64-$arch-cc" no-shared no-tests --prefix="$prefix/$arch"
    make -j"$(sysctl -n hw.ncpu)"
    make install_sw
  )
done

diff -r "$prefix/arm64/include" "$prefix/x86_64/include"

rm -rf "${prefix:?}/include" "${prefix:?}/lib"
mkdir -p "$prefix/lib"
cp -R "$prefix/arm64/include" "$prefix/include"
for lib in libcrypto.a libssl.a; do
  lipo -create "$prefix/arm64/lib/$lib" "$prefix/x86_64/lib/$lib" -output "$prefix/lib/$lib"
  lipo -info "$prefix/lib/$lib"
done
echo "Universal OpenSSL $version for macOS $MACOSX_DEPLOYMENT_TARGET is in $prefix. Set OPENSSL_ROOT_DIR=$prefix."
