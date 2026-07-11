# Cobra ♟️

Cobra is a UCI-compatible chess engine written in modern C++.

It is the successor to **Kobra**, continuing its search framework while introducing a completely new NNUE evaluation implementation.

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

## Relationship to Kobra

Cobra builds upon the search framework developed for Kobra while replacing its evaluation backend with an original NNUE implementation.

The primary objective of the project is to provide a self-contained engine that no longer depends on an external NNUE inference library, making

experimentation, maintenance, and future development considerably simpler.

## Goals

Cobra serves as a platform for exploring modern chess engine techniques while maintaining a clean, readable, and extensible C++ codebase.
