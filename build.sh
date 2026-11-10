#!/bin/bash
set -e

export JAVA_HOME=${JAVA_HOME:-$(/usr/libexec/java_home)}
# CMake reads OPENSSL_ROOT_DIR from the environment to find OpenSSL 3.
export OPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR:-$(brew --prefix openssl@3)}

# Clean up all previous output
rm -f CMakeCache.txt
rm -rf out/

# Create the JNI header file
$JAVA_HOME/bin/javac -h ./src/include ./src/main/java/AesResult.java ./src/main/java/RsaResult.java ./src/main/java/Library.java

# Ensure cmake targets are updated
cmake . -DCMAKE_BUILD_TYPE=Release

mvn clean package

make clean
make
