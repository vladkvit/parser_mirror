#pragma once

#include "parser.h"

#ifdef DEBUG_PARSER
unordered_map< symbol, char > debug_map;
#endif

void init_debug_map()
{
#ifdef DEBUG_PARSER
	debug_map.insert( make_pair( symbol( TK_PLUS ), '+' ) );
	debug_map.insert( make_pair( symbol( TK_MULT ), '*' ) );
	debug_map.insert( make_pair( symbol( TK_BOOL ), 'B' ) );
	debug_map.insert( make_pair( symbol( TK_BROP ), '(' ) );
	debug_map.insert( make_pair( symbol( TK_BRCL ), ')' ) );
	debug_map.insert( make_pair( symbol( TK_END ), '$' ) );

	debug_map.insert( make_pair( symbol( EX_VALUE ), 'V' ) );
	debug_map.insert( make_pair( symbol( EX_MULT ), 'M' ) );
	debug_map.insert( make_pair( symbol( EX_ADD ), 'A' ) );
	debug_map.insert( make_pair( symbol( EX_EMPTY ), '\\' ) );
	debug_map.insert( make_pair( symbol( EX_S ), 'S' ) );
#endif
}

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

static void user_rules( vector< parser_rule >& rules )
{
	//the EBNF substitutions
	rules.resize( 7 );
	rules[0].result_exp = EX_S;
	rules[0].rhs.push_back( EX_ADD );
	//rules[0].rhs.push_back( TK_END );
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