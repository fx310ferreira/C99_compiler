all: yacc lex gcc clean zip

run: yacc lex gcc clean
	./lexer < test.in > test.ll

yacc: uccompiler.y
	yacc -d -v -t -g -Wcounterexamples --report=all uccompiler.y

lex: uccompiler.l
	lex uccompiler.l

gcc: *.c
	gcc *.c -Wall -Wextra -o lexer 

clean:
	rm -f lex.yy.c y.tab.c y.tab.h y.output y.gv

zip:
	zip uccompiler.zip uccompiler.y uccompiler.l *.h *.c