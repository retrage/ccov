# ccov

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
