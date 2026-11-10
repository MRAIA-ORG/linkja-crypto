# CMakeLists.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project("linkja-crypto")

option(INCLUDE_SECRETS "Include the hashing secrets in the library" ON)

if(INCLUDE_SECRETS)
  add_definitions(-DINCLUDE_SECRETS)
  message(STATUS "INCLUDE_SECRETS")
else()
  add_definitions(-DEXCLUDE_SECRETS)
  message(STATUS "EXCLUDE_SECRETS")
endif()

find_package(Java REQUIRED)
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
find_package(JNI REQUIRED)
include(UseJava)
message(STATUS "Using Java ${Java_VERSION_STRING}")
message(STATUS "  - Java compiler ${Java_JAVAC_EXECUTABLE}")

# OpenSSL 3 comes from the system: Homebrew on macOS, libssl-dev on Linux,
# vcpkg on Windows. Point CMake at it with -DOPENSSL_ROOT_DIR=<prefix>.
# On macOS and Windows a static libcrypto keeps the built library
# self-contained to distribute. Linux links the distro's shared libcrypto.
if (APPLE OR WIN32)
	set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()
find_package(OpenSSL 3 REQUIRED)
message(STATUS "Using OpenSSL ${OPENSSL_VERSION}: ${OPENSSL_CRYPTO_LIBRARY}")
# The code uses the OpenSSL 1.1.1 API (SHA256_*, RSA_*), which OpenSSL 3 marks
# deprecated. Declare that API level so -Werror does not fail on it.
add_definitions(-DOPENSSL_API_COMPAT=10101)

if (UNIX)
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/modules")
	find_package(CMOCKA QUIET)
endif()

project ("liblinkjacrypto")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${JNI_INCLUDE_DIRS})

# try to extract the version from the VERSION file
FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION VERSION)
SET(LC_VERSION ${VERSION})
# Make sure to strip trailing newline
string(REGEX REPLACE "\n$" "" LC_VERSION "${LC_VERSION}")
if(LC_VERSION STREQUAL "") # lookup failed
	set(LC_VERSION "unknown")
endif()
message(STATUS "Detected version: ${LC_VERSION}")
unset(VERSION)

# Include our actual source.
add_subdirectory ("src")

# Include our C unit tests
if (UNIX)
 	enable_testing()
	add_subdirectory(src/test/c)
endif()
