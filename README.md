# README #

This is a simple parser generator written in C++ without any external libraries. It's a toy project - the features are readability, small size, and debug output at any stage of parser generation. Only tested with Visual Studio 2015.

### Current state ###

 * It's a work in progress.
 * The project assumes working grammar without shift-reduce or reduce-reduce conflicts. It will also not work with grammars with EPSILON.
