#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "ast.h"



void generator(struct node *program);
int codegen_expression(struct node *expression);

#endif