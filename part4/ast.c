// Frederico Ferreira 2021217116
// Andre Louro 2021232388
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
 char* categories[] = { "Program", "FuncDeclaration", "FuncDefinition", "ParamList", "FuncBody",
                "ParamDeclaration", "Declaration", "StatList", "If", "While", "Return", "Or",
                "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus",
                "Plus", "Store", "Comma", "Call", "BitWiseAnd", "BitWiseOr", "BitWiseXor",
                "Char", "ChrLit", "Identifier", "Int", "Short", "Natural", "Double", "Decimal",
                "Void", "Null", "Aux", "Error"
                };

// create a node of a given category with a given lexical symbol
struct node *newNode(enum category category, char *token, int line, int column) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->type = no_type;
    new->line = line;
    new->column = column;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    return new;
}

struct node_list *newNodeList(struct node *newNode) {
    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = newNode;
    new->next = NULL;
    // printf("NEW NODE LIST %s %s\n", categories[new->node->category], new->node->token);
    return new;
}

struct node_list concat(struct node_list *o_brother, struct node_list *n_brother){
    struct node_list *aux = o_brother;
    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = n_brother;

    return *o_brother;
}

// append a node to the list of children of the parent node
void addChild(struct node *parent, struct node *child) {
    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = child;
    new->next = NULL;
    struct node_list *children = parent->children;
    while(children->next != NULL){
        children = children->next;
    }
    children->next = new;
}

void addChildren(struct node *parent, struct node_list *children){
    struct node_list *aux = parent->children;
    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = children;
    
}

// get the n-th child of a node
struct node *getchild(struct node *parent, int position) {
    struct node_list *children = parent->children;
    while((children = children->next) != NULL)
        if(position-- == 0)
            return children->node;
    return NULL;
}

// link a function to a function declaration or definition
void link_function(struct node *parent, struct node *child) {
    parent->function = child;
}

// print the tree in a depth-first manner
void show(struct node *n, int level){
    // if(n->category == Error) return; //ignore error nodes
    for(int i = 0; i < level; i++) printf("..");
    if(n -> token)
	    printf("%s(%s)", categories[n->category], n->token);
    else
	    printf("%s", categories[n->category]);
    if (n->function){
        printf(" - %s", type_name(category_type(getchild(n->function, 0)->category)));
        if(getchild(n->function, 2)){
            printf("(");
            struct node_list *c = getchild(n->function, 2)->children;
            while ((c = c->next) != NULL){
                printf("%s", type_name(category_type(getchild(c->node, 0)->category)));
                if(c->next != NULL) printf(",");
            }
            printf(")");
        }
     }else if (n->type != no_type && n->category != Null){
        printf(" - %s", type_name(n->type));
    }
    printf("\n");
    struct node_list *c = n->children;
    while ((c = c->next) != NULL)
        show(c->node, level + 1);
}
