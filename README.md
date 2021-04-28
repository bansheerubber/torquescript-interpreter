# eggscript-interpreter
## What is eggscript?
Eggscript is an extension of an older programming language called Torquescript. Programmers familiar with Torquescript are also familiar with its numerous flaws that make it difficult to develop for. Eggscript fixes some of these flaws, and provides developers with a similar syntax to Torquescript. Eggscript will be able to transpile to Torquescript, sort of.  

Eggscript will introduce compatibility breaking changes with Torquescript. Some of the breaking changes will not be transpilable to Torquescript. Instead, the eggscript interpreter will be needed to interpret the code.

This repo will offer two interpreters: one for Torquescript and one for eggscript. The Torquescript interpreter will keep compatibility with TGE's interpreter, for better or for worse. The eggscript interpreter, on the other hand, will be optimized for performance so it will not suffer from the flaws of Torquescript.

---
## Compilation:
Requires C++17.

**Linux:**
```
$ make
```

**Windows using cygwin:**
```
$ make
```
Depending on what GCC compiler you use, it may not work. I used the latest version of TDM-GCC.

---

## Usage:
```
$ cd dist
$ ./eggscript

usage: eggscript [options] [files or directories]
    -h, --help                                        Show help message
    -o [directory], --output [directory]              Output transpiled Torquescript file(s) to the specified directory
    -t [number], --threads [number]                   Number of threads used during parsing. Defaults to amount of logical cores on your computer.
    -m, --minify                                      Minify the output files
    --no-warnings                                     Do not print warnings
```
You can also pipe text from stdout into the program:
```
$ eggscript < file.cs
```
```
$ cat file.cs | eggscript
```
The result of the parsing will be printed on stdout. Warnings are silenced when using piping. If there is an error, the program will return a non-zero exit code.