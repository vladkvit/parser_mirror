#include "lexer.h"

incremental_lexer::incremental_lexer()
{
	lexer_rules( token_map );
}

void incremental_lexer::clear()
{}

//for every new input character, the lexer outputs zero or one token
//For our language, there is no need for state in the lexer, since all tokens are 1 char wide
possible_token incremental_lexer::lexer( char in )
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

