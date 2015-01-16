#include "parser.h"
#include "user_parser_rules.h"
#include "action_goto_generation.h"
 
void incremental_parser::debug_print_arr()
{
#ifdef DEBUG_PARSER
	for( list<LR_stack_item>::iterator it = item_stack.begin(); it != item_stack.end(); ++it )
	{
		printf( "%c", debug_map[it->smb] );
		printf( "-%d", it->state );

		if( it->smb.nonterm_or_tok )
		{
			if( it->smb.tk == TK_BOOL )
			{
				printf( "-%d.b", it->current_value );
			}
		}
		else
		{
			if( it->current_value.type != DT_NONE )
				printf( "-%d.b", it->current_value );

		}
		printf( ", " );
	}
	printf( "\n" );
#endif
}

incremental_parser::incremental_parser()
{
	offset_state = 0;
	accepted = false;

	//EBNF right-recursive notation.
	//I'm assuming the typical algebraic operator precedence:
	//brackets take precedence over multiplication, which in turn takes precedence over addition

	//S' -> ADD
	//ADD -> MULT | MULT + ADD
	//MULT -> VALUE | VALUE * MULT
	//VALUE -> ( ADD ) | bool

	//This expands to 13 SLR states. 
	//The states and the transitions are stored in action_goto_table
	//For deriving state transitions, see SLR parse table derivation

	//The rules are stored in "rules"

	init_debug_map();

	init_rules(); //EBNF rules

	init_action_goto_table_fresh();

	//init_action_goto_table_precalculated();

	clear();
}

void incremental_parser::init_stack()
{
	LR_stack_item initial;
	initial.insert_exp( EX_EMPTY );
	initial.state = 0;
	shift( initial );

}

void incremental_parser::shift( LR_stack_item to_insert )
{
	item_stack.push_back( to_insert );
	
#ifdef DEBUG_PARSER
	printf( "shifted %c, ", debug_map[to_insert.smb] );
	debug_print_arr();
#endif

}

void incremental_parser::init_rules()
{
	user_rules( rules );
}

void incremental_parser::init_action_goto_table_fresh()
{
	parser_action_goto_table_generator::init_action_goto_table_fresh( rules, action_goto_table );
}

void incremental_parser::init_action_goto_table_precalculated()
{
	parser_action_goto_table_generator::pregenerated_bool_calculator_table( action_goto_table );
}

void incremental_parser::clear()
{
	item_stack.clear();
	init_stack();
	final_value = false;
	errors = false;
	accepted = false;
}

//for every new input token, run parser(). This is a helper function
bool incremental_parser::parser_t( symbol in, data_t data )
{
	if( errors )
		return false;

#ifdef DEBUG_PARSER
	printf( "Lookahead is %c\n", debug_map[in] );
#endif

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
		if( new_state.smb == symbol( EX_S ) )
		{
			assert( item_stack.size() == 1 );
			accepted = true;
			new_state.state = 0;
		}
		else
			new_state.state = action_goto_table[old_state.state][rule.result_exp].new_state;
		
		new_state.current_value = new_value;

		item_stack.push_back( new_state );

		//TODO make this nicer. Technically, the state lookup above finds garbage, although it will never be used.


		//TODO - add actual rule for accepting
		if( itm.reduce_rule == -1 || accepted ) //flag for accepting
		{
			/*data_t cur_val = item_stack.back().current_value;
			if( cur_val.type == DT_NONE )
				final_value = false;
			else if( cur_val.type == DT_BOOL )
				final_value = cur_val.d_bool;
			else
			{
				printf( "Logic error, incorrect data type returned" );
				final_value = false;
			}
			return false;*/
			final_value = new_value;
		}
		printf( "reduced by rule %d, ", itm.reduce_rule );
		debug_print_arr();
	}
	return reduced && !accepted;
}

//run for every token from the lexer
void incremental_parser::parser( token in )
{
	symbol s( in.name );
	while( parser_t( s, in.data_b ) )
		;
}
