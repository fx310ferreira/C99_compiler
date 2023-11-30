#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;

struct symbol_list *symbol_table;
struct symbol_lists *l_table_list;

void check_function(struct node *function) {
    struct node *type = getchild(function, 0);
    struct node *id = getchild(function, 1);
    struct symbol_list *local_symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    local_symbol_table->next = NULL;
    if(search_symbol(symbol_table, id->token) == NULL) {
        insert_symbol(symbol_table, id->token, category_type(type->category), function);
        insert_symbol_table(l_table_list, id->token, local_symbol_table);
    } else {
        printf("Identifier %s already declared %d, %d\n", id->token, id->line, id->column);
        semantic_errors++;
    }

    check_parameters(getchild(function, 2), local_symbol_table);
    check_expression(getchild(function, 3), local_symbol_table);
}

void check_expression(struct node *expression, struct symbol_list *scoped_table){
    struct node_list *child = expression->children;
    while((child = child->next) != NULL){
        switch (child->node->category)
        {
        case Declaration:
            check_declaration(child->node, scoped_table);
            break;
        
        default:
            break;
        }
    }
}

void check_parameters(struct node *parameters, struct symbol_list *scoped_table) {
    struct node_list *child = parameters->children;
    while((child = child->next) != NULL) {
        struct node *type = getchild(child->node, 0);
        struct node *id = getchild(child->node, 1);
        if(type->category == Void)
            continue;
        if(!id){
            continue;
        }
        if(search_symbol(symbol_table, id->token) == NULL || search_symbol(scoped_table, id->token) == NULL) {
            insert_symbol(scoped_table, id->token, category_type(type->category), child->node);
        } else {
            printf("Identifier %s already declared %d, %d\n", id->token, id->line, id->column);
            semantic_errors++;
        }
    }
}

void check_declaration(struct node *declaration, struct symbol_list *scoped_table) {
    struct node *type = getchild(declaration, 0);
    struct node *id = getchild(declaration, 1);
    if(!scoped_table){
        if(search_symbol(symbol_table, id->token) == NULL) {
            insert_symbol(symbol_table, id->token, category_type(type->category), declaration);
        } else {
            printf("Identifier %s already declared %d, %d\n", id->token, id->line, id->column);
            semantic_errors++;
        }
    }else{
        if(search_symbol(scoped_table, id->token) == NULL || search_symbol(symbol_table, id->token) == NULL) {
            insert_symbol(scoped_table, id->token, category_type(type->category), declaration);
        } else {
            printf("Identifier %s already declared %d, %d\n", id->token, id->line, id->column);
            semantic_errors++;
        }
    }
}

// semantic analysis begins here, with the AST root node
int check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;

    l_table_list = insert_symbol_table(l_table_list, NULL, symbol_table);

    struct node_list *child = program->children;
    add_putchar(symbol_table);
    add_getchar(symbol_table);
    while((child = child->next) != NULL){
        if(child->node->category == Declaration){
            check_declaration(child->node, NULL);
        }else{
            check_function(child->node);
        }
    }
    return semantic_errors;
}

struct symbol_lists *insert_symbol_table(struct symbol_lists *table_list, char *function_name, struct symbol_list *symbol_table){
    struct symbol_lists *new = (struct symbol_lists *) malloc(sizeof(struct symbol_lists));
    new->function_name = function_name;
    new->symbol_table = symbol_table;
    new->next = NULL;
    struct symbol_lists *table = table_list;
    while(table != NULL) {
        if(table->next == NULL) {
            table->next = new;    /* insert new symbol at the tail of the list */
            break;
        }
        table = table->next;
    }
    return new;
}

// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->next = NULL;
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}

void show_symbol_table() {
    struct symbol_lists *temp_table_list = l_table_list;
    while (temp_table_list != NULL)
    {
        struct symbol_list *symbol_table = temp_table_list->symbol_table;
        if(!temp_table_list->function_name)
            printf("===== Global Symbol Table =====\n");
        else
            printf("===== Function %s Symbol Table =====\n", temp_table_list->function_name);

        struct symbol_list *symbol;
        for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
            if(symbol->node->category == Declaration){
                printf("%s\t%s", symbol->identifier, type_name(symbol->type));
            }else if (symbol->node->category == ParamDeclaration){
                printf("%s\t%s\tparam", symbol->identifier, type_name(symbol->type));
            }else{
                printf("%s\t%s(", symbol->identifier, type_name(symbol->type));
                struct node_list *parameters = getchild(symbol->node, 2)->children;
                while((parameters = parameters->next) != NULL){
                    printf("%s", type_name(category_type(getchild(parameters->node, 0)->category)));
                    if(parameters->next != NULL){
                        printf(", ");
                    }
                }
                printf(")");
            }
            printf("\n");
        }
        printf("\n");
        temp_table_list = temp_table_list->next;
    }
}

void add_putchar(struct symbol_list *symbol_table){
    struct node *node = newNode(FunctionDefinition, NULL, 0, 0);
    struct node *type = newNode(Int, NULL, 0, 0);
    struct node *id = newNode(Identifier, "putchar", 0, 0);
    struct node *parameters = newNode(ParamList, NULL, 0, 0);
    struct node *parameter = newNode(ParamDeclaration, NULL, 0, 0);
    struct node *parameter_type = newNode(Int, NULL, 0, 0);
    addChild(node, type);
    addChild(node, id);
    addChild(node, parameters);
    addChild(parameters, parameter);
    addChild(parameter, parameter_type);
    insert_symbol(symbol_table, "putchar", integer_type, node);
}

void add_getchar(struct symbol_list *symbol_table){
    struct node *node = newNode(FunctionDefinition, NULL, 0, 0);
    struct node *type = newNode(Int, NULL, 0, 0);
    struct node *id = newNode(Identifier, "getchar", 0, 0);
    struct node *parameters = newNode(ParamList, NULL, 0, 0);
    struct node *parameter = newNode(ParamDeclaration, NULL, 0, 0);
    struct node *parameter_type = newNode(Void, NULL, 0, 0);
    addChild(node, type);
    addChild(node, id);
    addChild(node, parameters);
    addChild(parameters, parameter);
    addChild(parameter, parameter_type);
    insert_symbol(symbol_table, "getchar", integer_type, node);
}