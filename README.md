# eggscript-interpreter
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