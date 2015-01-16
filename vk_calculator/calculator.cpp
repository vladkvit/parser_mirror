#include "stdafx.h"

//should have enum tokens, enum nonterminals
#include "user_nonterminals.h"
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
			if( parser.errors )
			{
				printf( "Invalid input\n" );
			}
			else if( !parser.accepted )
			{
				printf( "Incomplete or invalid input\n" );
			}
			else
			{
				printf( "Result is %d\n", parser.final_value );
				
			}
			lex.clear();
			parser.clear();
		}

	}
	return 0;
}
