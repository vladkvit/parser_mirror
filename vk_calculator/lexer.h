#pragma once

#include "stdafx.h"
#include "user_lex_rules.h"

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

	data_t() : type( DT_NONE ) {}
	data_t( bool b )
	{
		set( b );
	}

	data_t( int i )
	{
		set( i );
	}
	data_t( float f )
	{
		set( f );
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

	token( tokens t ) : name( t ) {}

	token( tokens t, bool b ) : name( t )
	{
		data_b.set( b );
	}
	void set_data_b( bool b )
	{
		data_b.set( b );
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
	incremental_lexer();

	void clear();

	//for every new input character, the lexer outputs zero or one token
	//For our language, there is no need for state in the lexer, since all tokens are 1 char wide
	possible_token lexer( char in );
};
