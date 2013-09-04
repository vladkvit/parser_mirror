#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class recursive_parse
{
	string p_str;
	int offset;

public:

	bool parse( string str )
	{
		p_str = str;
		offset = 0;
		return parseExp();
	}

	bool parseExp(  )
	{
		bool op, op1;
		op = parseFactor( );
		if( p_str.length() - offset > 0 )
		{
			if( p_str[offset] == '+' )
			{
				offset++;
				op1 = parseExp();
				op |= op1;
			}
		}
		return op;
	}

	bool parseFactor()
	{
		bool op, op1;
		op = parseTerm();
		if( p_str.length() - offset > 0 )
		{
			if( p_str[offset] == '*' )
			{
				offset++;
				op1 = parseFactor();
				op &= op1;
			}
		}
		return op;
	}
	bool parseTerm()
	{
		bool returnVal = 0;
		if( p_str.length() - offset > 0 )
		{
			if( p_str[offset] == '1' )
			{
				offset++;
				return 1;
			}
			else if( p_str[offset] == '0' )
			{
				offset++;
				return 0;
			}
			else if( p_str[offset] == '(' )
			{
				offset++;
				returnVal = parseExp();
				if( p_str[offset] != ')' )
				{
					printf( "No closing bracket detected\n" );
					return returnVal;
				}
				offset++;
				return returnVal;
			}
			else
			{
				printf( "Term error\n" );
			}
		}
		else
		{
			printf( "Incomplete expression\n" );
		}
		return returnVal;
	}

};

struct test_helper
{
	string inp_str;
	bool wanted_out;

	test_helper(){}
	test_helper( string s, bool b ) : inp_str( s ), wanted_out( b ){}
};

int _tmain( int argc, _TCHAR* argv [] )
{
	vector< test_helper > tests;
	tests.push_back( test_helper( "0+1", true ) );
	tests.push_back( test_helper( "1+1", true ) );
	tests.push_back( test_helper( "1+0", true ) );
	tests.push_back( test_helper( "1*0", false ) );
	tests.push_back( test_helper( "0*0", false ) );
	tests.push_back( test_helper( "0*1", false ) );
	tests.push_back( test_helper( "(0*1)*1", false ) );
	tests.push_back( test_helper( "(0+1)*1", true ) );
	tests.push_back( test_helper( "(0*1)+1", true ) );
	tests.push_back( test_helper( "1*(0*1)", false ) );
	tests.push_back( test_helper( "1*(0+1)", true ) );
	tests.push_back( test_helper( "1+(0*1)", true ) );
	tests.push_back( test_helper( "(((0+1))*0)", false ) );
	tests.push_back( test_helper( "(1)", true ) );

	for( int i = 0; i < tests.size(); i++ )
	{
		recursive_parse p;
		bool out = p.parse( tests[i].inp_str );
		if( out != tests[i].wanted_out )
		{
			printf( "Wrong output for %s - received %d, needed %d\n", 
				tests[i].inp_str.c_str(), out, tests[i].wanted_out );
		}
	}

	/*
	while( true )
	{
		getline( std::cin, line );
		recursive_parse p;
		int b = p.parse( line );
		printf( "result: %d\n", b );
	}
	*/
	return 0;
}