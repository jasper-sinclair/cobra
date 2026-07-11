# Cobra ♟️
![alt tag](https://raw.githubusercontent.com/jasper-sinclair/cobra/main/docs/cobra.png)

Cobra is a UCI-compatible chess engine written in modern C++.

  [![Release][release-badge]][release-link]
  [![Commits][commits-badge]][commits-link]
  ![Downloads][downloads-badge]
  [![License][license-badge]][license-link]
  
It is the brother of **Kobra**, continuing its search framework while introducing a completely new NNUE evaluation implementation.

Unlike Kobra, which relied on a customized version of Daniel Shawul's `nnue-probe`, Cobra implements an minimal yet strong 768×128 NNUE architecture.

This removes the dependency on an external NNUE library while providing a cleaner, fully integrated evaluation pipeline.

## Features

* UCI (Universal Chess Interface) compatible
* 768×128 NNUE implementation
* Alpha-beta / negamax search
* Iterative deepening
* Aspiration windows
* Quiescence search
* Transposition table
* Null-move pruning
* Late Move Reductions (LMR)
* Multi-threaded search
* Configurable UCI options including hash size, thread count, and contempt

## Goals

Cobra serves as a platform for exploring modern chess engine techniques while maintaining a clean, readable, and extensible C++ codebase.
Cobra's strength & unique play results from the use of an original NNUE evaluation created & trained from millions of Kobra vs Kobra self-play games.
Much of the data used is available here: https://github.com/jasper-sinclair/cobra-data

NNUE features:
https://github.com/jasper-sinclair/cobra/blob/main/docs/nnue-features.md

NNUE training:
https://github.com/jasper-sinclair/cobra/blob/main/docs/nnue-train.md

NNUE creation:
https://github.com/jasper-sinclair/cobra/blob/main/docs/nnue-creation.md

KOBRA train:
https://github.com/jasper-sinclair/cobra-train

Kobra NNUE is powered by a highly optimized custom adaptation of https://github.com/dshawul/nnue-probe

[license-badge]:https://img.shields.io/github/license/jasper-sinclair/cobra?style=for-the-badge&label=license&color=success
[license-link]:https://github.com/jasper-sinclair/cobra/blob/main/LICENSE
[release-badge]:https://img.shields.io/github/v/release/jasper-sinclair/cobra?style=for-the-badge&label=official%20release
[release-link]:https://github.com/jasper-sinclair/cobra/releases/latest
[commits-badge]:https://img.shields.io/github/commits-since/jasper-sinclair/cobra/latest?style=for-the-badge
[commits-link]:https://github.com/jasper-sinclair/cobra/commits/main
[downloads-badge]:https://img.shields.io/github/downloads/jasper-sinclair/cobra/total?color=success&style=for-the-badge
