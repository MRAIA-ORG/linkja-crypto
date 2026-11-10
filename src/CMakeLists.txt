# CMakeList.txt : CMake project for the main src/ directory of linkja-crypto
#
cmake_minimum_required (VERSION 3.8)

# Define how many bytes should be in the secret value
SET(SECRET_LEN 32)
# Define how many characters of the secret value's hash we should display.
# By showing less than the whole string, we are minimizing information disclosure.
SET(SECRET_HASH_LEN 32)

if(EXISTS "src/include/linkja_secret.h")
  message(WARNING "*** The secret header file (src/include/linkja_secret) already exists ***\nIf you need a new secret generated, please delete the header first.")
else()
  message(STATUS "Generating the secret header file (src/include/linkja_secret).")

  # Start writing out the header file that will contain our secret value.
  file(WRITE include/linkja_secret.h "// This is an automatically generated file\n\n#ifndef LINKJA_SECRET_H\n#define LINKJA_SECRET_H\n\n")
  file(APPEND include/linkja_secret.h "#define LINKJA_SECRET_LEN ${SECRET_LEN}\n#define LINKJA_SECRET {")

  # Use openssl to generate random hex digits and use those to populate the secret
  # value in the header file
  if(WIN32)
    set(rand_cmd cmd /c openssl rand -hex ${SECRET_LEN})
    set(digest_cmd cmd /c openssl dgst -sha256 tmp/openssl.tmp)
  else()
    set(rand_cmd openssl rand -hex ${SECRET_LEN})
    set(digest_cmd openssl dgst -sha256 tmp/openssl.tmp)
  endif()
  execute_process(COMMAND ${rand_cmd} OUTPUT_VARIABLE RANDOM_VALUE)
  file(WRITE ../tmp/openssl.tmp "${RANDOM_VALUE}")
  # Note that the process executes in the context of the top-level directory, so we
  # access the file under tmp/ instead of ../tmp/
  execute_process(COMMAND ${digest_cmd} OUTPUT_VARIABLE RANDOM_VALUE_HASH)
  string(REGEX REPLACE "^.*= " "" RANDOM_VALUE_HASH "${RANDOM_VALUE_HASH}")
  string(REGEX REPLACE "\n$" "" RANDOM_VALUE_HASH "${RANDOM_VALUE_HASH}")
  file(REMOVE ../tmp/openssl.tmp)
  foreach(gen_loop RANGE 1 ${SECRET_LEN})
    MATH(EXPR str_start "(${gen_loop}*2)-2")
    string(SUBSTRING "${RANDOM_VALUE}" ${str_start} 2 VALUE)
    file(APPEND include/linkja_secret.h "0x${VALUE}")
    IF(NOT gen_loop EQUAL ${SECRET_LEN})
      file(APPEND include/linkja_secret.h ", ")
    ENDIF(NOT gen_loop EQUAL ${SECRET_LEN})
  endforeach(gen_loop)

  string(SUBSTRING "${RANDOM_VALUE_HASH}" 0 ${SECRET_HASH_LEN} RANDOM_VALUE_HASH)
  file(APPEND include/linkja_secret.h "}\n#define LINKJA_SECRET_HASH \"${RANDOM_VALUE_HASH}\" \n\n#endif\n")
endif()


# Specify the .c file that creates our shared library
add_library(linkjacrypto SHARED "linkja-crypto.c")

# Add some additional compiler options for strict warning checks.
if(UNIX OR APPLE)
    target_compile_options(linkjacrypto PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(WIN32)
    target_compile_options(linkjacrypto PRIVATE -Wall)
endif()

# Add the dependency to link to the openssl library
target_link_libraries(linkjacrypto OpenSSL::Crypto)
if(WIN32)
	target_link_libraries(linkjacrypto crypt32 ws2_32)
endif()


# Specify the directory where we want our final library written
set_target_properties(linkjacrypto
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)
