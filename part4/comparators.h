#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "ast.h"

enum type category_cast(enum type type1, enum type type2);
int undef_op(struct node *node1, struct node *node2);
int compare_types(enum type type1, enum type type2);
int compatible_types(enum type type2, enum type type1);

#endif