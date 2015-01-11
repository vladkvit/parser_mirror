//defines enum tokens and enum nonterminals
#include <vector>
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
	TK_EPSILON //epsilon, ε
};

//for iterating over every element
const std::array<tokens, 7> all_terminals = { TK_PLUS,TK_MULT,TK_BOOL,TK_BROP,TK_BRCL,TK_END,TK_EPSILON };

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

/*void generate_rules(vector< parser_rule >& rules)
{

}*/
