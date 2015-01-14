#pragma once
#include "parser.h"

//action-goto table generation needs to keep track of states
struct parser_generation_state
{
	multimap< int, int > rule_position_map;

	bool operator < ( const parser_generation_state& other ) const
	{
		map< int, int >::const_iterator it1 = rule_position_map.begin();
		map< int, int >::const_iterator it2 = other.rule_position_map.begin();

		if( rule_position_map.size() != other.rule_position_map.size() )
			return rule_position_map.size() < other.rule_position_map.size();

		for( ; it1 != rule_position_map.end(); ++it1, ++it2 )
		{
			if( it1->first != it2->first )
				return it1->first < it2->first;

			if( it1->second != it2->second )
				return it1->second < it2->second;

		}
		return false;
	}

	bool operator == ( const parser_generation_state& other ) const
	{
		if( *this < other || other < *this )
			return false;

		return true;
	}
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

		action_goto_table[2].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 1 ) ) );
		action_goto_table[2].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( false, 6 ) ) );
		action_goto_table[2].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 1 ) ) );

		action_goto_table[3].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( false, 7 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[3].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 3 ) ) );

		action_goto_table[4].insert( make_pair( symbol( TK_BROP ), action_goto_table_item( false, 4 ) ) );
		action_goto_table[4].insert( make_pair( symbol( TK_BOOL ), action_goto_table_item( false, 12 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_ADD ), action_goto_table_item( false, 8 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_MULT ), action_goto_table_item( false, 10 ) ) );
		action_goto_table[4].insert( make_pair( symbol( EX_VALUE ), action_goto_table_item( false, 11 ) ) );

		action_goto_table[5].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[5].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 6 ) ) );

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

		action_goto_table[9].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 2 ) ) );
		action_goto_table[9].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 2 ) ) );

		action_goto_table[10].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 1 ) ) );
		action_goto_table[10].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( false, 6 ) ) );
		action_goto_table[10].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 1 ) ) );

		action_goto_table[11].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 1 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( false, 7 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 3 ) ) );
		action_goto_table[11].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 3 ) ) );

		action_goto_table[12].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 6 ) ) );
		action_goto_table[12].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 6 ) ) );

		action_goto_table[13].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 4 ) ) );
		action_goto_table[13].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 4 ) ) );

		action_goto_table[14].insert( make_pair( symbol( TK_END ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_MULT ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_PLUS ), action_goto_table_item( true, 5 ) ) );
		action_goto_table[14].insert( make_pair( symbol( TK_BRCL ), action_goto_table_item( true, 5 ) ) );
	}

private:
	//given a nonterminal, finds all the nonterminal FIRSTs and the corresponding rules of that nonterminal.
	static void expand_rule( symbol in, const vector< parser_rule >& rules,
		const unordered_multimap< nonterminals, int > &lhs_accel,
		unordered_set<int> &rule_set )
	{
		if( in.nonterm_or_tok )
			return;

		queue<nonterminals> to_expand;
		unordered_set<nonterminals> visited; //technically we could check using rule_set, but having a "visited" simplifies the code
		to_expand.push( in.expr );
		while( to_expand.size() > 0 )
		{
			nonterminals e = to_expand.front();
			to_expand.pop();

			if( visited.count( e ) )
				continue;

			visited.insert( e );

			pair<unordered_multimap< nonterminals, int >::const_iterator,
				unordered_multimap< nonterminals, int >::const_iterator> window = lhs_accel.equal_range( e );

			for( ; window.first != window.second; ++window.first )
			{
				rule_set.insert( window.first->second );
				if( rules[window.first->second].rhs[0].nonterm_or_tok == false )
					to_expand.push( rules[window.first->second].rhs[0].expr );
			}
		}
		return;
	}

	static void build_lhs_rule_lookup( const vector< parser_rule >& rules, unordered_multimap< nonterminals, int > &rule_accel )
	{
		for( size_t i = 0; i < rules.size(); i++ )
		{
			rule_accel.insert( make_pair( rules[i].result_exp, i ) );
		}
	}

	//returns a lookup table for a symbol to its position(s) in the RHS of rules
	static void build_rhs_rule_lookup( const vector< parser_rule >& rules, unordered_multimap< symbol, pair<size_t, size_t> > &rule_accel )
	{
		for( size_t i = 0; i < rules.size(); i++ )
		{
			for( size_t j = 0; j < rules[i].rhs.size(); j++ )
			{
				rule_accel.insert( make_pair( rules[i].rhs[j], pair<size_t, size_t>( i, j ) ) );
			}
		}
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
	static void calculate_first_set( const vector< parser_rule >& rules, const unordered_multimap< nonterminals, int > &lhs_accel, unordered_multimap<symbol, tokens> &first )
	{
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

	static void calculate_follow_set(
		const vector< parser_rule >& rules,
		const unordered_multimap< symbol, pair<size_t, size_t> > &rhs_accel,
		const unordered_multimap< symbol, tokens > &first,
		unordered_multimap<nonterminals, tokens> &follow )
	{
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
	}


	static void calculate_states( const vector< parser_rule >& rules,
		const unordered_multimap< nonterminals, int > &lhs_accel,
		set<parser_generation_state*>& good_states,
		multimap< parser_generation_state*, parser_generation_state* >& children )
	{
		const size_t number_of_rules = rules.size();

		parser_generation_state* initial_state = new parser_generation_state();
		initial_state->rule_position_map.insert( pair<int, int>( 0, 0 ) );

		queue<parser_generation_state*> bfs_queue;
		bfs_queue.push( initial_state );

		multimap< parser_generation_state*, parser_generation_state* > parents;

		//while we have new possible states to analyze
		while( bfs_queue.size() > 0 )
		{
			parser_generation_state* inspect_state = bfs_queue.front();
			bfs_queue.pop();

			//expand state. state has rule ID and rhs position. For every rule in state, look at RHS
			//e.g. if we have S->A, we want to have S->A; +A->B; +B->x
			for( map<int, int>::const_iterator it = inspect_state->rule_position_map.begin(); it != inspect_state->rule_position_map.end(); ++it )
			{
				if( rules[it->first].rhs.size() <= it->second )
					continue;

				if( rules[it->first].rhs[it->second].nonterm_or_tok )
					continue;

				unordered_set<int> new_set;
				expand_rule( rules[it->first].rhs[it->second].expr, rules, lhs_accel, new_set );

				//we have a list of new blank rules that we should add to the state.
				for( unordered_set<int>::const_iterator it2 = new_set.begin(); it2 != new_set.end(); ++it2 )
				{
					bool found = false;
					for( auto range = inspect_state->rule_position_map.equal_range( *it2 ); range.first != range.second; ++range.first )
					{
						//The map does not guarantee uniqueness of key-value pairs. That's why we're checking if what we're about to insert already exists
						if( range.first->second == 0 ) //the number of items already found for a rule. 
						{
							found = true;
							break;
						}
					}
					if( !found )
						inspect_state->rule_position_map.insert( make_pair( *it2, 0 ) );
				}
			}

			//check if it's in good_states.
			bool found_state = false;
			for( set<parser_generation_state*>::const_iterator good_states_finder = good_states.begin(); good_states_finder != good_states.end(); ++good_states_finder )
			{
				if( *inspect_state == **good_states_finder )
				{
					found_state = true;
					break;
				}
			}
			if( found_state )
			{
				delete inspect_state;
				parents.erase( inspect_state );

				continue;
			}

			//if it isn't, push it into good_states and push all the children into queue
			good_states.insert( inspect_state );
			
			//find new states by going through rules and trying to incrementing them
			unordered_set<symbol> visited;
			for( multimap<int, int>::iterator it = inspect_state->rule_position_map.begin(); it != inspect_state->rule_position_map.end(); ++it )
			{
				parser_generation_state* new_state = new parser_generation_state( *inspect_state);

				multimap<int, int>::iterator it_new = new_state->rule_position_map.begin();
				while( true )
				{
					if( it_new->first == it->first && it_new->second == it->second )
						break;

					++it_new;
				}

				it_new->second++;

				if( it_new->second > rules[it_new->first].rhs.size() ) //rule outside bounds
				{
					delete new_state;
					continue;
				}

				if( visited.count( rules[it_new->first].rhs[it_new->second - 1] ) ) //already tried incrementing by this symbol
				{
					delete new_state;
					continue;
				}

				visited.insert( rules[it_new->first].rhs[it_new->second - 1] );

				parents.insert( make_pair( new_state, inspect_state ) );

				//we found a suitable rule to increment by a symbol. Go through the rest of the map and increment or remove all the other rules.
				for( multimap<int, int>::iterator it2 = new_state->rule_position_map.begin(); it2 != new_state->rule_position_map.end(); )
				{
					if( it2 == it_new )
					{
						++it2;
						continue;
					}

					//if a rule was already completed, remove it
					if( it2->second >= rules[it2->first].rhs.size() )
					{
						auto it2_new = it2;
						it2_new++;
						new_state->rule_position_map.erase( it2 ); //apparently map iterators are valid after map erase
						it2 = it2_new;

						++it2;
						continue;
					}

					//if a rule matches the symbol we're looking for
					if( rules[it_new->first].rhs[it_new->second - 1] == rules[it2->first].rhs[it2->second] )
					{
						it2->second++;
					}
					else //remove it otherwise
					{
						auto it2_new = it2;
						it2_new++;
						new_state->rule_position_map.erase( it2 ); //apparently map iterators are valid after map erase
						it2 = it2_new;
						continue;
					}

					if( it2 != new_state->rule_position_map.end() )
						++it2;
				}

				bfs_queue.push( new_state );
			}
		}

		for( multimap< parser_generation_state*, parser_generation_state* >::const_iterator parents_it = parents.begin(); parents_it != parents.end(); ++parents_it )
		{
			children.insert( make_pair( parents_it->second, parents_it->first ) );
		}
	}
public:

	static void debug_print_states( const set<parser_generation_state*> &states, 
		const vector< parser_rule >& rules, 
		multimap< parser_generation_state*, parser_generation_state* >& children )
	{
#ifdef DEBUG_PARSER
		int i = 0;
		for( set<parser_generation_state*>::const_iterator it = states.begin(); it != states.end(); ++it, i++ )
		{
			printf( "---State %d / %X---\n", i, *it );
			for( multimap< int, int >::const_iterator it2 = (*it)->rule_position_map.begin(); it2 != (*it)->rule_position_map.end(); ++it2 )
			{
				printf( "%c -> ", debug_map[symbol( rules[it2->first].result_exp) ] );

				int j = 0;
				for( ; j < rules[it2->first].rhs.size(); j++ )
				{
					if( j == it2->second )
						printf( ". " );

					printf( "%c ", debug_map[rules[it2->first].rhs[j] ] );
				}
				if( j == it2->second )
					printf( ". " );

				printf( "\n" );

			}
			printf( "Children states are :" );
			for( auto children_range = children.equal_range( *it ); children_range.first != children_range.second; ++children_range.first )
			{
				printf( "%X ", children_range.first->second );
			}
			printf( "\n" );
			printf( "\n" );
		}
#endif
	}


	static void calculate_action_goto_table( const vector< parser_rule >& rules,
		const unordered_multimap< nonterminals, int > &lhs_accel,
		const unordered_multimap< nonterminals, tokens > &follow,
		multimap< parser_generation_state*, parser_generation_state* > &children )
	{

	}

	static void init_action_goto_table_fresh( const vector< parser_rule >& rules, vector< map< symbol, action_goto_table_item > >& action_goto_table )
	{
		assert( action_goto_table.size() == 0 );

		unordered_multimap< nonterminals, int > lhs_accel;
		build_lhs_rule_lookup( rules, lhs_accel );
		unordered_multimap< symbol, pair< size_t, size_t > > rhs_accel;
		build_rhs_rule_lookup( rules, rhs_accel );
		unordered_multimap< symbol, tokens > first;
		calculate_first_set( rules, lhs_accel, first );
		unordered_multimap< nonterminals, tokens > follow;
		calculate_follow_set( rules, rhs_accel, first, follow );

		set<parser_generation_state*> states;
		multimap< parser_generation_state*, parser_generation_state* > children;
		calculate_states( rules, lhs_accel, states, children );
		debug_print_states( states, rules, children );
		calculate_action_goto_table( rules, lhs_accel, follow, children );
	}
};
