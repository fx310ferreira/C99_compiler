// Frederico Ferreira 2021217116
// Andre Louro 2021232388
#ifndef _AST_H
#define _AST_H

enum category { Program, FunctionDeclaration, FunctionDefinition, ParamList, FuncBody,
                ParamDeclaration, Declaration, StatList, If, While, Return, Or,
                And, Eq, Ne, Lt, Gt, Le, Ge, Add, Sub, Mul, Div, Mod, Not, Minus,
                Plus, Store, Comma, Call, BitWiseAnd, BitWiseOr, BitWiseXor,
                Char, ChrLit, Identifier, Int, Short, Natural, Double, Decimal,
                Void, Null, Aux, Error
                };

struct node {
    enum category category;
    char *token;
    struct node_list *children;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct flagsT {
    int l;
    int t;
};

struct node *newNode(enum category category, char *token);
void addChild(struct node *parent, struct node *child);
void addChildren(struct node *parent, struct node_list *children);
struct node_list *newNodeList(struct node *newNode);
struct node_list concat(struct node_list *o_brother, struct node_list *n_brother);
void show(struct node *n, int level);
#endif
