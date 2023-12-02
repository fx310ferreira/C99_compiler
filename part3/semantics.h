#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

enum symbol_type {var, func};

struct symbol_lists{
	char *function_name;
	struct symbol_list *symbol_table;
	struct symbol_lists *next;
};

struct symbol_list {
	char *identifier;
	enum type type;
	struct node *node;
	struct symbol_list *next;
	enum symbol_type symbol_type;
};

int check_program(struct node *program);
struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node, enum symbol_type symbol_type);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier, enum symbol_type symbol_type);
struct symbol_lists *insert_symbol_table(struct symbol_lists *table_list, char *function_name, struct symbol_list *symbol_table);
int check_parameters(struct node *parameters, struct symbol_list *scoped_table);
void check_function(struct node *function);
void check_function_declaration(struct node *function);
void check_body(struct node *expression, struct symbol_list *scoped_table);
void check_declaration(struct node *declaration, struct symbol_list *scoped_table);
void check_expression(struct node *expression, struct symbol_list *scoped_table);
void check_operation(struct node *expression, struct symbol_list *scoped_table);
void show_symbol_table();
void add_putchar(struct symbol_list *symbol_table);
void add_getchar(struct symbol_list *symbol_table);
int compare_params(struct node *params1, struct node *params2);

#endif
