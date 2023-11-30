%{
  #include <stdio.h>
  #include <string.h>
  
  int flag = 0;
  int line = 1, column = 1;
  int line_temp = 1, column_temp = 1;
  char buffer[1024];
  int buffer_index = 0;

  char * to_upper_case(char * text) {
    for(int i = 0; text[i]; i++){
        text[i] = text[i]-'a'+'A';
      }
    return(text);
  }
%}

DIGIT   [0-9]
ALPHA   [a-zA-Z_]
UCRESERVED char|else|while|if|int|short|double|return|void
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"
NEWLINE "\n"|"\r"|"\r\n"
ESCSEQUENCES \\(n|t|\\|\'|\"|[0-7]{1,3})
VALIDCHAR [^\'\n\r\\]|{ESCSEQUENCES}
DECIMALS ({DIGIT}+"."?{DIGIT}*|{DIGIT}*"."{DIGIT}+)

%x COMMENT
%%
"//".*                                      { column += yyleng; }

"/*"                                        { BEGIN(COMMENT); column_temp = column; line_temp=line; column_temp += yyleng; }
<COMMENT>.                                  { column_temp += yyleng;}
<COMMENT><<EOF>>                            { printf("Line %d, column %d: unterminated comment\n", line, column); BEGIN(INITIAL);} 
<COMMENT>{NEWLINE}                          { line_temp++; column_temp = 1;} 
<COMMENT>"*/"                               { column=column_temp; line=line_temp; column += yyleng; BEGIN(INITIAL);}

{UCRESERVED}                                { if(flag) printf("%s\n", to_upper_case(yytext)); column += yyleng; }
{RESERVED}                                  { if(flag) printf("RESERVED(%s)\n", yytext); column += yyleng; }
{ALPHA}({ALPHA}|{DIGIT})*                  { if(flag) printf("IDENTIFIER(%s)\n", yytext); column += yyleng; }
{DIGIT}+                                    { if(flag) printf("NATURAL(%s)\n", yytext); column += yyleng; }
{DECIMALS}([Ee][+-]?{DIGIT}+)?              { if(flag) printf("DECIMAL(%s)\n", yytext); column += yyleng; }

\'({VALIDCHAR})\'                           { if(flag) printf("CHRLIT(%s)\n", yytext); column += yyleng; }
\'(({VALIDCHAR})*|([^\'\n\r])*)             { printf("Line %d, column %d: unterminated char constant\n", line, column); column += yyleng; }
\'({VALIDCHAR}|\\[^\n'])*\'                 { printf("Line %d, column %d: invalid char constant (%s)\n", line, column, yytext); column += yyleng; }

{NEWLINE}                                   { line++; column = 1; }
[ \t]                                       { column += yyleng; }
"!"                                         { if(flag) printf("NOT\n"); column += yyleng; }
"&"                                         { if(flag) printf("BITWISEAND\n"); column += yyleng; }
"|"                                         { if(flag) printf("BITWISEOR\n"); column += yyleng; }
"^"                                         { if(flag) printf("BITWISEXOR\n"); column += yyleng; }
"&&"                                        { if(flag) printf("AND\n"); column += yyleng; }
"||"                                        { if(flag) printf("OR\n"); column += yyleng; }
"=="                                        { if(flag) printf("EQ\n"); column += yyleng; }
"!="                                        { if(flag) printf("NE\n"); column += yyleng; }
">="                                        { if(flag) printf("GE\n"); column += yyleng; }
"<="                                        { if(flag) printf("LE\n"); column += yyleng; }
">"                                         { if(flag) printf("GT\n"); column += yyleng; }
"<"                                         { if(flag) printf("LT\n"); column += yyleng; }
"{"                                         { if(flag) printf("LBRACE\n"); column += yyleng; }
"}"                                         { if(flag) printf("RBRACE\n"); column += yyleng; }
"("                                         { if(flag) printf("LPAR\n"); column += yyleng; }
")"                                         { if(flag) printf("RPAR\n"); column += yyleng; }
"="                                         { if(flag) printf("ASSIGN\n"); column += yyleng; }
"*"                                         { if(flag) printf("MUL\n"); column += yyleng; }
","                                         { if(flag) printf("COMMA\n"); column += yyleng; }
"/"                                         { if(flag) printf("DIV\n"); column += yyleng; }
"-"                                         { if(flag) printf("MINUS\n"); column += yyleng; }
"%"                                         { if(flag) printf("MOD\n"); column += yyleng; }
"+"                                         { if(flag) printf("PLUS\n"); column += yyleng; }
";"                                         { if(flag) printf("SEMI\n"); column += yyleng; }

.                                           { printf("Line %d, column %d: unrecognized character (%s)\n", line, column, yytext); column += yyleng; }

%%
extern int yylex();



int main(int argc, char **argv) {
  if(argc == 2){
    if(strcmp(argv[1], "-l") == 0)
      flag = 1;
  }
  yylex();    /* run the lexical analysis automaton */  
  return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
  return 1;
}

