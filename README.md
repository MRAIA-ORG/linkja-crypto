# linkja-crypto
C library to handle cryptographic functions.  Because this is intended to be used by the
linkja programs (which are written in Java), this includes the Java Native Interface (JNI)
headers with the C code.


## Supported platforms

The C library targets Windows x64, Linux x64 and macOS. We test macOS by hand and Linux in CI. Windows is untested since the move to OpenSSL 3. The library links OpenSSL 3 found on the system with CMake's `find_package(OpenSSL 3)`. The repository no longer ships OpenSSL binaries.

* macOS and Windows link libcrypto statically, so the built library has no OpenSSL dependency at run time.
* Linux links the distribution's shared libcrypto (`libssl3` on Ubuntu).
* If CMake does not find OpenSSL, set `OPENSSL_ROOT_DIR` to its install prefix, either as an environment variable or with `-DOPENSSL_ROOT_DIR=...`.

CI (`.github/workflows/ci.yml`) runs on Ubuntu for every pull request. It assembles the jar, builds with a throwaway secret and runs the C tests. Then it builds without the secret and runs `src/test/java/NoSecretsTest.java`.

### Secret and no-secrets builds

`INCLUDE_SECRETS` is `ON` by default. Configure then writes a new random secret to `src/include/linkja_secret.h` with `openssl rand`, so the `openssl` command must be on your `PATH`.

With `-DINCLUDE_SECRETS=OFF`, the library has no secret. `Library.createSecureHash` then throws `UnsupportedOperationException`. All other methods work.

## Building on macOS

### Dependencies

1. Xcode command line tools (`xcode-select --install`)
2. Homebrew packages: `brew install cmake openssl@3 cmocka maven`
3. A JDK 25, e.g. `brew install openjdk@25`

The scripts read `JAVA_HOME` and `OPENSSL_ROOT_DIR` when set. Otherwise they use `/usr/libexec/java_home` and `brew --prefix openssl@3`. Homebrew Java is not registered with `/usr/libexec/java_home`, so set `JAVA_HOME` for it. Set `OPENSSL_ROOT_DIR` yourself only to use another OpenSSL 3 install.

### Compiling and testing

These are the commands we ran on macOS (Apple Silicon) with OpenSSL 3.6 and Java 25:

```
export JAVA_HOME=/opt/homebrew/opt/openjdk@25
./build.sh
./test.sh
```

`build.sh` creates the JNI header, configures CMake, builds the jar with Maven and compiles `target/liblinkjacrypto.dylib`. `test.sh` runs the C tests with `ctest` and then the Java check in `src/test/java/Test.java`. Use `./build-nosecrets.sh` for a build without the secret.

### Architecture

The build makes a library for the architecture of the Mac it runs on. Check it with `lipo -archs target/liblinkjacrypto.dylib`.

A universal library (arm64 and x86_64) needs a universal OpenSSL 3. Homebrew installs OpenSSL for one architecture only. With a universal OpenSSL, configure with:

```
cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DOPENSSL_ROOT_DIR=<universal OpenSSL prefix>
```

If the OpenSSL is not universal, the link stops with `symbol(s) not found for architecture x86_64`.

## Building on Linux

CI runs these commands on Ubuntu (`ubuntu-latest`) with Temurin JDK 25:

```
sudo apt-get install -y libssl-dev cmake libcmocka-dev
mvn -B package
cmake . -DCMAKE_BUILD_TYPE=Release
make
ctest --output-on-failure
cmake . -DINCLUDE_SECRETS=OFF
make
jar=$(ls target/linkja-crypto-*.jar)
javac -cp "$jar" -d target/test-classes src/test/java/NoSecretsTest.java
java -cp "$jar:target/test-classes" -Djava.library.path=target NoSecretsTest
```

The library is `target/liblinkjacrypto.so`. The second `cmake` line makes a build without the secret. `NoSecretsTest` exits 1 if that build still has the secret.

## Building on Windows

These instructions were developed on a 64-bit Windows 10 Enterprise version 20H2. To ensure completeness, we started from a completely fresh install of Windows. It's possible that you may already have some of the dependencies installed. Given the number of versions of frameworks and installers, we haven't tested this with more than the specific versions listed. If you have success (or problems) with other verions, please let us know.

### Install Visual Studio 2019

[Download Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16)

During the installation and setup, you should select:

* Linux Development with C++
* Desktop Development with C++

> NOTE: Although we are installing the Visual Studio 2019 IDE, this is primarily to get the compilers and tooling that it provides.  We won't use the IDE to actually compile the code.

**Alternate**: If you already have Visual Studio installed, instructions for setting up these pieces can be found at the following links:

* [Install Linux tools](https://docs.microsoft.com/en-us/cpp/linux/download-install-and-setup-the-linux-development-workload?view=vs-2019)
* [CMake project integration with Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019)


### Download CMocka for MSVC

For this, we used the executable installer of CMocka 1.1.0 MSVC - [https://cmocka.org/files/1.1/cmocka-1.1.0-msvc.exe](https://cmocka.org/files/1.1/cmocka-1.1.0-msvc.exe)

### Java

You will need to inistall Java 1.8 or higher. [OpenJDK](https://openjdk.java.net/) is recommended
    
1. Linkja requires the JDK (not just the JRE) to be installed
2. Will use javac and java binaries
3. Requires Java Native Interface (JNI), which comes with most JDKs

For these instructions, we will get a Windows installer from [AdoptOpenJdk](https://adoptopenjdk.net/).

For this, we used version OpenJDK 11 (LTS) with the HotSpot JVM.

In the setup, ensure all of the options are set:

* Add to PATH
* Associate .jar
* Set JAVA_HOME variable
* JavaSoft (Oracle) registry keys


### OpenSSL

You need OpenSSL 3 static libraries and the `openssl` command. We have not tested this on Windows since the move to OpenSSL 3. One way is [vcpkg](https://vcpkg.io) with the triplet that has static libraries and the dynamic C runtime:

```
vcpkg install openssl:x64-windows-static-md
```

Then set `OPENSSL_ROOT_DIR` to the install prefix, e.g. `C:\vcpkg\installed\x64-windows-static-md`. CMake reads it from the environment.

### Download linkja-crypto

Clone the repository from [https://github.com/linkja/linkja-crypto.git](https://github.com/linkja/linkja-crypto.git) using whatever git client you prefer.

**Instructions for Visual Studio 2019**

0. From Visual Studio 2019, click on "Clone a repository".
1. "Repository location" - [https://github.com/linkja/linkja-crypto.git](https://github.com/linkja/linkja-crypto.git)
2. "Path" - feel free to use the default, or customize.  For this guide we are placing the code in `C:\Users\Linkja\Source\Repos\linkja-crypto`
3. Go ahead and close Visual Studio at this time. As noted above, we will not be using the IDE to compile.


### Open the Developer Command Prompt

In order for the build process to work, you will need to run the `x64 Native Tools Command Prompt for VS 2019`.  This can be navigated to from the Windows Start menu, and can be found under the `Visual Studio 2019` foldler.  There are similarly named options, so please confirm that you have selected the right one.

First, we will point CMake at OpenSSL and put the `openssl` command on the `PATH`. Change the paths to match your install.

```
SET OPENSSL_ROOT_DIR=C:\vcpkg\installed\x64-windows-static-md
SET PATH=%PATH%;%OPENSSL_ROOT_DIR%\tools\openssl
```

Next we will change to the directory where we downloaded the linkja-crypto code

```
cd C:\Users\Linkja\Source\Repos\linkja-crypto
```

The following commands will ensure that any previous build artifacts are removed.

```
del CMakeCache.txt
del src\linkjacrypto.exp
del src\linkjacrypto.lib
del src\linkjacrypto.dll
del src\linkjacrypto.dll.manifest
```

The following command is the key one for linkja-crypto.  Linkja-crypto uses a generated header file with a random hash, which should onlly be used once per project and then removed.  However, just know that once you delete the header file you won't be able to generate the same crypto library again.

```
del src\include\linkja_secret.h
```

Finally, the following commands will run our CMake scripts and then run the actual compilation process.

```
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" .
nmake clean
nmake
```

### Finishing Up

At this point you will see in the `src` subdirectory the following files:

* linkjacrypto.exp
* linkjacrypto.lib
* linkjacrypto.dll
* linkjacrypto.dll.manifest

While all of them can be shared, you only really need `linkjacrypto.dll`.  This should be distributed to those who need to run hashing. You should delete these files as well as the `linkja_secret.h` once you are done and the DLL has been distributed.