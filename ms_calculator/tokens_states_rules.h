//defines enum tokens and enum expressions
#include <vector>
#include <array>

using namespace std;

//terminal symbols
enum tokens
{
	TK_PLUS,
	TK_MULT,
	TK_BOOL,
	TK_BROP,
	TK_BRCL,
	TK_END //no-more-characters signal
};

//nonterminal symbols
enum expressions
{
	EX_VALUE,
	EX_MULT,
	EX_ADD,
	EX_EMPTY
};

//for iterating over every element
const std::array<expressions, 4> all_expressions = { EX_VALUE,EX_MULT,EX_ADD,EX_EMPTY };

/*void generate_rules(vector< parser_rule >& rules)
{

}*/
