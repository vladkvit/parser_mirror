//defines enum tokens and enum expressions

enum tokens
{
	TK_PLUS,
	TK_MULT,
	TK_BOOL,
	TK_BROP,
	TK_BRCL,
	TK_END //no-more-characters signal
};

enum expressions
{
	EX_VALUE,
	EX_MULT,
	EX_ADD,
	EX_EMPTY
};