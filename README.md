# ccov: Code Coverage based printf() debug support tool

`printf()` debug is simple yet powerful when debugger is not available.
We often insert `printf()` to trace all possible code path to find bug locations.

ccov automates this hard and time-consuming task.

It is an LLVM pass that inserts trace functions to the target
at BasicBlock level.
The trace function outputs execution log at runtime.

## How it works

Let's assume collatz conjecture function.

```c
void collatz(int n)
{
  while (n != 1) {
    if (n % 2 == 0) {
      n /= 2;
    } else {
      n = 3 * n + 1;
    }
  }
}
```

If you want to insert debug trace for all possible code-path,
you have to add `printf()` like this:

```c
void collatz(int n)
{
  printf("#1\n");
  while (n != 1) {
    printf("#2\n");
    if (n % 2 == 0) {
      printf("#3\n");
      n /= 2;
    } else {
      printf("#4\n");
      n = 3 * n + 1;
    }
    printf("#5\n");
  }
  printf("#6\n");
}
```

Even this function is quite simple, but there are six lines of insertions.

ccov make this process much easier.
Only you have to do is, compile the target with ccov enabled.

```
$ clang -g -O0 -Xclang -load -Xclang ccov/build/CCov/libCCov.so -c -o main.o main.c
```

Note that ccov uses DebugLoc information in the LLVM bitcode.
Becauses of this, you have to compile the source code with `-g` and `-O0`.

On runtime, the executable outputs traces.

```
#CCOV:main.c:collatz:10
#CCOV:main.c:collatz:18:ret
#CCOV:main.c:collatz:7:entry
```

This output is from `__log_coverage()` in [sample/ccov.h](sample/ccov.h).
You can define your own trace output format.
[sample/report.py](sample/report.py) is an example script using ccov output.

```
    30|    10|  while (n != 1) {
    31|    18|}
    32|     7|void collatz(int n)
```

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
