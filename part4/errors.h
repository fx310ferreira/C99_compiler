#ifndef ERRORS_H
#define ERRORS_H

#include "ast.h"

void function_error(struct node *func1, struct node *func2);
void op_error(struct node *node1, struct node *node2, char *op, int line, int column);
void conflicting_types_error(struct node *node1, enum type type, int line, int column);
void already_declared_error(struct node *node);
void unknown_symbol_error(struct node *node);
void invalid_void_error(struct node *node);

#endif