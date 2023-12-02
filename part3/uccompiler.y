%{
// Frederico Ferreira 2021217116
// Andre Louro 2021232388

#include "ast.h"
#include <stdlib.h>

int yylex(void);
void yyerror(char *);

struct node *program;
struct node *type_spec;
struct node_list *aux;
%}

%union{ 
    char *lexeme;
    struct node *node;
    struct node_list *node_list;
}

%token LBRACE RBRACE LPAR RPAR SEMI CHAR INT DOUBLE VOID SHORT ASSIGN COMMA PLUS MINUS MUL DIV MOD EQ NE 
GT GE LT LE AND OR BITWISEAND BITWISEOR BITWISEXOR NOT IF ELSE WHILE RETURN RESERVED
%token<lexeme> IDENTIFIER CHRLIT DECIMAL NATURAL
%type<node> functionsAndDeclarations functionDefinition functionBody 
typeSpec functionDeclaration parameterDeclaration 

%type<node_list> declarator declaratorList declaration functionDeclarator parameterList declarationsAndStatements 
statementList statement statementError exprList expr exprComma

%nonassoc LOW
%nonassoc ELSE

%left COMMA
%right ASSIGN
%left OR
%left AND
%left BITWISEOR
%left BITWISEXOR
%left BITWISEAND
%left EQ NE 
%left GT GE LT LE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT
%left HIGH

%locations
%%

 functionsAndDeclarations: declaration                                  { $$ = program = newNode(Program, NULL, 0, 0);
                                                                          addChildren(program, $1); }
                         | functionsAndDeclarations declaration         { addChildren(program, $2); }
                         | functionDefinition                           { $$ = program = newNode(Program, NULL, 0, 0);
                                                                         addChild(program, $1); }
                         | functionsAndDeclarations functionDefinition  { addChild(program, $2); }
                         | functionDeclaration                          { $$ = program = newNode(Program, NULL, 0, 0);
                                                                          addChild(program, $1); }
                         | functionsAndDeclarations functionDeclaration { addChild(program, $2); }
                         ;

functionDefinition: typeSpec functionDeclarator functionBody           { $$ = newNode(FunctionDefinition, NULL, 0, 0);
                                                                         addChild($$, $1);
                                                                         addChildren($$, $2);
                                                                         addChild($$, $3); }
                  ;

functionBody: LBRACE declarationsAndStatements RBRACE                   { $$ = newNode(FuncBody, NULL, 0, 0);
                                                                         if($2 != NULL){addChildren($$, $2);} }
            | LBRACE RBRACE                                             { $$ = newNode(FuncBody, NULL, 0, 0); }
            ;

declarationsAndStatements: declarationsAndStatements declaration        { if($1 != NULL){$$ = $1;  concat($$, $2);}
                                                                          else $$ = $2; }
                         | declaration                                  { $$ = $1; }
                         | declarationsAndStatements statement          { if($1 != NULL){$$ = $1;  concat($$, $2);}
                                                                          else $$ = $2; }
                         | statement                                    { $$ = $1;}
                         ;
functionDeclaration: typeSpec functionDeclarator SEMI                   { $$ = newNode(FunctionDeclaration, NULL, 0, 0); 
                                                                          addChild($$, $1);
                                                                          addChildren($$, $2); }
                   ;

functionDeclarator: IDENTIFIER LPAR parameterList RPAR                  { $$ = newNodeList(newNode(Identifier, $1, @1.first_line, @1.first_column));
                                                                          concat($$, $3); }
                  ;
parameterList: parameterDeclaration                                     { $$ = newNodeList(newNode(ParamList, NULL, 0, 0));
                                                                         addChild($$->node, $1); }
             | parameterList COMMA parameterDeclaration                 { $$ = $1; addChild($$->node, $3); }
             ;

parameterDeclaration: typeSpec IDENTIFIER                               { $$ = newNode(ParamDeclaration, NULL, 0, 0);
                                                                         addChild($$, $1);
                                                                         addChild($$, newNode(Identifier, $2, @2.first_line, @2.first_column));}
                    | typeSpec                                          { $$ = newNode(ParamDeclaration, NULL, 0, 0);
                                                                         addChild($$, $1); }
                    ;

declaration: typeSpec declaratorList SEMI                               { $$ = $2;
                                                                          type_spec = $1 ; } 
           | error SEMI                                                 { $$ = newNodeList(newNode(Error, NULL, 0, 0)); }
           ;

declaratorList: declarator                                              { $$ = $1; }
              | declaratorList COMMA declarator                         { concat($1, $3);}
              ;

declarator: IDENTIFIER                                                  { $$ = newNodeList(newNode(Declaration, NULL, 0, 0));
                                                                          addChild($$->node, type_spec);   
                                                                          addChild($$->node, newNode(Identifier, $1, @1.first_line, @1.first_column)); }
          | IDENTIFIER ASSIGN exprList                                  { $$ = newNodeList(newNode(Declaration, NULL, 0, 0));
                                                                          addChild($$->node, type_spec);
                                                                          addChild($$->node, newNode(Identifier, $1, @1.first_line, @1.first_column));
                                                                          addChildren($$->node, $3);}
          ;

typeSpec: INT                                                           { $$ = type_spec = newNode(Int, NULL, @1.first_line, @1.first_column); }
        | DOUBLE                                                        { $$ = type_spec = newNode(Double, NULL, @1.first_line, @1.first_column); }
        | CHAR                                                          { $$ = type_spec = newNode(Char, NULL, @1.first_line, @1.first_column); }
        | VOID                                                          { $$ = type_spec = newNode(Void, NULL, @1.first_line, @1.first_column); }
        | SHORT                                                         { $$ = type_spec = newNode(Short, NULL, @1.first_line, @1.first_column); }
        ;

statementError: statement                                               { $$ = $1;}
              | error SEMI                                              { $$ = newNodeList(newNode(Error, NULL, 0, 0)); }
              ;

statementList: statementError                                           { $$ = newNodeList(newNode(StatList, NULL, 0, 0));
                                                                          if($1 != NULL) addChild($$->node, $1->node);}
             | statementList statementError                             { $$ = $1; if($2 != NULL) addChild($$->node, $2->node); }
             ;

statement: SEMI                                                         { $$ = NULL; }
         | exprComma SEMI                                                { $$ = $1; }
         | LBRACE statementList RBRACE                                  { if($2->node->children->next == NULL) $$ = NULL; 
                                                                          else{
                                                                            $$ = $2;
                                                                            int count = 0;
                                                                            struct node_list *aux = $$->node->children;
                                                                            while((aux=aux->next) != NULL){
                                                                                count++;
                                                                                if (count == 2) break;
                                                                            }
                                                                            if(count <= 1) $$ = newNodeList($2->node->children->next->node);
                                                                          }
                                                                          }
         | LBRACE RBRACE                                                { $$ = NULL; }
         | IF LPAR exprComma RPAR statement %prec LOW                    { $$ = newNodeList(newNode(If, NULL, @1.first_line, @1.first_column)); 
                                                                          addChildren($$->node, $3);
                                                                          if($5 != NULL) addChild($$->node, $5->node);
                                                                          else addChild($$->node, newNode(Null, NULL, 0, 0));
                                                                          addChild($$->node, newNode(Null, NULL, 0, 0)); }
         | IF LPAR exprComma RPAR statement ELSE statement               { $$ = newNodeList(newNode(If, NULL, @1.first_line, @1.first_column));
                                                                          addChildren($$->node, $3);
                                                                          if($5 != NULL) addChild($$->node, $5->node);
                                                                          else addChild($$->node, newNode(Null, NULL, 0, 0));
                                                                          if($7 != NULL) addChild($$->node, $7->node);
                                                                          else addChild($$->node, newNode(Null, NULL, 0, 0)); }
         | WHILE LPAR exprComma RPAR statement                           { $$ = newNodeList(newNode(While, NULL, @1.first_line, @1.first_column));
                                                                          addChildren($$->node, $3);
                                                                          if($5 != NULL) addChild($$->node, $5->node);
                                                                          else addChild($$->node, newNode(Null, NULL, 0, 0)); }
         | RETURN exprComma SEMI                                         { $$ = newNodeList(newNode(Return, NULL, @1.first_line, @1.first_column));
                                                                          addChildren($$->node, $2); }
         | RETURN SEMI                                                  { $$ = newNodeList(newNode(Return, NULL, @1.first_line, @1.first_column));
                                                                          addChild($$->node, newNode(Null, NULL, 0, 0)); }
         | LBRACE error RBRACE                                          { $$ = newNodeList(newNode(Error, NULL, 0, 0)); }
         
         ;

expr: IDENTIFIER                        { $$ = newNodeList(newNode(Identifier, $1, @1.first_line, @1.first_column)); }
    | NATURAL                           { $$ = newNodeList(newNode(Natural, $1, @1.first_line, @1.first_column)); }
    | CHRLIT                            { $$ = newNodeList(newNode(ChrLit, $1, @1.first_line, @1.first_column)); }
    | DECIMAL                           { $$ = newNodeList(newNode(Decimal, $1, @1.first_line, @1.first_column)); }
    | expr ASSIGN expr                  { $$ = newNodeList(newNode(Store, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr PLUS expr                    { $$ = newNodeList(newNode(Add, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr MINUS expr                   { $$ = newNodeList(newNode(Sub, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr MUL expr                     { $$ = newNodeList(newNode(Mul, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr DIV expr                     { $$ = newNodeList(newNode(Div, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr MOD expr                     { $$ = newNodeList(newNode(Mod, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr EQ expr                      { $$ = newNodeList(newNode(Eq, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr NE expr                      { $$ = newNodeList(newNode(Ne, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr GT expr                      { $$ = newNodeList(newNode(Gt, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr GE expr                      { $$ = newNodeList(newNode(Ge, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr LT expr                      { $$ = newNodeList(newNode(Lt, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr LE expr                      { $$ = newNodeList(newNode(Le, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr AND expr                     { $$ = newNodeList(newNode(And, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr OR expr                      { $$ = newNodeList(newNode(Or, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr BITWISEAND expr              { $$ = newNodeList(newNode(BitWiseAnd, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr BITWISEOR expr               { $$ = newNodeList(newNode(BitWiseOr, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | expr BITWISEXOR expr              { $$ = newNodeList(newNode(BitWiseXor, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node);
                                          addChild($$->node, $3->node); }
    | PLUS expr   %prec HIGH            { $$ = newNodeList(newNode(Plus, NULL, @1.first_line, @1.first_column));
                                          addChild($$->node, $2->node); }
    | MINUS expr  %prec HIGH            { $$ = newNodeList(newNode(Minus, NULL, @1.first_line, @1.first_column));
                                          addChild($$->node, $2->node); }
    | NOT expr                          { $$ = newNodeList(newNode(Not, NULL, @1.first_line, @1.first_column));
                                          addChild($$->node, $2->node); }
    | LPAR exprComma RPAR                { $$ = $2; }
    | IDENTIFIER LPAR RPAR              { $$ = newNodeList(newNode(Call, NULL, @1.first_line, @1.first_column));
                                          addChild($$->node, newNode(Identifier, $1, @1.first_line, @1.first_column)); }
    | IDENTIFIER LPAR exprList RPAR     { $$ = newNodeList(newNode(Call, NULL, @1.first_line, @1.first_column));
                                          addChild($$->node, newNode(Identifier, $1, @1.first_line, @1.first_column));
                                          addChildren($$->node, $3); }
    | IDENTIFIER LPAR error RPAR        { $$ = newNodeList(newNode(Error, NULL, 0, 0)); }
    | LPAR error RPAR                   { $$ = newNodeList(newNode(Error, NULL, 0, 0)); }
    ;

exprList: expr                          { $$ = $1; }
        | exprList COMMA expr           { $$ = $1; concat($$, $3); }
        ;

exprComma: expr                         { $$ = $1; }
         | exprComma COMMA expr         { $$ = newNodeList(newNode(Comma, NULL, @2.first_line, @2.first_column));
                                          addChild($$->node, $1->node); 
                                          addChild($$->node, $3->node); }
         ;

%%
  