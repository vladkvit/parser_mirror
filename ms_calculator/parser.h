#pragma once
#include "lexer.h"

//it's convenient to have a datatype that can represent either a terminal or a nonterminal
struct symbol
{
	bool nonterm_or_tok; //if token then true, if nonterminal then false
	union //depending on nonterm_or_tok
	{
		tokens tk;
		nonterminals expr;
	};

	void insert_tok( tokens tki )
	{
		nonterm_or_tok = true;
		tk = tki;
	}
	void insert_exp( nonterminals ex )
	{
		nonterm_or_tok = false;
		expr = ex;
	}

	symbol() {}
	symbol( tokens tki ) : nonterm_or_tok( true ), tk( tki ) {}
	symbol( nonterminals ex ) : nonterm_or_tok( false ), expr( ex ) {}

	bool operator < ( const symbol& other ) const
	{
		if( other.nonterm_or_tok != nonterm_or_tok )
		{
			return nonterm_or_tok;
		}
		if( nonterm_or_tok )
		{
			return other.expr < expr;
		}
		return other.tk < tk;
	}

	bool operator==( const symbol &other ) const
	{
		if( nonterm_or_tok != other.nonterm_or_tok )
			return false;

		if( nonterm_or_tok == true )
			return ( tk == other.tk );
		else
			return ( expr == other.expr );
	}
};

template<>
class hash<symbol> {
public:
	size_t operator()( const symbol &c ) const
	{
		//when switching over to enum classes, this will have to change.
		// see http://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
		// and http://stackoverflow.com/questions/9646297/c11-hash-function-for-any-enum-type
		if( c.nonterm_or_tok )
			return hash<int>()( c.tk ) ^ ( int )c.nonterm_or_tok;
		else
			return hash<int>()( c.expr ) ^ ( int )c.nonterm_or_tok;
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

	void insert_tok( tokens tki )
	{
		smb.insert_tok( tki );
	}
	void insert_exp( nonterminals ex )
	{
		smb.insert_exp( ex );
	}
};

//stores a single grammar rule for the language
struct parser_rule
{
	nonterminals result_exp; //left side of the equation
	vector<symbol> rhs;
	bool( *callback )( const list< LR_stack_item>& );
};

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