all: lex gcc

lex: uccompiler.l
	lex uccompiler.l

gcc: lex.yy.c
	gcc lex.yy.c -o lexer
