# Carving by instrumenting LLVM IR code for machine learning

## Prerequisite

1. Clang/LLVM, developed on version 13.0.1, not tested on other versions.
    * Use apt package downloader (https://apt.llvm.org) or manualy built from source (https://releases.llvm.org/download.html).
    `sudo apt install llvm-13-dev clang-13 libclang-13-dev lld-13 libc++abi-13-dev`
    * It assumes `llvm-config, clang, clang++, opt, ...` are on `PATH`

2. [gllvm](https://github.com/SRI-CSL/gllvm), needed to get whole program bitcode

3. Python 3.6+, make

## Build
  `make`

## 1. Target subject build

1. Build target program using `make` or `cmake` as usual, but set `CC=gclang` and `CXX=gclang++` to let gllvm to compile the target program. It varies how to set compiler to use for different programs, but most popular open source programs support building with non-default compiler.
    * It is recommend to use `--disable-shared` flags.
        * Codes that is build in shared libraries won't be instrumented for carving, so you won't able to get carved objects.
    * It is recommend to turn on debug options, but it is not necessary.
    * Example : ``CC=gclang CXX=gclang++ CFLAGS="-O0 -g" CXXFLAGS="-O0 -g" ./configure --prefix=`pwd`\gclang_install --disable-shared``
2. `get-bc <target executable>` You can get bitcode of the executable file.
3. (optional) `llvm-dis <target.bc>` will make human-readable LLVM IR code of the target program.

## 2. Carving Instrumentation

1. `opt -enable-new-pm=0 -load {$PROJECT_PATH}/lib/carve_model_pass.so --carve < <target.bc> -o <out.bc>`

2. `clang++ <out.bc> <compile flags> -o <target.carv> -L {$PROJECT_PATH}/lib -l:m_carver.a`
    * `<compile flags>` are usually shared libraries that are linked to the original target executable.
    * You can get list of shared linked shared libraries by running `ldd <target executable>`, if libpthread.so is linked, you need to put `-lpthread` as compile flags

3. It will generate an executable named as like `target.carv`.

## 3. Run carving

1. `mkdir <carved_ctx_dir>`
2. `<target.carv> <args> <carved_ctx_dir>`; Run the new carving executable as same as the original executable, but add a directory path at the end to store all carved contexts.