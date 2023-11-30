%{
// Frederico Ferreira 2021217116
// Andre Louro 2021232388

  #include <stdio.h>
  #include <string.h>
  #include "ast.h"
  #include "y.tab.h"

  #define YY_USER_ACTION { syn_line = line; syn_column = column; column += yyleng; }
  #define PASS_TOKEN() yylval.lexeme = strdup(yytext);

  extern struct node *program;

  struct flagsT flags = {0, 0};
  int line = 1, column = 1;
  int syn_line = 1, syn_column = 1;
  int line_temp = 1, column_temp = 1;
  int error_count = 0;
  
  char buffer[1024];
  int buffer_index = 0;

  int yyparse(void);
  void yyerror(char *);
%}

DIGIT   [0-9]
ALPHA   [a-zA-Z_]
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"
NEWLINE "\n"|"\r"|"\r\n"
ESCSEQUENCES \\(n|t|\\|\'|\"|[0-7]{1,3})
VALIDCHAR [^\'\n\r\\]|{ESCSEQUENCES}
DECIMALS ({DIGIT}+"."?{DIGIT}*|{DIGIT}*"."{DIGIT}+)

%x COMMENT
%%
"//".*                                      { ; }

"/*"                                        { BEGIN(COMMENT); column_temp = column-yyleng; line_temp=line; }
<COMMENT>.                                  { ; }
<COMMENT><<EOF>>                            { printf("Line %d, column %d: unterminated comment\n", line_temp, column_temp); BEGIN(INITIAL); } 
<COMMENT>{NEWLINE}                          { line++; column = 1; syn_line=line; syn_column=column; } 
<COMMENT>"*/"                               { BEGIN(INITIAL); }

char                                        { if(flags.l) printf("CHAR\n"); if(!flags.l)return CHAR; }
else                                        { if(flags.l) printf("ELSE\n"); if(!flags.l)return ELSE; }
while                                       { if(flags.l) printf("WHILE\n"); if(!flags.l)return WHILE; }
if                                          { if(flags.l) printf("IF\n"); if(!flags.l)return IF; }
int                                         { if(flags.l) printf("INT\n"); if(!flags.l)return INT; }
short                                       { if(flags.l) printf("SHORT\n"); if(!flags.l)return SHORT; }
double                                      { if(flags.l) printf("DOUBLE\n"); if(!flags.l)return DOUBLE; }
return                                      { if(flags.l) printf("RETURN\n"); if(!flags.l)return RETURN; }
void                                        { if(flags.l) printf("VOID\n"); if(!flags.l)return VOID; }
{RESERVED}                                  { if(flags.l) printf("RESERVED(%s)\n", yytext); if(!flags.l)return RESERVED;}
{ALPHA}({ALPHA}|{DIGIT})*                   { if(flags.l) printf("IDENTIFIER(%s)\n", yytext); PASS_TOKEN() if(!flags.l)return IDENTIFIER; }
{DIGIT}+                                    { if(flags.l) printf("NATURAL(%s)\n", yytext); PASS_TOKEN() if(!flags.l)return NATURAL; }
{DECIMALS}([Ee][+-]?{DIGIT}+)?              { if(flags.l) printf("DECIMAL(%s)\n", yytext); PASS_TOKEN() if(!flags.l)return DECIMAL; }

\'({VALIDCHAR})\'                           { if(flags.l) printf("CHRLIT(%s)\n", yytext); PASS_TOKEN() if(!flags.l)return CHRLIT; }
\'(({VALIDCHAR})*|([^\'\n\r])*)             { printf("Line %d, column %d: unterminated char constant\n", line, column-yyleng); }
\'({VALIDCHAR}|\\[^\n'])*\'                 { printf("Line %d, column %d: invalid char constant (%s)\n", line, column-yyleng, yytext); }

{NEWLINE}                                   { line++; column = 1; syn_line=line; syn_column=column;}
[ \t]                                       { syn_line=line; syn_column=column; }
"!"                                         { if(flags.l) printf("NOT\n"); if(!flags.l)return NOT;}
"&"                                         { if(flags.l) printf("BITWISEAND\n"); if(!flags.l)return BITWISEAND;}
"|"                                         { if(flags.l) printf("BITWISEOR\n"); if(!flags.l)return BITWISEOR;}
"^"                                         { if(flags.l) printf("BITWISEXOR\n"); if(!flags.l)return BITWISEXOR;}
"&&"                                        { if(flags.l) printf("AND\n"); if(!flags.l)return AND;}
"||"                                        { if(flags.l) printf("OR\n"); if(!flags.l)return OR;}
"=="                                        { if(flags.l) printf("EQ\n"); if(!flags.l)return EQ;}
"!="                                        { if(flags.l) printf("NE\n"); if(!flags.l)return NE;}
">="                                        { if(flags.l) printf("GE\n"); if(!flags.l)return GE;}
"<="                                        { if(flags.l) printf("LE\n"); if(!flags.l)return LE;}
">"                                         { if(flags.l) printf("GT\n"); if(!flags.l)return GT;}
"<"                                         { if(flags.l) printf("LT\n"); if(!flags.l)return LT;}
"{"                                         { if(flags.l) printf("LBRACE\n"); if(!flags.l)return LBRACE;}
"}"                                         { if(flags.l) printf("RBRACE\n"); if(!flags.l)return RBRACE;}
"("                                         { if(flags.l) printf("LPAR\n"); if(!flags.l)return LPAR;}
")"                                         { if(flags.l) printf("RPAR\n"); if(!flags.l)return RPAR;}
"="                                         { if(flags.l) printf("ASSIGN\n"); if(!flags.l)return ASSIGN;}
"*"                                         { if(flags.l) printf("MUL\n"); if(!flags.l)return MUL;}
","                                         { if(flags.l) printf("COMMA\n"); if(!flags.l)return COMMA;}
"/"                                         { if(flags.l) printf("DIV\n"); if(!flags.l)return DIV;}
"-"                                         { if(flags.l) printf("MINUS\n"); if(!flags.l)return MINUS;}
"%"                                         { if(flags.l) printf("MOD\n"); if(!flags.l)return MOD;}
"+"                                         { if(flags.l) printf("PLUS\n"); if(!flags.l)return PLUS;}
";"                                         { if(flags.l) printf("SEMI\n"); if(!flags.l)return SEMI;}

.                                           { printf("Line %d, column %d: unrecognized character (%s)\n", line, column-yyleng, yytext); }

%%
extern int yylex();

void yyerror(char *error) {
    error_count++;
    printf("Line %d, column %d: %s: %s\n", syn_line, syn_column, error,  yytext);
}

int main(int argc, char **argv) {
  if(argc == 2){
    if(strcmp(argv[1], "-l") == 0)
      flags.l = 1;
    if(strcmp(argv[1], "-t") == 0)
      flags.t = 1;
  }
  if(flags.l){
    yylex();    /* run the lexical analysis automaton */  
  }else{
    yyparse();
    if(flags.t){
      if(program != NULL && error_count == 0)
        show(program, 0);
      }
  }
  return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
  return 1;
}

