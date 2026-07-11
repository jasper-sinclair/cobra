# Cobra ♟️

Cobra is a UCI-compatible chess engine written in modern C++. It combines a classical alpha-beta search with an NNUE evaluation network and a collection of modern search optimizations to provide strong analysis and playing strength.

## Features

- UCI (Universal Chess Interface) compatible
- NNUE-based evaluation
- Alpha-beta / negamax search
- Iterative deepening
- Aspiration windows
- Quiescence search
- Transposition table
- Null-move pruning
- Late Move Reductions (LMR)
- Multi-threaded search
- Configurable hash size, thread count, and contempt value

## Usage

Cobra implements the UCI protocol and can be used with any compatible chess GUI, including Arena, Cute Chess, Banksia GUI, Fritz, and ChessBase.

## Project

The goal of Cobra is to explore modern chess engine techniques while maintaining a clean and readable C++ codebase. Development focuses on search improvements, evaluation quality, and overall playing strength.

## License

GPL-3.0
