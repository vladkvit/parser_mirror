#pragma once
#include "parser.h"

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

class parser_action_goto_table_generator
{

public:
	static void pregenerated_bool_calculator_table( vector< map< symbol, action_goto_table_item > >& action_goto_table )
	{
		assert( action_goto_table.size() == 0 );

		int num_states = 15;
		action_goto_table.resize( num_states );
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

private:

	static unordered_multimap< nonterminals, int > build_lhs_rule_lookup( const vector< parser_rule >& rules )
	{
		unordered_multimap< nonterminals, int > rule_accel;

		for( size_t i = 0; i < rules.size(); i++ )
		{
			rule_accel.insert( make_pair( rules[i].result_exp, i ) );
		}
		return rule_accel;
	}

	//returns a lookup table for a symbol to its position(s) in the RHS of rules
	static unordered_multimap< symbol, pair<size_t, size_t> > build_rhs_rule_lookup( const vector< parser_rule >& rules )
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

	static unordered_set< tokens > calculate_first( const vector< parser_rule >& rules,
		const unordered_multimap< nonterminals, int > &lhs_accel, symbol NT,
		unordered_set< nonterminals >& visited )
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
					unordered_set< tokens > tks = calculate_first( rules, lhs_accel, child.expr, visited );

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
	static unordered_multimap<symbol, tokens> calculate_first_set( const vector< parser_rule >& rules, const unordered_multimap< nonterminals, int > &lhs_accel )
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

			unordered_set< tokens > first_symbol_set = calculate_first( rules, lhs_accel, root_key, visited );

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

	static unordered_set< tokens > calculate_follow(
		const vector< parser_rule >& rules,
		const unordered_multimap< symbol, pair< size_t, size_t> > &rhs_accel,
		const unordered_multimap< symbol, tokens > &first,
		nonterminals NT,
		unordered_set<nonterminals> &visited )
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
				unordered_set< tokens > extr_follow = calculate_follow( rules, rhs_accel, first, rules[rule_index].result_exp, visited );
				for( unordered_set< tokens >::const_iterator it = extr_follow.begin(); it != extr_follow.end(); ++it )
				{
					follow.insert( *it );
				}
			}
		}

		return follow;
	}

	static unordered_multimap<nonterminals, tokens> calculate_follow_set(
		const vector< parser_rule >& rules,
		const unordered_multimap< symbol, pair<size_t, size_t> > &rhs_accel,
		const unordered_multimap< symbol, tokens > &first )
	{
		unordered_multimap<nonterminals, tokens> follow;

		for( size_t i = 0; i < all_nonterminals.size(); i++ )
		{
			symbol root_key = symbol( all_nonterminals[i] );

			//DFS the rules graph.
			unordered_set< nonterminals > visited; //hash for looking at visited nodes.

			unordered_set< tokens > follow_symbol_set = calculate_follow( rules, rhs_accel, first, root_key.expr, visited );

			for( unordered_set< tokens >::iterator sit = follow_symbol_set.begin(); sit != follow_symbol_set.end(); ++sit )
			{
				follow.insert( make_pair( root_key.expr, *sit ) );
			}
		}

		assert( follow.size() > 0 );
		return follow;
	}

	static void calculate_action_goto_table()
	{
	}
public:
	static void init_action_goto_table_fresh( const vector< parser_rule >& rules, vector< map< symbol, action_goto_table_item > >& action_goto_table )
	{
		assert( action_goto_table.size() == 0 );

		unordered_multimap< nonterminals, int > lhs_accel = build_lhs_rule_lookup( rules );
		unordered_multimap< symbol, pair< size_t, size_t > > rhs_accel = build_rhs_rule_lookup( rules );
		unordered_multimap< symbol, tokens > first = calculate_first_set( rules, lhs_accel );
		unordered_multimap< nonterminals, tokens > follow = calculate_follow_set( rules, rhs_accel, first );
		calculate_action_goto_table();
	}
};