#include "stdafx.h"
#include <unordered_map>
#include <iostream>
#include <list>

//Boolean math expression evaluator
//Goals: easy to switch reading from cin to files
//It lexes, parses and evaluates input on the fly (streaming).
//It should handle very large, but relatively "flat" input
//For lowering stack memory consumption in large, heavily-bracketed files, 
//the design can be updated to first find the AST children (deepest brackets) and evaluate them first
//If more performance is needed, look into using a parser library instead

using namespace std;

enum tokens
{
	TK_PLUS,
	TK_MULT,
	TK_BOOL,
	TK_BROP,
	TK_BRCL
};

struct token
{
	tokens name;

	//for passing the value of boolean. 
	//If extending this parser, it may need to be switched to a union of different types
	bool data_b;

	token(){};
	token( tokens t, bool data ) : name( t ), data_b( data ) {}
};

enum expressions
{
	EX_VALUE,
	EX_MULT,
	EX_ADD
};

/*
struct ASTnode
{
	union
	{
		tokens oprtr; //valid only if children are non-null
		bool value;
	};
	ASTnode* left_child;
	ASTnode* right_child;

	ASTnode() : left_child( NULL ), right_child( NULL ) {}
};*/

struct LR_stack_item
{
	bool expr_or_tok; //0 = expression, 1 = token
	union //depending on bool expr_or_tok
	{
		tokens tk;
		expressions expr;
	};

	//since this parser doesn't need to keep a real AST,
	//this serves as an "accumulator"
	bool current_value; 

	/*//the root of the AST is the operation that gets executed last
	//the subtrees are the operands
	ASTnode* AST;

	LR_stack_item() : AST( NULL ) {}
	*/
};

struct possible_token
{
	bool has_token;
	token tk;

	possible_token() : has_token( false ) {}

	void set_token( token t )
	{
		has_token = true;
		tk = t;
	}
};

struct parser_rule
{
	expressions result_exp;
	vector < LR_stack_item > to_match;
};

class incremental_lexer
{
	unordered_map< char, tokens > token_map;

public:
	incremental_lexer()
	{
		token_map.insert( make_pair( '+', TK_PLUS ) );
		token_map.insert( make_pair( '*', TK_MULT ) );
		token_map.insert( make_pair( '0', TK_BOOL ) );
		token_map.insert( make_pair( '1', TK_BOOL ) );
		token_map.insert( make_pair( '(', TK_BROP ) );
		token_map.insert( make_pair( ')', TK_BRCL ) );
	}

	//for every new input character, the lexer outputs zero or one token
	//For our language, there is no need for state in the lexer
	possible_token lexer( char in )
	{
		possible_token out;
		unordered_map< char, tokens >::iterator it_finder = token_map.find( in );
		if( it_finder != token_map.end() )
		{
			out.set_token( token( it_finder->second, in ) );
		}
		else
		{
			//Optional: assert( false ) or throw "Unrecognized token"
			//Also optional: check for spaces
			printf( "Unrecognized token, ignoring.\n" );
		}

		return out;
	}
};

class incremental_parser
{
	//simple LR parser. 
	//Logic: for every token, it shifts the token onto the stack
	//Then, it tries reducing the stack via the grammar rules
	//note: multiple reductions may be needed per one shift

	vector< parser_rule > rules;
	list< LR_stack_item> item_stack; //using a list because we need access to all elements

	unordered_map< tokens, char > debug_map;
	unordered_map< expressions, char > debug_map2;

	void debug_print_arr()
	{
		for( list<LR_stack_item>::iterator it = item_stack.begin(); it != item_stack.end(); ++it )
		{
			if( it->expr_or_tok )
			{
				printf( "%c", debug_map[it->tk] );
				if( it->tk == TK_BOOL )
				{
					printf( "%d", it->current_value );
				}
			}
			else
			{
				printf( "%c%d", debug_map2[it->expr], it->current_value );
			}
			printf( " " );
		}
		printf( "\n" );
	}

public:
	incremental_parser()
	{
		//EBNF-ish notation. I'm trying to keep it left-recursive to minimize stack space
		//I'm assuming the typical algebraic operator precedence:
		//brackets take precedence over multiplication, which in turn takes precedence over
		//addition.
		//MULT -> VALUE | MULT mult_op VALUE
		//ADD -> ADD plus_op MULT | MULT
		//VALUE -> ( ADD )
		
		debug_map.insert( make_pair( TK_PLUS, '+' ) );
		debug_map.insert( make_pair( TK_MULT, '*' ) );
		debug_map.insert( make_pair( TK_BOOL, 'B' ) );
		debug_map.insert( make_pair( TK_BROP, '(' ) );
		debug_map.insert( make_pair( TK_BRCL, ')' ) );

		debug_map2.insert( make_pair( EX_VALUE, 'V' ) );
		debug_map2.insert( make_pair( EX_MULT, 'M' ) );
		debug_map2.insert( make_pair( EX_ADD, 'A' ) );
			
	}

	void shift( token in)
	{
		LR_stack_item to_insert;
		to_insert.expr_or_tok = 1;
		to_insert.tk = in.name;
		to_insert.current_value = in.data_b;

		item_stack.push_back( to_insert );

		debug_print_arr();
	}

	//see if any of the items match the rules
	//current implementation is not fancy, just an O(n+m) matching
	//there are many various faster ways of matching, see string matching algorithms
	//the reducing could be much fancier, but it's not necessary
	bool reduce()
	{
		bool substitution_made = false;

		//depending on how nice to be when detecting errors,
		//the printfs can be substituded for dumping the stack and continuing
		LR_stack_item tmp = item_stack.back();
		if( tmp.expr_or_tok )
			switch( tmp.tk )
			{
			case TK_BOOL:
				{
					LR_stack_item val;
					val.expr_or_tok = 0;
					val.expr = EX_VALUE;
					val.current_value = tmp.current_value;
					item_stack.pop_back();
					item_stack.push_back( val );
					substitution_made = true;
					break;
				}
			case TK_BRCL: //bracket close
				{
					if( item_stack.size() < 3 )
					{
						printf( "Invalid syntax" );
					}
					item_stack.pop_back(); //remove closing bracket

					LR_stack_item value = item_stack.back();
					if( value.expr_or_tok || value.expr != EX_ADD )
					{
						printf( "Invalid syntax" );
					}
					item_stack.pop_back(); //temporarily remove the value

					if( ! item_stack.back().expr_or_tok || item_stack.back().tk != TK_BROP )
					{
						//no open bracket
						printf( "Invalid syntax" );
					}
					item_stack.pop_back();

					value.expr = EX_VALUE;
					item_stack.push_back( value );
					substitution_made = true;
					break;
				}
			default:
				{
					break;
				}
			}
		else
		{
			switch( tmp.expr )
			{
			case EX_VALUE:
				{
					//we can match this into MULT
					//If there is a MULT mult_op, then update the bool and remove those values
					LR_stack_item value = item_stack.back();
					item_stack.pop_back();
					if( item_stack.size() > 2 )
					{
						//so much boilerplate... I can't seem to get rbegin()-1 compiling inline
						LR_stack_item tk_m = item_stack.back();
						list< LR_stack_item>::reverse_iterator it = item_stack.rbegin();
						it++;
						LR_stack_item tk_mv = *it;
						if( tk_m.expr_or_tok
							&& tk_m.tk == TK_MULT
							&& tk_mv.expr_or_tok == false
							&& tk_mv.expr == EX_MULT )
						{
							value.current_value &= tk_mv.current_value;
							item_stack.pop_back();
							item_stack.pop_back();
						}
					}
					value.expr = EX_MULT;
					item_stack.push_back( value );

					substitution_made = true;
					break;
				}

			case EX_MULT:
				{
					//exchange it for an ADD
					//if there are other ADDs, update the value and remove them
					LR_stack_item mult = *( item_stack.rbegin() );
					item_stack.pop_back();
					if( item_stack.size() > 2 )
					{
						//TODO
						//possibly do stuff
					}
					mult.expr = EX_ADD;
					item_stack.push_back( mult );

					substitution_made = true;
					break;
				}
			default:
				break;
			}
		}

		debug_print_arr();
		return substitution_made;
	}

public:
	//for every new input token, run parser()
	void parser( token in )
	{
		shift( in );
		while( reduce() ){} //reduce as many times as needed
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	incremental_lexer lex;
	incremental_parser parser;

	for( char c = getchar(); c != '\n'; c = getchar() )
	{
		possible_token t = lex.lexer( c );
		if( !t.has_token )
			continue;

		parser.parser( t.tk );
	}
	return 0;
}
