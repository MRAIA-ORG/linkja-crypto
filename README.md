# linkja-crypto
C library to handle cryptographic functions.  Because this is intended to be used by the
linkja programs (which are written in Java), this includes the Java Native Interface (JNI)
headers with the C code.

**WORK IN PROGRESS** - This is under active development

## Dependencies

Setup and building of the linkja-crypto library requires the following:

1. [CMake](https://cmake.org) (3.8 or higher)
2. Java (1.8 (8) or higher; tested with 25 [Exclipse Temurin OpenJDK](https://openjdk.java.net/) is allowed)
    1. Requires the JDK to be installed
    2. Will use javac and java binaries
    3. Requires Java Native Interface (JNI), which comes with most JDKs
3. [OpenSSL](https://www.openssl.org/) - the openssl binary needs to be in your path for the build scripts to work.  You can test this by executing "openssl version" at the command line.
4. C compiler and make system
    1. For macOS, gcc and make can be used.
    2. For Windows, Visual Studio 2026 Community edition can be used (earlier versions going back to 2019 almost certainly work as well).
5. [cmocka](https://cmocka.org/) for unit tests
    1. For macOS, `brew install cmocka` at the command line (You need to have Homebrew installed to do this).
    2. For Windows, [download the latest source code](https://cmocka.org/files/2.0/)
        1. Extract the .tar.xz to a folder. 
        2. You can use Visual Studio to open the folder and build via CMakeList

## Building

There are multiple components that go into building the linkja-crypto library.  For macOS, these are wrapped up into the `build.sh` script (available from the root directory).

1. Create the JNI header file
  >`$JAVA_HOME/bin/javac -h ./src/include ./src/java/Library.java`

2. Ensure cmake targets are built and/or updated
  > `cmake .`

3. Build the library
  ```
  make clean
  make
  ```

## Testing
To ensure everything is set up correctly, you can compile and run a simple test program using the testing script:

```
./test.sh
```
=======

## Building on Windows

These instructions were developed on a 64-bit Windows 11 Pro version 10.0.26200. Windows 10 should also work. To ensure completeness, we started from a completely fresh install of Windows. It's possible that you may already have some of the dependencies installed. Given the number of versions of frameworks and installers, we haven't tested this with more than the specific versions listed. If you have success (or problems) with other versions, please let us know.

### Install Visual Studio 2026

[Download Visual Studio 2026 Community Edition](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=18)

During the installation and setup, you should select:

* Linux and embedded development with C++
* Desktop Development with C++

> NOTE: Although we are installing the Visual Studio 2026 IDE, this is primarily to get the compilers and tooling that it provides.  We won't use the IDE to actually compile the code.

**Alternate**: If you already have Visual Studio installed, instructions for setting up these pieces can be found at the following links:

* [Install Linux tools](https://docs.microsoft.com/en-us/cpp/linux/download-install-and-setup-the-linux-development-workload?view=vs-2026)
* [CMake project integration with Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2026)


### Download CMocka for MSVC

For this, you may use the executable installer of CMocka 1.1.0 MSVC - [https://cmocka.org/files/1.1/cmocka-1.1.0-msvc.exe](https://cmocka.org/files/1.1/cmocka-1.1.0-msvc.exe).
The latest version of CMocka, 2.0, works as well and is preferred, but may be more challenging to install. You may [download the latest source code](https://cmocka.org/files/2.0/) and
        1. Extract the .tar.xz to a folder. 
        2. You can use Visual Studio to open the folder and build via CMakeList

### Java

You will need to install Java 8 (1.8) or higher. It is now tested with Java 25 so please use Java 25 if at all possible. [Exclipse Temurin OpenJDK](https://adoptium.net/temurin/releases) is recommended
    
1. Linkja requires the JDK (not just the JRE) to be installed
2. Will use javac and java binaries
3. Requires Java Native Interface (JNI), which comes with most JDKs

For these instructions, we will get a Windows installer from [Adoptium](https://adoptium.net/temurin/releases).

For this, we used version Eclipse Temurin OpenJDK 25 (LTS) with the HotSpot JVM.

In the setup, ensure all of the options are set:

* Add to PATH
* Associate .jar
* Set JAVA_HOME variable
* JavaSoft (Oracle) registry keys


### OpenSSL

You will need OpenSSL tools and libraries.  You can use versions downloadable from [Shining Light Productions](https://slproweb.com/products/Win32OpenSSL.html) (the Light package is all you need). The latest tested version is 3.6.1 (Win64) and thus you can likely use a 3.6.x version without issue.

Run the Windows installer choosing `C:\Program Files\OpenSSL` as the install directory.

### Download linkja-crypto

Clone the repository from [https://github.com/linkja/linkja-crypto.git](https://github.com/linkja/linkja-crypto.git) using whatever git client you prefer.

**Instructions for Visual Studio 2019**

0. From Visual Studio 2019, click on "Clone a repository".
1. "Repository location" - [https://github.com/linkja/linkja-crypto.git](https://github.com/linkja/linkja-crypto.git)
2. "Path" - feel free to use the default, or customize.  For this guide we are placing the code in `C:\Users\Linkja\Source\Repos\linkja-crypto`
3. Go ahead and close Visual Studio at this time. As noted above, we will not be using the IDE to compile.


### Open the Developer Command Prompt

In order for the build process to work, you will need to run the `x64 Native Tools Command Prompt for VS 2019`.  This can be navigated to from the Windows Start menu, and can be found under the `Visual Studio 2019` folder.  There are similarly named options, so please confirm that you have selected the right one.

You may run the newly included `linkja_compile.bat` batch file to execute the below commands all at once. The previous linkjacrypto.dll, if any exists, will be deleted by the batch file!

First, we will set up our command prompt `PATH` environment variable to include the path to the OpenSSL binaries--the following is the default used by the windows installer above.

```
SET PATH=%PATH%;"C:\Program Files\OpenSSL-Win64\bin"
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

The following command is the key one for linkja-crypto.  Linkja-crypto uses a generated header file with a random hash, which should only be used once per project and then removed.  However, just know that once you delete the header file you won't be able to generate the same crypto library again.

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

