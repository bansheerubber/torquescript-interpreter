# eggscript-interpreter
## What is eggscript?
Eggscript is an extension of an older programming language called Torquescript. Programmers familiar with Torquescript are also familiar with its numerous flaws that make it difficult to develop for. Eggscript fixes some of these flaws, and provides developers with a similar syntax to Torquescript.

Eggscript will introduce compatibility breaking changes with Torquescript. Some of the breaking changes will not be interpretable by Torquescript. Instead, the eggscript interpreter will be needed to interpret the code.

This repo will offer two interpreters: one for Torquescript and one for eggscript. The Torquescript interpreter will keep compatibility with TGE's interpreter, for better or for worse. The eggscript interpreter, on the other hand, will be optimized for performance so it will not suffer from the flaws of Torquescript.

The interpreters will also eventually be designed to easily include in your own projects. Adding Torquescript or eggscript support to your own program should be as simple as including some libraries.

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

## Testing:
**To build tests:**
```
$ make build-tests
```
**To run tests:**
```
$ make test
```