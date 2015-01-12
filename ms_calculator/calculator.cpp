#include "stdafx.h"

//should have enum tokens, enum nonterminals
#include "tokens_states_rules.h"
#include "lexer.h"
#include "parser.h"

//Boolean math expression evaluator, implemented as an SLR(1) parser
//Goals: easy to switch reading from cin to files
//It lexes, parses and evaluates input on the fly (streaming).
//It should handle very large (larger than memory space), but relatively "flat" input
//For lowering stack memory consumption in large, heavily-bracketed files, 
//the design can be updated to first find the AST children (deepest brackets) and evaluate them first
//If more performance or flexibility is needed, a parser library may be beneficial

using namespace std;

//state machine helper
struct action_goto_table_item
{
	bool shift_or_reduce; //false=shift, true=reduce
	union
	{
		int new_state;
		int reduce_rule;
	};

	action_goto_table_item() {}
	action_goto_table_item( bool sr, int st ) : shift_or_reduce( sr ), new_state( st ) {}
};

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

bool rule0_cbk( const list< LR_stack_item>& stk );

bool rule1_cbk( const list< LR_stack_item>& stk );

bool rule2_cbk( const list< LR_stack_item>& stk );

bool rule3_cbk( const list< LR_stack_item>& stk );

bool rule4_cbk( const list< LR_stack_item>& stk );

bool rule5_cbk( const list< LR_stack_item>& stk );

bool rule6_cbk( const list< LR_stack_item>& stk );

class incremental_parser
{
public:
	bool final_value;
	bool errors;

private:
	static const int NUM_STATES = 15;

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
		debug_map.insert( make_pair( TK_END, '/' ) );

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
		rules[0].result_exp = EX_ADD;
		rules[0].rhs.push_back( EX_MULT );
		rules[0].callback = rule0_cbk;

		rules[1].result_exp = EX_ADD;
		rules[1].rhs.push_back( EX_MULT );
		rules[1].rhs.push_back( TK_PLUS );
		rules[1].rhs.push_back( EX_ADD );
		rules[1].callback = rule1_cbk;

		rules[2].result_exp = EX_MULT;
		rules[2].rhs.push_back( EX_VALUE );
		rules[2].callback = rule2_cbk;

		rules[3].result_exp = EX_MULT;
		rules[3].rhs.push_back( EX_VALUE );
		rules[3].rhs.push_back( TK_MULT );
		rules[3].rhs.push_back( EX_MULT );
		rules[3].callback = rule3_cbk;

		rules[4].result_exp = EX_VALUE;
		rules[4].rhs.push_back( TK_BROP );
		rules[4].rhs.push_back( EX_ADD );
		rules[4].rhs.push_back( TK_BRCL );
		rules[4].callback = rule4_cbk;

		rules[5].result_exp = EX_VALUE;
		rules[5].rhs.push_back( TK_BOOL );
		rules[5].callback = rule5_cbk;

		rules[6].result_exp = EX_S;
		rules[6].rhs.push_back( EX_ADD );
		rules[6].rhs.push_back( TK_END );
		rules[6].callback = rule6_cbk;
	}

	unordered_multimap< nonterminals, int > build_lhs_rule_lookup() const
	{
		unordered_multimap< nonterminals, int > rule_accel;

		for( size_t i = 0; i < rules.size(); i++ )
		{
			rule_accel.insert( make_pair( rules[i].result_exp, i ) );
		}
		return rule_accel;
	}

	//returns a lookup table for a symbol to its position(s) in the RHS of rules
	unordered_multimap< symbol, pair<size_t, size_t> > build_rhs_rule_lookup() const
	{
		unordered_multimap< symbol, pair<size_t, size_t> > rule_accel;

		for( size_t i = 0; i < rules.size(); i++ )
		{
			for( size_t j = 0; j < rules[i].rhs.size(); j++ )
			{
				rule_accel.insert( make_pair( rules[i].rhs[j], pair<size_t, size_t>( i, j ) ) );
			}
		}
		return rule_accel;
	}

	unordered_set< tokens > calculate_first( const unordered_multimap< nonterminals, int > &lhs_accel, symbol NT, unordered_set< nonterminals >& visited ) const
	{
		unordered_set< tokens > first;

		if( NT.nonterm_or_tok )
		{
			first.insert( NT.tk );
			return first;
		}


		if( visited.count( NT.expr ) > 0 )
		{
			return first;
		}

		visited.insert( NT.expr );

		//get rules
		pair<unordered_multimap< nonterminals, int >::const_iterator,
			unordered_multimap< nonterminals, int >::const_iterator> range = lhs_accel.equal_range( NT.expr );

		//iterate over rules
		while( range.first != range.second )
		{
			vector<symbol>::const_iterator rule_it = rules[range.first->second].rhs.begin();

			while( rule_it != rules[range.first->second].rhs.end() )
			{
				symbol child = *rule_it;

				//if there is a production X->b, then First(X) includes b
				if( child.nonterm_or_tok ) //token
				{
					first.insert( child.tk );
					break;
				}
				//if there is a production X->Y1Y2..Yk, then FIRST(X) includes FIRST(Y1Y2..Yk)
				//FIRST(Y1Y2..Yk) is just FIRST( Y1 ) if FIRST( Y1 ) does not contain epsilon
				else //nonterminal
				{
					unordered_set< tokens > tks = calculate_first( lhs_accel, child.expr, visited );

					first.insert( tks.begin(), tks.end() );

					if( tks.count( TK_EPSILON ) == 0 )
						break;

					first.erase( TK_EPSILON );
				}

				rule_it++;
			}
			if( rule_it == rules[range.first->second].rhs.end() )
			{
				first.insert( TK_EPSILON );
			}
			range.first++;
		}

		return first;
	}

	//If we imagine the first set as a dependency graph, 
	//finding the FIRST set an be thought of as finding all the destinations from every point
	//lhs_accel outputs rule indeces when queried with the LHS of an expression
	unordered_multimap<symbol, tokens> calculate_first_set( const unordered_multimap< nonterminals, int > &lhs_accel ) const
	{
		unordered_multimap<symbol, tokens> first;

		//This implementation is the "naive foreach rule, do DFS" approach.
		//For speedup, look into algorithms here: http://en.wikipedia.org/wiki/Reachability or implement Dynamic Programming

		//we want to iterate over every nonterminal, but we can't iterate over an enum easily
		//so, instead, iterate over every rule and take the LHS. Every nonterminal is guaranteed to be on the LHS of some rule

		//for every rule, take LHS. See if there's an entry in "first" - if there is, we've looked at the nonterminal before
		//if we haven't - loop over every rule that has that nonterminal as LHS.

		for( size_t i = 0; i < all_nonterminals.size(); i++ )
		{
			symbol root_key = symbol( all_nonterminals[i] );

			//DFS the rules graph. 
			unordered_set< nonterminals > visited; //hash for looking at visited nodes.

			unordered_set< tokens > first_symbol_set = calculate_first( lhs_accel, root_key, visited );

			for( unordered_set< tokens >::const_iterator sit = first_symbol_set.begin(); sit != first_symbol_set.end(); ++sit )
			{
				first.insert( make_pair( root_key, *sit ) );
			}
		}

		for( size_t i = 0; i < all_terminals.size(); i++ )
		{
			first.insert( make_pair( symbol( all_terminals[i] ), all_terminals[i] ) );
		}

		return first;
	}

	unordered_set< tokens > calculate_follow(
		const unordered_multimap< symbol, pair< size_t, size_t> > &rhs_accel,
		const unordered_multimap< symbol, tokens > &first,
		nonterminals NT,
		unordered_set<nonterminals> &visited ) const
	{
		unordered_set< tokens > follow;

		if( visited.count( NT ) > 0 )
			return follow;

		visited.insert( NT );

		//find out where in the rules on the RHS the nonterminal occurs
		pair<unordered_multimap< symbol, pair< size_t, size_t> >::const_iterator,
			unordered_multimap< symbol, pair< size_t, size_t> >::const_iterator> range = rhs_accel.equal_range( NT );

		for( ; range.first != range.second; ++range.first )
		{
			//A->aB, then FOLLOW(B) incl. FOLLOW(A)
			//A->aBbc, then FOLLOW(B) incl. FIRST(b)
			//if FIRST(b) incl. eps, then FOLLOW(B) incl. FISRT(c)
			//if FIRST(b) incl. eps, then FOLLOW(B) incl. FOLLOW(A)

			size_t rule_index = range.first->second.first;
			size_t next_position = range.first->second.second + 1; //looking at the next symbol
			while( next_position < rules[rule_index].rhs.size() ) //our symbol isn't the last item in the RHS
			{
				//A->aBbc, then FOLLOW(B) incl. FIRST(b)
				symbol first_finder = rules[rule_index].rhs[next_position];
				pair<unordered_multimap< symbol, tokens >::const_iterator,
					unordered_multimap< symbol, tokens>::const_iterator> range2 = first.equal_range( first_finder );

				for( ; range2.first != range2.second; ++range2.first )
				{
					follow.insert( range2.first->second );
				}
				if( follow.count( TK_EPSILON ) == 0 )
					break;

				follow.erase( TK_EPSILON );

				next_position++;
			}
			if( next_position >= rules[rule_index].rhs.size() )
			{
				//A->aB, then FOLLOW(B) incl. FOLLOW(A)
				unordered_set< tokens > extr_follow = calculate_follow( rhs_accel, first, rules[rule_index].result_exp, visited );
				for( unordered_set< tokens >::const_iterator it = extr_follow.begin(); it != extr_follow.end(); ++it )
				{
					follow.insert( *it );
				}
			}
		}

		return follow;
	}

	unordered_multimap<nonterminals, tokens> calculate_follow_set(
		const unordered_multimap< symbol, pair<size_t, size_t> > &rhs_accel,
		const unordered_multimap< symbol, tokens > &first ) const
	{
		unordered_multimap<nonterminals, tokens> follow;

		for( size_t i = 0; i < all_nonterminals.size(); i++ )
		{
			symbol root_key = symbol( all_nonterminals[i] );

			//DFS the rules graph.
			unordered_set< nonterminals > visited; //hash for looking at visited nodes.

			unordered_set< tokens > follow_symbol_set = calculate_follow( rhs_accel, first, root_key.expr, visited );

			for( unordered_set< tokens >::iterator sit = follow_symbol_set.begin(); sit != follow_symbol_set.end(); ++sit )
			{
				follow.insert( make_pair( root_key.expr, *sit ) );
			}
		}

		assert( follow.size() > 0 );
		return follow;
	}

	void calculate_action_goto_table()
	{

	}

	void init_action_goto_table_fresh()
	{
		unordered_multimap< nonterminals, int > lhs_accel = build_lhs_rule_lookup();
		unordered_multimap< symbol, pair< size_t, size_t > > rhs_accel = build_rhs_rule_lookup();
		unordered_multimap< symbol, tokens > first = calculate_first_set( lhs_accel );
		unordered_multimap< nonterminals, tokens > follow = calculate_follow_set( rhs_accel, first );
		calculate_action_goto_table();
	}

	void init_action_goto_table_precalculated()
	{
		action_goto_table.resize( NUM_STATES );
		action_goto_table[0].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, -1 ) ) );
		action_goto_table[0].insert( make_pair( symbol( TK_BROP ), action_goto_table_item( false, 4 ) ) );
		action_goto_table[0].insert( make_pair( symbol( TK_BOOL ), action_goto_table_item( false, 5 ) ) );
		action_goto_table[0].insert( make_pair( symbol( EX_ADD ), action_goto_table_item( false, 1 ) ) );
		action_goto_table[0].insert( make_pair( symbol( EX_MULT ), action_goto_table_item( false, 2 ) ) );
		action_goto_table[0].insert( make_pair( symbol( EX_VALUE ), action_goto_table_item( false, 3 ) ) );

		action_goto_table[1].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, -1 ) ) );

		action_goto_table[2].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 0 ) ) );
		action_goto_table[2].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( false, 6 ) ) );
		action_goto_table[2].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 0 ) ) );

		action_goto_table[3].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 2 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( false, 7 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 2 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 2 ) ) );

		action_goto_table[4].insert( make_pair( symbol( TK_BROP ), action_goto_table_item( false, 4 ) ) );
		action_goto_table[4].insert( make_pair( symbol( TK_BOOL ), action_goto_table_item( false, 12 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_ADD ), action_goto_table_item( false, 8 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_MULT ), action_goto_table_item( false, 10 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_VALUE ), action_goto_table_item( false, 11 ) ) );

		action_goto_table[5].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 5 ) ) );

		action_goto_table[6].insert( make_pair( symbol( TK_BROP ), action_goto_table_item( false, 4 ) ) );
		action_goto_table[6].insert( make_pair( symbol( TK_BOOL ), action_goto_table_item( false, 12 ) ) );
		action_goto_table[6].insert( make_pair( symbol( EX_ADD ), action_goto_table_item( false, 9 ) ) );
		action_goto_table[6].insert( make_pair( symbol( EX_MULT ), action_goto_table_item( false, 10 ) ) );
		action_goto_table[6].insert( make_pair( symbol( EX_VALUE ), action_goto_table_item( false, 11 ) ) );

		action_goto_table[7].insert( make_pair( symbol( TK_BROP ), action_goto_table_item( false, 4 ) ) );
		action_goto_table[7].insert( make_pair( symbol( TK_BOOL ), action_goto_table_item( false, 12 ) ) );
		action_goto_table[7].insert( make_pair( symbol( EX_MULT ), action_goto_table_item( false, 13 ) ) );
		action_goto_table[7].insert( make_pair( symbol( EX_VALUE ), action_goto_table_item( false, 11 ) ) );

		action_goto_table[8].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( false, 14 ) ) );

		action_goto_table[9].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 1 ) ) );
		action_goto_table[9].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 1 ) ) );

		action_goto_table[10].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 0 ) ) );
		action_goto_table[10].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( false, 6 ) ) );
		action_goto_table[10].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 0 ) ) );

		action_goto_table[11].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 2 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( false, 7 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 2 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 2 ) ) );

		action_goto_table[12].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 5 ) ) );

		action_goto_table[13].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 3 ) ) );

		action_goto_table[14].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 4 ) ) );
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
