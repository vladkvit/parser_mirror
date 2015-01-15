#pragma once

#include <regex>
#include <unordered_map>
#include <array>

using namespace std;

//TODO: switch the enums to enum classes
//terminal symbols
enum tokens
{
	TK_PLUS,
	TK_MULT,
	TK_BOOL,
	TK_BROP,
	TK_BRCL,
	TK_END, //no-more-characters $ signal
	TK_EPSILON //epsilon, ε. Currently, the parser does not support grammars with epsilon
};

//for iterating over every element
const std::array<tokens, 7> all_terminals = { TK_PLUS,TK_MULT,TK_BOOL,TK_BROP,TK_BRCL,TK_END,TK_EPSILON };

//For now, it just matches characters.
static void lexer_rules( unordered_map< char, tokens >& token_map )
{
	token_map.insert( make_pair( '+', TK_PLUS ) );
	token_map.insert( make_pair( '*', TK_MULT ) );
	token_map.insert( make_pair( '0', TK_BOOL ) );
	token_map.insert( make_pair( '1', TK_BOOL ) );
	token_map.insert( make_pair( '(', TK_BROP ) );
	token_map.insert( make_pair( ')', TK_BRCL ) );
	token_map.insert( make_pair( '\n', TK_END ) );
}
