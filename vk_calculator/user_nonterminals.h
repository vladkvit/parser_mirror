//defines enum tokens and enum nonterminals
#pragma once

#define DEBUG_PARSER

#include <array>
#include <unordered_map>

using namespace std;

//TODO: switch this to enum class
//nonterminal symbols
enum nonterminals
{
	EX_VALUE,
	EX_MULT,
	EX_ADD,
	EX_EMPTY,
	EX_S
};

//for iterating over every element
const std::array<nonterminals, 5> all_nonterminals = { EX_VALUE,EX_MULT,EX_ADD,EX_EMPTY, EX_S };


