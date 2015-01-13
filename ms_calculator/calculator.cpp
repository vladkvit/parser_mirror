#include "stdafx.h"

//should have enum tokens, enum nonterminals
#include "tokens_states_rules.h"
#include "lexer.h"
#include "parser.h"
#include "action_goto_generation.h"

//Boolean math expression evaluator, implemented as an SLR(1) parser
//Goals: easy to switch reading from cin to files
//It lexes, parses and evaluates input on the fly (streaming).
//It should handle very large (larger than memory space), but relatively "flat" input
//For lowering stack memory consumption in large, heavily-bracketed files, 
//the design can be updated to first find the AST children (deepest brackets) and evaluate them first
//If more performance or flexibility is needed, a parser library may be beneficial

using namespace std;

//This code is no longer being used since I switched from map<key, map<values>> to multimap< key, values>
/*
template <typename A, typename B>
void map_map_insert_helper( map<A, unordered_set<B>> &mp, const A &key, const B &item )
{
	map<A, unordered_set<B>>::iterator itx = mp.find( key );
	if( itx == mp.end() )
	{
		itx = ( mp.insert( make_pair( key, unordered_set<B>() ) ) ).first;
	}
	itx->second.insert( item );
}
*/



bool rule0_cbk( const list< LR_stack_item>& stk )
{
	return stk.back().current_value.d_bool;
}

bool rule1_cbk( const list< LR_stack_item>& stk )
{
	bool val1 = stk.back().current_value.d_bool;
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;//can't increment by more than 1 with lists
	prev_it++;
	bool val2 = prev_it->current_value.d_bool;
	return val1 | val2;
}

bool rule2_cbk( const list< LR_stack_item>& stk )
{
	return stk.back().current_value.d_bool;
}

bool rule3_cbk( const list< LR_stack_item>& stk )
{
	bool val1 = stk.back().current_value.d_bool;
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;//can't increment by more than 1 with lists
	prev_it++;
	bool val2 = prev_it->current_value.d_bool;
	return val1 & val2;

}

bool rule4_cbk( const list< LR_stack_item>& stk )
{
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;
	return prev_it->current_value.d_bool;
}

bool rule5_cbk( const list< LR_stack_item>& stk )
{
	return stk.back().current_value.d_bool;
}

bool rule6_cbk( const list< LR_stack_item>& stk )
{
	return stk.back().current_value.d_bool;
}



class incremental_parser
{
public:
	bool final_value;
	bool errors;

private:


	//simple SLR(1) parser. 

	vector< parser_rule > rules;

	list< LR_stack_item> item_stack; //using a list because we need access to all elements

#ifdef DEBUG_PARSER
	unordered_map< tokens, char > debug_map;
	unordered_map< nonterminals, char > debug_map2;
#endif

	//unordered_map could be substituted to improve performance
	vector< map< symbol, action_goto_table_item > > action_goto_table;

	map< symbol, tokens > follow_table;

	int offset_state;

	void debug_print_arr()
	{
#ifdef DEBUG_PARSER
		for( list<LR_stack_item>::iterator it = item_stack.begin(); it != item_stack.end(); ++it )
		{
			if( it->smb.nonterm_or_tok )
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
#endif
	}

public:
	incremental_parser()
	{
		offset_state = 0;

		//EBNF right-recursive notation.
		//I'm assuming the typical algebraic operator precedence:
		//brackets take precedence over multiplication, which in turn takes precedence over addition

		//S' -> ADD
		//ADD -> MULT | MULT + ADD
		//MULT -> VALUE | VALUE * MULT
		//VALUE -> ( ADD ) | bool

		//This expands to 15 SLR states. 
		//The states and the transitions are stored in action_goto_table
		//For deriving state transitions, see SLR parse table derivation

		//The rules are stored in "rules"

		init_debug_map();

		init_rules();

		init_action_goto_table_fresh();

		init_action_goto_table_precalculated();

		clear();
	}

private:

	void init_stack()
	{
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

	void init_debug_map()
	{
#ifdef DEBUG_PARSER
		debug_map.insert( make_pair( TK_PLUS, '+' ) );
		debug_map.insert( make_pair( TK_MULT, '*' ) );
		debug_map.insert( make_pair( TK_BOOL, 'B' ) );
		debug_map.insert( make_pair( TK_BROP, '(' ) );
		debug_map.insert( make_pair( TK_BRCL, ')' ) );
		debug_map.insert( make_pair( TK_END, '$' ) );

		debug_map2.insert( make_pair( EX_VALUE, 'V' ) );
		debug_map2.insert( make_pair( EX_MULT, 'M' ) );
		debug_map2.insert( make_pair( EX_ADD, 'A' ) );
		debug_map2.insert( make_pair( EX_EMPTY, '\\' ) );
#endif
	}

	void init_rules()
	{
		//the EBNF substitutions
		rules.resize( 7 );
		rules[0].result_exp = EX_S;
		rules[0].rhs.push_back( EX_ADD );
		rules[0].rhs.push_back( TK_END );
		rules[0].callback = rule6_cbk;

		rules[1].result_exp = EX_ADD;
		rules[1].rhs.push_back( EX_MULT );
		rules[1].callback = rule0_cbk;

		rules[2].result_exp = EX_ADD;
		rules[2].rhs.push_back( EX_MULT );
		rules[2].rhs.push_back( TK_PLUS );
		rules[2].rhs.push_back( EX_ADD );
		rules[2].callback = rule1_cbk;

		rules[3].result_exp = EX_MULT;
		rules[3].rhs.push_back( EX_VALUE );
		rules[3].callback = rule2_cbk;

		rules[4].result_exp = EX_MULT;
		rules[4].rhs.push_back( EX_VALUE );
		rules[4].rhs.push_back( TK_MULT );
		rules[4].rhs.push_back( EX_MULT );
		rules[4].callback = rule3_cbk;

		rules[5].result_exp = EX_VALUE;
		rules[5].rhs.push_back( TK_BROP );
		rules[5].rhs.push_back( EX_ADD );
		rules[5].rhs.push_back( TK_BRCL );
		rules[5].callback = rule4_cbk;

		rules[6].result_exp = EX_VALUE;
		rules[6].rhs.push_back( TK_BOOL );
		rules[6].callback = rule5_cbk;
	}

	void init_action_goto_table_fresh()
	{
		parser_action_goto_table_generator::init_action_goto_table_fresh( rules, action_goto_table );
	}

	void init_action_goto_table_precalculated()
	{
		parser_action_goto_table_generator::pregenerated_bool_calculator_table( action_goto_table );
	}

public:

	void clear()
	{
		item_stack.clear();
		init_stack();
		final_value = false;
		errors = false;
	}

	//for every new input token, run parser(). This is a helper function
	bool parser_t( symbol in, data_t data )
	{
		if( errors )
			return false;

		LR_stack_item top = item_stack.back();
		map< symbol, action_goto_table_item >::iterator it = action_goto_table[top.state].find( in );
		if( it == action_goto_table[top.state].end() )
		{
			//optional:
			//printf( "bad syntax, starting anew\n" );
			//clear();
			errors = true;
			return false;
		}

		action_goto_table_item itm = it->second;
		//TODO - add actual rule for accepting
		if( itm.reduce_rule == -1 ) //flag for accepting
		{
			data_t cur_val = item_stack.back().current_value;
			if( cur_val.type == DT_NONE )
				final_value = false;
			else if( cur_val.type == DT_BOOL )
				final_value = cur_val.d_bool;
			else
			{
				printf( "Logic error, incorrect data type returned" );
				final_value = false;
			}
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
			//alternatively, we could add an AST calculation stage
			//TODO - implement operations other than bool-bool interactions.
			bool new_value = false;
			new_value = ( *rule.callback )( item_stack );

			for( size_t i = 0; i < rule.rhs.size(); i++ )
			{
				item_stack.pop_back();
			}

			LR_stack_item old_state = item_stack.back();
			LR_stack_item new_state;

			new_state.insert_exp( rule.result_exp );
			new_state.state = action_goto_table[old_state.state][rule.result_exp].new_state;
			new_state.current_value = new_value;

			item_stack.push_back( new_state );

			debug_print_arr();
		}
		return reduced;
	}

	//run for every token from the lexer
	void parser( token in )
	{
		symbol s( in.name );
		while( parser_t( s, in.data_b ) )
			;
	}
};



//on "bad" input, this function will return false.
//one can check for parser.errors to see if input was parsed correctly
bool calculator_function( string in )
{
	incremental_lexer lex;
	incremental_parser parser;

	in.push_back( '\n' );

	for( size_t i = 0; i < in.length(); i++ )
	{
		possible_token t = lex.lexer( in[i] );
		if( !t.has_token )
		{
			continue;
		}
		parser.parser( t.tk );
	}
	return parser.final_value;
}

int _tmain( int argc, _TCHAR* argv[] )
{
	incremental_lexer lex;
	incremental_parser parser;

#ifdef TEST_PARSER
	//testing the calculator function:
	string input = "(0*(0+1)+(1))"; //should evaluate to true
	bool output = calculator_function( input );
	if( !output )
	{
		printf( "Failed test\n" );
		//return 0;
	}
#endif

	printf( "Enter X to exit\n" );

	for( char c = getchar();; c = getchar() )
	{
		if( c == 'X' )
		{
			break;
		}

		possible_token t = lex.lexer( c );
		if( !t.has_token )
		{
			continue;
		}

		parser.parser( t.tk );

		if( c == '\n' )
		{
			if( !parser.errors )
			{
				printf( "Result is %d\n", parser.final_value );
			}
			else
			{
				printf( "Invalid input\n" );
			}
			lex.clear();
			parser.clear();
		}

	}
	return 0;
}
