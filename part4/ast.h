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

enum type {integer_type,char_type, short_type, double_type,  void_type, undef_type, no_type};
#define type_name(type) (type == integer_type ? "int" : (type == double_type ? "double" : (type == char_type ? "char" : (type == short_type ? "short" : (type == void_type ? "void" : (type == undef_type ? "undef" : "no_type"))))))
#define category_type(category) (category == Int ? integer_type : (category == Double ? double_type : (category == Char ? char_type : (category == Short ? short_type : (category == Void ? void_type : no_type)))))
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

struct flagsT {
    int l;
    int t;
    int s;
};

struct node *newNode(enum category category, char *token, int line, int column);
void addChild(struct node *parent, struct node *child);
void addChildren(struct node *parent, struct node_list *children);
struct node_list *newNodeList(struct node *newNode);
struct node_list concat(struct node_list *o_brother, struct node_list *n_brother);
void show(struct node *n, int level);
struct node *getchild(struct node *parent, int position);
void link_function(struct node *parent, struct node *child);
#endif
