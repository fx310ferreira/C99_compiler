
# COMP Project 2023/2024
###### André Louro nº2021232388 || Frederico Ferreira nº2021217116
## Rewritten Grammar 
#### Added Priorities: 
```yacc
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
```

The ``%right`` is used for right associative operators, the ``%left`` is used for left associative operators. The ``%noasssoc`` was used to remove the ambiguity from the ``if`` ``else`` production. The ``HIGH`` priority was used for the ``Plus`` and ``Minus``.
#### Altered BNF Grammar:
```Grammar
<functionsAndDeclarations> → <declaration>
                           | <functionsAndDeclarations> <declaration>
                           | <functionDefinition>                   
                           | <functionsAndDeclarations> <functionDefinition>
                           | <functionDeclaration>
                           | <functionsAndDeclarations> <functionDeclaration>
  
<functionDefinition> → <typeSpec> <functionDeclarator> <functionBody>
   
<functionBody> → LBRACE <declarationsAndStatements> RBRACE
               | LBRACE RBRACE
     
<declarationsAndStatements> → <declarationsAndStatements> <declaration>    
                            | <declaration>
                            | <declarationsAndStatements> <statement> 
                            | <statement>     

<functionDeclaration> → <typeSpec> <functionDeclarator> SEMI

<functionDeclarator> → IDENTIFIER LPAR <parameterList> RPAR

<parameterList> → <parameterDeclaration>
                |  <parameterList> COMMA <parameterDeclaration>

<parameterDeclaration> → <typeSpec> IDENTIFIER
                       | <typeSpec>
     
<declaration> → <typeSpec> <declaratorList> SEMI
              | <error> SEMI

<declaratorList> → <declarator>
                 | <declaratorList> COMMA <declarator>

<declarator> → IDENTIFIER
             | IDENTIFIER ASSIGN <exprList>

<typeSpec> → INT
           | DOUBLE
           | CHAR
           | VOID
           | SHORT

<statementError> → <statement> 
                 | <error> SEMI

<statementList> → <statementError>
                | <statementList> <statementError>

<statement> → <SEMI>
            | <exprComma> SEMI
            | LBRACE <statementList> RBRACE
            | LBRACE RBRACE
            | IF LPAR <exprComma> RPAR <statement>  (Note: %prec LOW)
            | IF LPAR <exprComma> RPAR <statement> ELSE <statement>
            | WHILE LPAR <exprComma> RPAR <statement>
            | RETURN <exprComma> SEMI
            | RETURN SEMI
            | LBRACE <error> RBRACE      

<expr> → IDENTIFIER
       | NATURAL                          
       | CHRLIT                           
       | DECIMAL                         
       | <expr> ASSIGN <expr>               
       | <expr> PLUS <expr>                   
       | <expr> MINUS <expr>                   
       | <expr> MUL <expr>                    
       | <expr> DIV <expr>                     
       | <expr> MOD <expr>                   
       | <expr> EQ <expr>                    
       | <expr> NE <expr>                      
       | <expr> GT <expr>                    
       | <expr> GE <expr>                    
       | <expr> LT <expr>                     
       | <expr> LE <expr>                     
       | <expr> AND <expr>                   
       | <expr> OR <expr>                    
       | <expr> BITWISEAND <expr>           
       | <expr> BITWISEOR <expr>            
       | <expr> BITWISEXOR <expr>            
       | PLUS <expr> (Note: %prec HIGH)           
       | MINUS <expr>  (Note: %prec HIGH)            
       | NOT <expr>                         
       | LPAR <exprComma> RPAR               
       | IDENTIFIER LPAR RPAR             
       | IDENTIFIER LPAR <exprList> RPAR     
       | IDENTIFIER LPAR <error> RPAR        
       | LPAR <error> RPAR                 

<exprList> → <expr>                        
           | <exprList> COMMA <expr>          

<exprComma> → <expr>                         
            | <exprComma> COMMA <expr>        
```

The **EBNF Grammar** was transformed in a **BNF Grammar** by creating an extra production in case of optional parameters and by creating a new node in case of having undefined number of parameters. Because of the error productions a new node (``statementError``) was added in order to remove the ambiguity that the error productions create.
Another honorable mention is the creation of the separated nodes for ``exprList`` && ``exprComma`` this was done because of the necessity to have a ``COMMA`` node on the AST only when the ``exprList``  is not inside a function call.

## Algorithms, AST Data Structures and Symbol Table
When creating the **AST Tree** we came across with two main problems. 
The first one being the multiple appearances on a ``Declaration node`` when only one "Declaration" with multiple variables appeared. To solve this problem we opted for creating the Declaration Node inside the ``Declarator branch`` and storing the type of the declaration on a global variable to be used inside the ``Declarator``. 
Another problem, was the recurrent necessity of adding brothers, different methods where experimented and many of them proved to be effective. The one that was implemented was the one that lead to *less memory allocation*. 

Our *strategy* consistes on using the following *data structures*: 
```C
struct node {
    enum category category;
    char *token;
    enum type type;
    int line;
    int column;
    struct node *function;
    struct node_list *children;

};

struct node_list {
    struct node *node;
    struct node_list *next;
};
```

Besides using functions like ``addChild`` , to create a new node we created three more functions, one called ``newNodeList`` which was destined to create a new node list with a certain node, a function called ``concat`` which concatenated two ``nodeList`` (*effectively creating brothers this way*) and one called ``addChildren`` with the purpose of adding a node list to a certain node. Using this three functions together we created a strategy to add brothers.

To implement the *Symbol Tables* we have a list that is a *linked list of symbol tables*, the first table on that list is the global table and all the scoped tables are following it. We pass the scoped tables to all the functions as parameters

## Code Generation 
*Due to time constraints this part of the project was not completed therefor no technical justification is necessary*

