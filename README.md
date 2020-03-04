# ccov: Code Coverage based printf() debug support tool

`printf()` debug is simple yet powerful when debugger is not available.
We often insert `printf()` to trace all possible code path to find out
bug location. However, it is time-consuming and hard to inserting correctly.
This repository tries to automate this task.

ccov is an LLVM pass that inserts trace functions to the target
at BasicBlock level.
The trace function outputs execution log at runtime.

## Prerequisites

ccov requires `__log_coverage()` function with following prototype.

```c
void __log_coverage(const char *file, const char *func, const int line, const int attr);
```

See [sample/ccov.h](sample/ccov.h) for example definition.

### Dependencies

* CMake 3.4.3 or later
* LLVM 9 or later

## Building ccov

```console
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## License

ccov is released under the [MIT license](LICENSE).
