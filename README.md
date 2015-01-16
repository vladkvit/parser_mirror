# README #

This is a simple parser generator written in C++ without any external libraries. It's a toy project - the features are readability, small codebase, and debug output at any stage of parser generation.

### Current state ###

 * The "lexer" is a trivial character-matcher without regex support.
 * The project assumes working grammar without shift-reduce or reduce-reduce conflicts. It will also not work with grammars with EPSILON.
 * It uses SLR(1) logic at the moment. The comments explain how to modify it to be LR(0) or LR(1)
 * This project is meant for learning, as opposed to using it in production.
 * Only tested with Visual Studio 2015.

### Coming soon, maybe ###
 * A fancier lexer with user-defined token data structures
 * LR(0) and LR(1) parsing.
