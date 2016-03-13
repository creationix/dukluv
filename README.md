dukluv
======

LibUV bindings for duktape JS engine

The goal of this project is to make a node.js-like environment that's ultra lean for tiny devices.

<http://dukluv.io/>

Prerequisites
-------------

Linux:
  
    $ sudo apt-get build-essential
    $ sudo apt-get install cmake

OS X (using Brew)

    $ brew install cmake

Windows

- Install cmake manually
- Use cygwin or start an MSVC command prompt

Compiling
---------

    $ git submodule init; git submodule update
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

The result is `./dukluv`, a standalone binary which you can add to
PATH or copy to e.g. `/usr/local/bin/`.
