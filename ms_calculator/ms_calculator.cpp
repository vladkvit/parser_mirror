#include "stdafx.h"
#include <unordered_map>
#include <map>
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
	TK_BRCL,
	TK_END
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
	EX_ADD,
	EX_EMPTY,
	EX_E,
	EX_T,
	EX_ACCEPT
};

struct symbol
{
	bool expr_or_tok; //token is true, expression is false
	union //depending on bool expr_or_tok
	{
		tokens tk;
		expressions expr;
	};

	void insert_tok( tokens tki )
	{
		expr_or_tok = true;
		tk = tki;
	}
	void insert_exp( expressions ex )
	{
		expr_or_tok = false;
		expr = ex;
	}

	symbol() {}
	symbol( tokens tki ) : expr_or_tok( true ), tk( tki ) {}
	symbol( expressions ex ) : expr_or_tok( false ), expr( ex ) {}

	bool operator < ( const symbol& other ) const
	{
		if( other.expr_or_tok != expr_or_tok )
		{
			return expr_or_tok;
		}
		if( expr_or_tok )
		{
			return other.expr < expr;
		}
		return other.tk < tk;
	}
};

struct LR_stack_item
{
	symbol smb;

	//since this parser doesn't need to keep a real AST,
	//this serves as an "accumulator"
	bool current_value; 

	int state;

	void insert_tok( tokens tki )
	{
		smb.insert_tok( tki );
	}
	void insert_exp( expressions ex )
	{
		smb.insert_exp( ex );
	}
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
	int num_to_pop;
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
		token_map.insert( make_pair( '\n', TK_END ) );
	}

	//for every new input character, the lexer outputs zero or one token
	//For our language, there is no need for state in the lexer, since all tokens are 1 char wide
	possible_token lexer( char in )
	{
		possible_token out;
		unordered_map< char, tokens >::iterator it_finder = token_map.find( in );
		if( it_finder != token_map.end() )
		{
			out.set_token( token( it_finder->second, in ) );
			if( it_finder->second == TK_BOOL )
			{
				if( in == '0' )
				{
					out.tk.data_b = false;
				}
				else
				{
					out.tk.data_b = true;
				}
			}
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

struct parse_table_item
{
	bool shift_or_reduce; //false=shift, true=reduce
	union
	{
		int new_state;
		int reduce_rule;
	};

	parse_table_item() {}
	parse_table_item( bool sr, int st ) : shift_or_reduce( sr ), new_state( st ) {}
};

class incremental_parser
{
public:
	bool final_value;

private:
	static const int NUM_STATES = 15;

	//simple SLR(1) parser. 

	vector< parser_rule > rules;
	list< LR_stack_item> item_stack; //using a list because we need access to all elements

	unordered_map< tokens, char > debug_map;
	unordered_map< expressions, char > debug_map2;

	//map and not an unordred_map because I'm too lazy to write a hasher helper
	//C++ can't figure out a default hash function for non-primitive types
	vector< map< symbol, parse_table_item > > parse_table;

	int offset_state;

	void debug_print_arr()
	{
		for( list<LR_stack_item>::iterator it = item_stack.begin(); it != item_stack.end(); ++it )
		{
			if( it->smb.expr_or_tok )
			{
				printf( "%c", debug_map[it->smb.tk] );
				if( it->smb.tk == TK_BOOL )
				{
					printf( "%d", it->current_value );
				}
			}
			else
			{
				printf( "%c-%d-%d", debug_map2[it->smb.expr], it->current_value, it->state );
			}
			printf( " " );
		}
		printf( "\n" );
	}

public:
	incremental_parser()
	{
		offset_state = 0;

		//EBNF-ish notation.
		//I'm assuming the typical algebraic operator precedence:
		//brackets take precedence over multiplication, which in turn takes precedence over addition

		//MULT -> VALUE | MULT mult_op VALUE
		//ADD -> ADD plus_op MULT | MULT
		//VALUE -> ( ADD )

		debug_map.insert( make_pair( TK_PLUS, '+' ) );
		debug_map.insert( make_pair( TK_MULT, '*' ) );
		debug_map.insert( make_pair( TK_BOOL, 'B' ) );
		debug_map.insert( make_pair( TK_BROP, '(' ) );
		debug_map.insert( make_pair( TK_BRCL, ')' ) );
		debug_map.insert( make_pair( TK_END, '/' ) );

		debug_map2.insert( make_pair( EX_VALUE, 'V' ) );
		debug_map2.insert( make_pair( EX_MULT, 'M' ) );
		debug_map2.insert( make_pair( EX_ADD, 'A' ) );
		debug_map2.insert( make_pair( EX_EMPTY, '\\' ) );
		debug_map2.insert( make_pair( EX_E, 'E' ) );
		debug_map2.insert( make_pair( EX_T, 'T' ) );
		
		parse_table.resize( NUM_STATES );
		parse_table[0].insert( make_pair( symbol( TK_BROP ), parse_table_item( false, 4 ) ) );
		parse_table[0].insert( make_pair( symbol( TK_BOOL), parse_table_item( false, 5 ) ) );
		parse_table[0].insert( make_pair( symbol( EX_ADD ), parse_table_item( false, 1 ) ) );
		parse_table[0].insert( make_pair( symbol( EX_MULT ), parse_table_item( false, 2 ) ) );
		parse_table[0].insert( make_pair( symbol( EX_VALUE ), parse_table_item( false, 3 ) ) );

		parse_table[1].insert( make_pair( symbol( TK_END ), parse_table_item( true, -1 ) ) );
		
		parse_table[2].insert( make_pair( symbol( TK_END ), parse_table_item( true, 0 ) ) );
		parse_table[2].insert( make_pair( symbol( TK_PLUS ), parse_table_item( false, 6 ) ) );
		parse_table[2].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 0 ) ) );

		parse_table[3].insert( make_pair( symbol( TK_END ), parse_table_item( true, 2 ) ) );
		parse_table[3].insert( make_pair( symbol( TK_MULT ), parse_table_item( false, 7 ) ) );
		parse_table[3].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 2 ) ) );
		parse_table[3].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 2 ) ) );

		parse_table[4].insert( make_pair( symbol( TK_BROP ), parse_table_item( false, 4 ) ) );
		parse_table[4].insert( make_pair( symbol( TK_BOOL ), parse_table_item( false, 12 ) ) );
		parse_table[4].insert( make_pair( symbol( EX_ADD ), parse_table_item( false, 8 ) ) );
		parse_table[4].insert( make_pair( symbol( EX_MULT ), parse_table_item( false, 10 ) ) );
		parse_table[4].insert( make_pair( symbol( EX_VALUE ), parse_table_item( false, 11 ) ) );

		parse_table[5].insert( make_pair( symbol( TK_END ), parse_table_item( true, 5 ) ) );
		parse_table[5].insert( make_pair( symbol( TK_MULT ), parse_table_item( true, 5 ) ) );
		parse_table[5].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 5 ) ) );
		parse_table[5].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 5 ) ) );

		parse_table[6].insert( make_pair( symbol( TK_BROP ), parse_table_item( false, 4 ) ) );
		parse_table[6].insert( make_pair( symbol( TK_BOOL ), parse_table_item( false, 12 ) ) );
		parse_table[6].insert( make_pair( symbol( EX_ADD ), parse_table_item( false, 9 ) ) );
		parse_table[6].insert( make_pair( symbol( EX_MULT ), parse_table_item( false, 10 ) ) );
		parse_table[6].insert( make_pair( symbol( EX_VALUE ), parse_table_item( false, 11 ) ) );

		parse_table[7].insert( make_pair( symbol( TK_BROP ), parse_table_item( false, 4 ) ) );
		parse_table[7].insert( make_pair( symbol( TK_BOOL ), parse_table_item( false, 12 ) ) );
		parse_table[7].insert( make_pair( symbol( EX_MULT ), parse_table_item( false, 13 ) ) );
		parse_table[7].insert( make_pair( symbol( EX_VALUE ), parse_table_item( false, 11 ) ) );

		parse_table[8].insert( make_pair( symbol( TK_BRCL ), parse_table_item( false, 14 ) ) );

		parse_table[9].insert( make_pair( symbol( TK_END ), parse_table_item( true, 1 ) ) );
		parse_table[9].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 1 ) ) );

		parse_table[10].insert( make_pair( symbol( TK_END ), parse_table_item( true, 0 ) ) );
		parse_table[10].insert( make_pair( symbol( TK_PLUS ), parse_table_item( false, 6 ) ) );
		parse_table[10].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 0 ) ) );

		parse_table[11].insert( make_pair( symbol( TK_END ), parse_table_item( true, 2 ) ) );
		parse_table[11].insert( make_pair( symbol( TK_MULT ), parse_table_item( false, 7 ) ) );
		parse_table[11].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 2 ) ) );
		parse_table[11].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 2 ) ) );

		parse_table[12].insert( make_pair( symbol( TK_END ), parse_table_item( true, 5 ) ) );
		parse_table[12].insert( make_pair( symbol( TK_MULT ), parse_table_item( true, 5 ) ) );
		parse_table[12].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 5 ) ) );
		parse_table[12].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 5 ) ) );

		parse_table[13].insert( make_pair( symbol( TK_END ), parse_table_item( true, 3 ) ) );
		parse_table[13].insert( make_pair( symbol( TK_MULT ), parse_table_item( true, 3 ) ) );
		parse_table[13].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 3 ) ) );
		parse_table[13].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 3 ) ) );

		parse_table[14].insert( make_pair( symbol( TK_END ), parse_table_item( true, 4 ) ) );
		parse_table[14].insert( make_pair( symbol( TK_MULT ), parse_table_item( true, 4 ) ) );
		parse_table[14].insert( make_pair( symbol( TK_PLUS ), parse_table_item( true, 4 ) ) );
		parse_table[14].insert( make_pair( symbol( TK_BRCL ), parse_table_item( true, 4 ) ) );

		rules.resize( 6 );
		rules[0].result_exp = EX_ADD;
		rules[0].num_to_pop = 1;

		rules[1].result_exp = EX_ADD;
		rules[1].num_to_pop = 3;

		rules[2].result_exp = EX_MULT;
		rules[2].num_to_pop = 1;

		rules[3].result_exp = EX_MULT;
		rules[3].num_to_pop = 3;

		rules[4].result_exp = EX_VALUE;
		rules[4].num_to_pop = 3;

		rules[5].result_exp = EX_VALUE;
		rules[5].num_to_pop = 1;
		
		LR_stack_item initial;
		initial.insert_exp( EX_EMPTY );
		initial.state = 0;
		shift( initial );
	}

	void shift( LR_stack_item to_insert )
	{
		item_stack.push_back( to_insert );
		debug_print_arr();
	}

	void reduce( int rule )
	{
		//note: we're not building an AST tree, so
		//we have to hack in the operations into here
		debug_print_arr();
	}

public:
	//for every new input token, run parser()

	bool parser( symbol in, bool data )
	{
		LR_stack_item top = item_stack.back();
		map< symbol, parse_table_item >::iterator it = parse_table[top.state].find( in );
		if( it == parse_table[top.state].end() )
		{
			printf( "bad syntax\n" );
			throw "Incorrect syntax";
		}

		parse_table_item itm = it->second;
		if( itm.reduce_rule == -1 ) //flag for accepting
		{
			final_value = item_stack.back().current_value;
			return false;
		}

		bool reduced = false;
		if( itm.shift_or_reduce == false ) //shift
		{
			LR_stack_item new_s;
			new_s.smb = in;
			new_s.current_value = data;
			new_s.state = itm.new_state;
			shift( new_s );
		}
		else //reduce
		{
			reduced = true;
			parser_rule rule = rules[itm.reduce_rule];
			//since we're not building an AST tree, the below is somewhat of a hack to compute the value
			bool new_value = false;
			switch( itm.reduce_rule )
			{
				case 0:
				{
					new_value = item_stack.back().current_value;
					break;
				}
				case 1:
				{
					bool val1 = item_stack.back().current_value;
					list< LR_stack_item>::reverse_iterator prev_it = item_stack.rbegin();
					prev_it++;//can't increment by more than 1 with lists
					prev_it++;
					bool val2 = prev_it->current_value;
					new_value = val1 | val2;
					break;
				}
				case 2:
				{
					new_value = item_stack.back().current_value;
					break;
				}
				case 3:
				{
					bool val1 = item_stack.back().current_value;
					list< LR_stack_item>::reverse_iterator prev_it = item_stack.rbegin();
					prev_it++;//can't increment by more than 1 with lists
					prev_it++;
					bool val2 = prev_it->current_value;
					new_value = val1 & val2;
					break;
				}
				case 4:
				{
					list< LR_stack_item>::reverse_iterator prev_it = item_stack.rbegin();
					prev_it++;
					new_value = prev_it->current_value;
					break;
				}
				case 5:
				{
					new_value = item_stack.back().current_value;
					break;
				}

				default:
					printf( "Rule error\n" );
					break;
			}

			for( int i = 0; i < rule.num_to_pop; i++ )
			{
				item_stack.pop_back();
			}

			LR_stack_item old_state = item_stack.back();
			LR_stack_item new_state;

			new_state.insert_exp( rule.result_exp );
			new_state.state = parse_table[ old_state.state][ rule.result_exp ].new_state;
			new_state.current_value = new_value;

			item_stack.push_back( new_state );

			debug_print_arr();
		}
		return reduced;
	}

	void parser_t( token in )
	{
		symbol s( in.name );
		while( parser( s, in.data_b ) );
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	incremental_lexer lex;
	incremental_parser parser;

	for( char c = getchar();; c = getchar() )
	{
		possible_token t = lex.lexer( c );
		if( !t.has_token )
			continue;

		parser.parser_t( t.tk );

		if( c == '\n' )
		{
			printf( "Result is %d\n", parser.final_value );
			break;
		}
	}
	return 0;
}
