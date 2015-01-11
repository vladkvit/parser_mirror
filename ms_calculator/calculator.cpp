#include "stdafx.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <iostream>
#include <list>
#include <queue>

//should have enum tokens, enum nonterminals
#include "tokens_states_rules.h"

//Boolean math expression evaluator, implemented as an SLR(1) parser
//Goals: easy to switch reading from cin to files
//It lexes, parses and evaluates input on the fly (streaming).
//It should handle very large (larger than memory space), but relatively "flat" input
//For lowering stack memory consumption in large, heavily-bracketed files, 
//the design can be updated to first find the AST children (deepest brackets) and evaluate them first
//If more performance or flexibility is needed, a parser library may be beneficial

using namespace std;

enum data_type
{
	DT_BOOL,
	DT_INT,
	DT_FLOAT,
	DT_NONE
};

struct data_t
{
	data_type type;

	union
	{
		bool d_bool;
		int d_int;
		double d_float;
	};

	void set( bool b )
	{
		type = DT_BOOL;
		d_bool = b;
	}
	void set( int i )
	{
		type = DT_INT;
		d_int = i;
	}
	void set( double d )
	{
		type = DT_FLOAT;
		d_float = d;
	}

	data_t() : type(DT_NONE) {}
	data_t(bool b)
	{
		set(b);
	}

	data_t(int i)
	{
		set(i);
	}
	data_t(float f)
	{
		set(f);
	}
};

//Helper struct for containing data for tokens such as booleans
struct token
{
	tokens name;

	//for passing the value of boolean. 
	data_t data_b;

	token()
	{
		data_b.type = DT_NONE;
	};

	token(tokens t) : name(t) {}

	token( tokens t, bool b ) : name( t )
	{
		data_b.set(b);
	}
	void set_data_b(bool b)
	{
		data_b.set(b);
	}
};

//Helper struct. If has_token is false, this is just a dummy struct
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

	void clear()
	{}

	//for every new input character, the lexer outputs zero or one token
	//For our language, there is no need for state in the lexer, since all tokens are 1 char wide
	possible_token lexer( char in )
	{
		possible_token out;
		unordered_map< char, tokens >::iterator it_finder = token_map.find( in );
		if( it_finder != token_map.end() )
		{
			out.set_token( token( it_finder->second ) );
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
			printf( "Unrecognized token '%c', ignoring.\n", in );
		}

		return out;
	}
};



struct symbol
{
	bool expr_or_tok; //if token then true, if expression then false
	union //depending on expr_or_tok
	{
		tokens tk;
		nonterminals expr;
	};

	void insert_tok( tokens tki )
	{
		expr_or_tok = true;
		tk = tki;
	}
	void insert_exp( nonterminals ex )
	{
		expr_or_tok = false;
		expr = ex;
	}

	symbol() {}
	symbol( tokens tki ) : expr_or_tok( true ), tk( tki ) {}
	symbol( nonterminals ex ) : expr_or_tok( false ), expr( ex ) {}

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

	bool operator==(const symbol &other) const
	{
		if (expr_or_tok != other.expr_or_tok)
			return false;

		if (expr_or_tok == true)
			return (tk == other.tk);
		else
			return (expr == other.expr);
	}
};

//parser stack item. In this parser design, token and state stacks are merged
struct LR_stack_item
{
	symbol smb;

	//since this parser doesn't need to keep a real AST,
	//this serves as an "accumulator"
	data_t current_value;

	int state;

	void insert_tok(tokens tki)
	{
		smb.insert_tok(tki);
	}
	void insert_exp(nonterminals ex)
	{
		smb.insert_exp(ex);
	}
};

//stores a single grammar rule for the language
struct parser_rule
{
	nonterminals result_exp; //left side of the equation
	list<symbol> rhs;
	bool(*callback)(const list< LR_stack_item>&);
};

//state machine helper
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

struct parser_state
{
	vector<int> symbol_positions;
};

template <typename A, typename B>
void map_map_insert_helper( map<A, unordered_set<B>> &mp, const A &key, const B &item )
{
	map<A, unordered_set<B>>::iterator itx = mp.find(key);
	if (itx == mp.end())
	{
		itx = (mp.insert(make_pair(key, unordered_set<B>()))).first;
	}
	itx->second.insert(item);
}

bool rule0_cbk(const list< LR_stack_item>& stk);

bool rule1_cbk(const list< LR_stack_item>& stk);

bool rule2_cbk(const list< LR_stack_item>& stk);

bool rule3_cbk(const list< LR_stack_item>& stk);

bool rule4_cbk(const list< LR_stack_item>& stk);

bool rule5_cbk(const list< LR_stack_item>& stk);

class incremental_parser
{
public:
	bool final_value;
	bool errors;

private:
	static const int NUM_STATES = 15;

	//simple SLR(1) parser. 

	vector< parser_rule > rules;
	list<parser_state> creating_parse_table_states;
	
	list< LR_stack_item> item_stack; //using a list because we need access to all elements

#ifdef DEBUG_PARSER
	unordered_map< tokens, char > debug_map;
	unordered_map< nonterminals, char > debug_map2;
#endif

	//unordered_map could be substituted to improve performance
	vector< map< symbol, parse_table_item > > parse_table;

	map< symbol, tokens > follow_table;

	int offset_state;

	void debug_print_arr()
	{
#ifdef DEBUG_PARSER
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
		//The states and the transitions are stored in parse_table
		//For deriving state transitions, see SLR parse table derivation

		//The rules are stored in "rules"

		init_debug_map();

		init_rules();

		init_parse_table_fresh();

		init_parse_table_precalculated();
		
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
		debug_map.insert(make_pair(TK_PLUS, '+'));
		debug_map.insert(make_pair(TK_MULT, '*'));
		debug_map.insert(make_pair(TK_BOOL, 'B'));
		debug_map.insert(make_pair(TK_BROP, '('));
		debug_map.insert(make_pair(TK_BRCL, ')'));
		debug_map.insert(make_pair(TK_END, '/'));

		debug_map2.insert(make_pair(EX_VALUE, 'V'));
		debug_map2.insert(make_pair(EX_MULT, 'M'));
		debug_map2.insert(make_pair(EX_ADD, 'A'));
		debug_map2.insert(make_pair(EX_EMPTY, '\\'));
#endif
	}

	void init_rules()
	{
		//the EBNF substitutions
		rules.resize(6);
		rules[0].result_exp = EX_ADD;
		rules[0].rhs.push_back( EX_MULT );
		rules[0].callback = rule0_cbk;

		rules[1].result_exp = EX_ADD;
		rules[1].rhs.push_back(EX_MULT);
		rules[1].rhs.push_back(TK_PLUS);
		rules[1].rhs.push_back(EX_ADD);
		rules[1].callback = rule1_cbk;

		rules[2].result_exp = EX_MULT;
		rules[2].rhs.push_back(EX_VALUE);
		rules[2].callback = rule2_cbk;

		rules[3].result_exp = EX_MULT;
		rules[3].rhs.push_back(EX_VALUE);
		rules[3].rhs.push_back(TK_MULT);
		rules[3].rhs.push_back(EX_MULT);
		rules[3].callback = rule3_cbk;

		rules[4].result_exp = EX_VALUE;
		rules[4].rhs.push_back(TK_BROP);
		rules[4].rhs.push_back(EX_ADD);
		rules[4].rhs.push_back(TK_BRCL);
		rules[4].callback = rule4_cbk;

		rules[5].result_exp = EX_VALUE;
		rules[5].rhs.push_back( TK_BOOL );
		rules[5].callback = rule5_cbk;
	}

	unordered_multimap< nonterminals, int > build_lhs_rule_lookup()
	{
		unordered_multimap< nonterminals, int > rule_accel;

		for (size_t i = 0; i < rules.size(); i++)
		{
			rule_accel.insert(make_pair(rules[i].result_exp, i));
		}
		return rule_accel;
	}

	/*unordered_multimap< symbol, int > build_rhs_rule_lookup()
	{
		unordered_multimap< symbol, int > rule_accel;

		for (size_t i = 0; i < rules.size(); i++)
		{
			rule_accel.insert(make_pair(rules[i].rhs.back(), i));
		}
		return rule_accel;
	}*/

	unordered_set< tokens > calculate_first(const unordered_multimap< nonterminals, int > &lhs_accel, nonterminals NT, unordered_set< nonterminals >& visited  )
	{
		unordered_set< tokens > first;


		

		if (visited.count( NT ) > 0)
		{
			return first;
		}

		visited.insert( NT );

		//get rules
		pair<unordered_multimap< nonterminals, int >::const_iterator,
			unordered_multimap< nonterminals, int >::const_iterator> range = lhs_accel.equal_range(NT);

		//iterate over rules
		while (range.first != range.second)
		{
			list<symbol>::iterator rule_it = rules[range.first->second].rhs.begin();

			while (rule_it != rules[range.first->second].rhs.end())
			{
				symbol child = *rule_it;

				//if there is a production X->b, then First(X) includes b
				if (child.expr_or_tok) //token
				{
					first.insert(child.tk);
					break;
				}
				//if there is a production X->Y1Y2..Yk, then FIRST(X) includes FIRST(Y1Y2..Yk)
				//FIRST(Y1Y2..Yk) is just FIRST( Y1 ) if FIRST( Y1 ) does not contain epsilon
				else //nonterminal
				{
					unordered_set< tokens > tks = calculate_first(lhs_accel, child.expr, visited);

					first.insert(tks.begin(), tks.end());

					if (tks.count(TK_EPSILON) == 0)
						break;

					first.erase(TK_EPSILON);
					
				}
				
				rule_it++;
			}
			if (rule_it == rules[range.first->second].rhs.end())
			{
				first.insert(TK_EPSILON);
			}
			range.first++;
		}


		return first;
	}

	//If we imagine the first set as a dependency graph, 
	//finding the FIRST set an be thought of as finding all the destinations from every point
	//lhs_accel outputs rule indeces when queried with the LHS of an expression
	unordered_multimap< nonterminals, tokens> calculate_first_set( const unordered_multimap< nonterminals, int > &lhs_accel )
	{

		unordered_multimap<nonterminals, tokens> first;
		
		//This implementation is the "naive foreach rule, do DFS" approach.
		//For speedup, look into algorithms here: http://en.wikipedia.org/wiki/Reachability or implement Dynamic Programming

		//we want to iterate over every nonterminal, but we can't iterate over an enum easily
		//so, instead, iterate over every rule and take the LHS. Every nonterminal is guaranteed to be on the LHS of some rule
		
		//for every rule, take LHS. See if there's an entry in "first" - if there is, we've looked at the nonterminal before
		//if we haven't - loop over every rule that has that nonterminal as LHS.

		for (size_t i = 0; i < rules.size(); i++)
		{
			nonterminals root_key = rules[i].result_exp;
			if (first.count(root_key) > 0)
				continue;


			//DFS the rules graph. 
			//queue< nonterminals > firsts; //holds nonterminals whose firsts should be part of "first"
			unordered_set< nonterminals > visited; //hash for looking at visited nodes.

			//firsts.push(root_key);
			unordered_set< tokens > first_symbol_set = calculate_first(lhs_accel, root_key, visited);

			for (unordered_set< tokens >::iterator sit = first_symbol_set.begin(); sit != first_symbol_set.end(); ++sit)
			{
				first.insert(make_pair(root_key, *sit));
			}
		}

		return first;
	}

	/*map<symbol, unordered_set<tokens>> calculate_follow(const unordered_multimap< nonterminals, int > &lhs_accel,
		const unordered_multimap< symbol, int > &rhs_accel,
		const unordered_multimap<nonterminals, tokens> &first )
	{
		
		map<symbol, unordered_set<tokens>> follow;
		
		for (size_t i = 0; i < rules.size(); i++)
		{
			parser_rule tmp_rule = rules[i];

			list<symbol>::const_iterator it = tmp_rule.rhs.begin();
			list<symbol>::const_iterator it2 = it;
			it2++;
			for ( ; it2 != tmp_rule.rhs.end(); ++it, ++it2)
			{
				if (it2->expr_or_tok) //token
				{
					map_map_insert_helper<symbol, tokens>(follow, *it, it2->tk);
				}
				else
				{
					pair<unordered_multimap< nonterminals, tokens >::const_iterator,
						unordered_multimap< nonterminals, tokens >::const_iterator> first_range = first.equal_range(it2->expr);

					for (; first_range.first != first_range.second; first_range.first++)
					{
						map_map_insert_helper<symbol, tokens>(follow, *it, first_range.first->second);
					}
				}
				symbol tmp_symb(*it);
			}
			
			//at this point, "it" should be pointing to the last element of the list

			queue<symbol> bfs_queue;
			set<symbol> visited_set;
			bfs_queue.push(*it);

			while (bfs_queue.size() > 0)
			{
				symbol end_symbol = bfs_queue.front();
				bfs_queue.pop();

				if (visited_set.count(end_symbol) > 0)
					continue;

				visited_set.insert(end_symbol);

				pair< unordered_multimap< symbol, int >::const_iterator,
					unordered_multimap< symbol, int >::const_iterator > start_rules = rhs_accel.equal_range(end_symbol);

				for (; start_rules.first != start_rules.second; start_rules.first++)
				{
					nonterminals lhs = rules[start_rules.first->second].result_exp;

					
					//map_map_insert_helper<symbol, tokens>(follow, *it, );
				}

			}
		}

		return follow;
	}*/

	void calculate_parse_table()
	{

	}

	void init_parse_table_fresh()
	{
		unordered_multimap< nonterminals, int > lhs_accel = build_lhs_rule_lookup();
		//unordered_multimap< symbol, int > rhs_accel = build_rhs_rule_lookup();
		unordered_multimap<nonterminals, tokens> first = calculate_first_set(lhs_accel);
		//map<symbol, unordered_set<tokens>> follow = calculate_follow(lhs_accel, rhs_accel, first );
		calculate_parse_table();

		int j = 1;
	}

	void init_parse_table_precalculated()
	{
		parse_table.resize(NUM_STATES);
		parse_table[0].insert(make_pair(symbol(TK_END), parse_table_item(true, -1)));
		parse_table[0].insert(make_pair(symbol(TK_BROP), parse_table_item(false, 4)));
		parse_table[0].insert(make_pair(symbol(TK_BOOL), parse_table_item(false, 5)));
		parse_table[0].insert(make_pair(symbol(EX_ADD), parse_table_item(false, 1)));
		parse_table[0].insert(make_pair(symbol(EX_MULT), parse_table_item(false, 2)));
		parse_table[0].insert(make_pair(symbol(EX_VALUE), parse_table_item(false, 3)));

		parse_table[1].insert(make_pair(symbol(TK_END), parse_table_item(true, -1)));

		parse_table[2].insert(make_pair(symbol(TK_END), parse_table_item(true, 0)));
		parse_table[2].insert(make_pair(symbol(TK_PLUS), parse_table_item(false, 6)));
		parse_table[2].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 0)));

		parse_table[3].insert(make_pair(symbol(TK_END), parse_table_item(true, 2)));
		parse_table[3].insert(make_pair(symbol(TK_MULT), parse_table_item(false, 7)));
		parse_table[3].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 2)));
		parse_table[3].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 2)));

		parse_table[4].insert(make_pair(symbol(TK_BROP), parse_table_item(false, 4)));
		parse_table[4].insert(make_pair(symbol(TK_BOOL), parse_table_item(false, 12)));
		parse_table[4].insert(make_pair(symbol(EX_ADD), parse_table_item(false, 8)));
		parse_table[4].insert(make_pair(symbol(EX_MULT), parse_table_item(false, 10)));
		parse_table[4].insert(make_pair(symbol(EX_VALUE), parse_table_item(false, 11)));

		parse_table[5].insert(make_pair(symbol(TK_END), parse_table_item(true, 5)));
		parse_table[5].insert(make_pair(symbol(TK_MULT), parse_table_item(true, 5)));
		parse_table[5].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 5)));
		parse_table[5].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 5)));

		parse_table[6].insert(make_pair(symbol(TK_BROP), parse_table_item(false, 4)));
		parse_table[6].insert(make_pair(symbol(TK_BOOL), parse_table_item(false, 12)));
		parse_table[6].insert(make_pair(symbol(EX_ADD), parse_table_item(false, 9)));
		parse_table[6].insert(make_pair(symbol(EX_MULT), parse_table_item(false, 10)));
		parse_table[6].insert(make_pair(symbol(EX_VALUE), parse_table_item(false, 11)));

		parse_table[7].insert(make_pair(symbol(TK_BROP), parse_table_item(false, 4)));
		parse_table[7].insert(make_pair(symbol(TK_BOOL), parse_table_item(false, 12)));
		parse_table[7].insert(make_pair(symbol(EX_MULT), parse_table_item(false, 13)));
		parse_table[7].insert(make_pair(symbol(EX_VALUE), parse_table_item(false, 11)));

		parse_table[8].insert(make_pair(symbol(TK_BRCL), parse_table_item(false, 14)));

		parse_table[9].insert(make_pair(symbol(TK_END), parse_table_item(true, 1)));
		parse_table[9].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 1)));

		parse_table[10].insert(make_pair(symbol(TK_END), parse_table_item(true, 0)));
		parse_table[10].insert(make_pair(symbol(TK_PLUS), parse_table_item(false, 6)));
		parse_table[10].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 0)));

		parse_table[11].insert(make_pair(symbol(TK_END), parse_table_item(true, 2)));
		parse_table[11].insert(make_pair(symbol(TK_MULT), parse_table_item(false, 7)));
		parse_table[11].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 2)));
		parse_table[11].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 2)));

		parse_table[12].insert(make_pair(symbol(TK_END), parse_table_item(true, 5)));
		parse_table[12].insert(make_pair(symbol(TK_MULT), parse_table_item(true, 5)));
		parse_table[12].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 5)));
		parse_table[12].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 5)));

		parse_table[13].insert(make_pair(symbol(TK_END), parse_table_item(true, 3)));
		parse_table[13].insert(make_pair(symbol(TK_MULT), parse_table_item(true, 3)));
		parse_table[13].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 3)));
		parse_table[13].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 3)));

		parse_table[14].insert(make_pair(symbol(TK_END), parse_table_item(true, 4)));
		parse_table[14].insert(make_pair(symbol(TK_MULT), parse_table_item(true, 4)));
		parse_table[14].insert(make_pair(symbol(TK_PLUS), parse_table_item(true, 4)));
		parse_table[14].insert(make_pair(symbol(TK_BRCL), parse_table_item(true, 4)));
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
		map< symbol, parse_table_item >::iterator it = parse_table[top.state].find( in );
		if( it == parse_table[top.state].end() )
		{
			//optional:
			//printf( "bad syntax, starting anew\n" );
			//clear();
			errors = true;
			return false;
		}

		parse_table_item itm = it->second;
		//TODO - add actual rule for accepting
		if( itm.reduce_rule == -1 ) //flag for accepting
		{
			data_t cur_val = item_stack.back().current_value;
			if (cur_val.type == DT_NONE)
				final_value = false;
			else if (cur_val.type == DT_BOOL)
				final_value = cur_val.d_bool;
			else
			{
				printf("Logic error, incorrect data type returned");
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
			new_value = (*rule.callback)(item_stack);

			for( size_t i = 0; i < rule.rhs.size(); i++ )
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

	//run for every token from the lexer
	void parser( token in )
	{
		symbol s( in.name );
		while( parser_t( s, in.data_b ) )
			;
	}
};


bool rule0_cbk( const list< LR_stack_item>& stk)
{
	return stk.back().current_value.d_bool;
}

bool rule1_cbk(const list< LR_stack_item>& stk)
{
	bool val1 = stk.back().current_value.d_bool;
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;//can't increment by more than 1 with lists
	prev_it++;
	bool val2 = prev_it->current_value.d_bool;
	return val1 | val2;
}

bool rule2_cbk(const list< LR_stack_item>& stk)
{
	return stk.back().current_value.d_bool;
}

bool rule3_cbk(const list< LR_stack_item>& stk)
{
	bool val1 = stk.back().current_value.d_bool;
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;//can't increment by more than 1 with lists
	prev_it++;
	bool val2 = prev_it->current_value.d_bool;
	return val1 & val2;

}

bool rule4_cbk(const list< LR_stack_item>& stk)
{
	list< LR_stack_item>::const_reverse_iterator prev_it = stk.rbegin();
	prev_it++;
	return prev_it->current_value.d_bool;
}

bool rule5_cbk(const list< LR_stack_item>& stk)
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

int _tmain(int argc, _TCHAR* argv[])
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

	for( char c = getchar(); ; c = getchar() )
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
