# torquescript-interpreter
## What is Torquescript?
Torquescript is an older programming language which was used for video game development in an older version of the Torque Game Engine. Programmers familiar with Torquescript are also familiar with its many flaws making it difficult to develop for. However, Torquescript has some neat ideas that make some parts of game development effortless, even with its flaws. Torquescript was the first programming language I learned and I carry a sort of fascination for it to this day. I want to make my own programming language borrowing some of Torquescript's better ideas, but first I wanted to familiarize myself with building an interpreter since I have never made one before.

---
## Try it out online:
Visit https://bansheerubber.com/torquescript/ to try Torquescript online.

---
## Compilation:
Requires C++17 and `libfmt`.
```
$ git clone --recurse-submodules https://github.com/bansheerubber/torquescript-interpreter
```

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

## Running Code
```
$ ./torquescript code.cs
```

---

## Development:
I included a basic script `auto.py` that watches for changes in the source directory and automatically recompiles the executable whenever it finds changes (Linux only).

### Git Hooks
For some Git hooks that help automate testing a little, visit https://github.com/bansheerubber/torquescript-interpreter-hooks.

### Testing:
**To build tests:**
```
$ make build-tests
```
**To run tests:**
```
$ make test
```