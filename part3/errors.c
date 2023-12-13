#include <stdio.h>
#include "ast.h"
#include "semantics.h"

extern int semantic_errors;


void function_error(struct node *func1, struct node *func2){
    printf("Line %d, column %d: Conflicting types (got %s", getchild(func1, 1)->line, getchild(func1, 1)->column, type_name(category_type(getchild(func1, 0)->category)));
    struct node_list *params = getchild(func1, 2)->children;
    printf("(");
    while((params = params->next)){
        printf("%s", type_name(category_type(getchild(params->node, 0)->category)));
        if(params->next != NULL) printf(",");
    }
    printf(")");
    printf(", expected %s", type_name(category_type(getchild(func2, 0)->category)));
    params = getchild(func2, 2)->children;
    printf("(");
    while((params = params->next)){
        printf("%s", type_name(category_type(getchild(params->node, 0)->category)));
        if(params->next != NULL) printf(",");
    }
    printf("))\n");
    semantic_errors++;
}

void conflicting_types_error(struct node *node1, enum type type, int line, int column){
    printf("Line %d, column %d: Conflicting types (got " , line, column);
    if(node1->function){ 
        printf("%s", type_name(category_type(getchild(node1->function, 0)->category)));
        struct node_list *params = getchild(node1->function, 2)->children;
        printf("(");
        while((params = params->next)){
            printf("%s", type_name(category_type(getchild(params->node, 0)->category)));
            if(params->next != NULL) printf(",");
        }
        printf(")");
    }else{
        printf("%s", type_name(node1->type));
    }
    printf(", expected %s)\n", type_name(type));
}

void op_error(struct node *node1, struct node *node2, char *op, int line, int column){
    printf("Line %d, column %d: Operator %s cannot be applied to ", line, column, op);
    if(!node2)
        printf("type ");
    else
        printf("types ");
    if(node1->function){ 
        printf("%s", type_name(category_type(getchild(node1->function, 0)->category)));
        struct node_list *params = getchild(node1->function, 2)->children;
        printf("(");
        while((params = params->next)){
            printf("%s", type_name(category_type(getchild(params->node, 0)->category)));
            if(params->next != NULL) printf(",");
        }
        printf(")");
    }else{
        printf("%s", type_name(node1->type));
    }
    if(!node2){
        printf("\n");
        return;
    }else if(node2->function){
        printf(", %s", type_name(category_type(getchild(node2->function, 0)->category)));
        struct node_list *params = getchild(node2->function, 2)->children;
        printf("(");
        while((params = params->next)){
            printf("%s", type_name(category_type(getchild(params->node, 0)->category)));
            if(params->next != NULL) printf(",");
        }
        printf(")");
    }else{
        printf(", %s", type_name(node2->type));
    }
    printf("\n");
    semantic_errors++;
}

void already_declared_error(struct node *node){
    printf("Line %d, column %d: Symbol %s already defined\n", node->line, node->column, node->token);
    semantic_errors++;
}

void unknown_symbol_error(struct node *node){
    printf("Line %d, column %d: Unknown symbol %s\n", node->line, node->column, node->token);
    semantic_errors++;
}

void invalid_void_error(struct node *node){
    printf("Line %d, column %d: Invalid use of void type in declaration\n", node->line, node->column);
    semantic_errors++;
}
