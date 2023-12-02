all: yacc lex gcc clean zip

run: yacc lex gcc clean
	./lexer -s < test.c

yacc: uccompiler.y
	yacc -d -v -t -g -Wcounterexamples --report=all uccompiler.y

lex: uccompiler.l
	lex uccompiler.l

gcc: lex.yy.c y.tab.c ast.c semantics.c
	gcc lex.yy.c y.tab.c ast.c semantics.c -Wall -Wextra -Wno-unused-function -o lexer 

clean:
	rm -f lex.yy.c y.tab.c y.tab.h y.output y.gv

zip:
	zip uccompiler.zip uccompiler.y uccompiler.l ast.h ast.c