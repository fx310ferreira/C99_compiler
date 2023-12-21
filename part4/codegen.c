#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "codegen.h"

int temporary;   // sequence of temporary registers in a function

extern struct symbol_lists *l_table_list;
extern struct symbol_list *symbol_table;

char *get_type(enum type type) {
    switch(type) {
        case integer_type:
            return "i32";
        case char_type:
            return "i32";
        case short_type:
            return "i32";
        case void_type:
            return "void";
        case double_type:
            return "double";
        default:
            return "UNDEF";
    }
}

int codegen_natural(struct node *natural) {
    printf("  %%%d = add i32 %s, 0\n", temporary, natural->token);
    return temporary++;
}

int codegen_char(struct node *character) {
    printf("  %%%d = add i32 %s, 0\n", temporary, character->token);
    return temporary++;
}

int codegen_expression(struct node *expression) {
    int tmp = -1;
    switch(expression->category) {
        case Natural:
            tmp = codegen_natural(expression);
            break;
        case ChrLit:
            tmp = codegen_char(expression);
            break;
        case Identifier:
            // tmp = temporary++;
            // printf("  %%%d = add i32 0, %%%s\n", tmp, expression->token);
            break;
        default:
            break;
    }
    return tmp;
}

void codegen_declaration(struct node *declaration) {
    struct node *type = getchild(declaration, 0);
    struct node *identifier = getchild(declaration, 1);
    struct node *expression = getchild(declaration, 2);
    printf("@%s = global %s 0\n", identifier->token, get_type(category_type(type->category)));
    // if(expression != NULL) {
    //     int tmp = codegen_expression(expression);
    //     printf("store %s %%%d, %s* %%%s\n", get_type(category_type(type->category)), tmp, get_type(category_type(type->category)), identifier->token);
    // }
}

void codegen_parameters(struct node *parameters) {
    struct node *parameter;
    int curr = 0;
    while((parameter = getchild(parameters, curr++)) != NULL) {
        if(getchild(parameter, 0)->category == Void)
            return;
        if(curr > 1)
            printf(", ");
        if(getchild(parameter, 1))
            printf("%s %%%s", get_type(category_type(getchild(parameter, 0)->category)), getchild(parameter, 1)->token);
        else
            printf("%s %%%d", get_type(category_type(getchild(parameter, 0)->category)), temporary++);
        
    }
}

void codegen_function(struct node *function) {
    temporary = 1;
    struct symbol_lists *entry = search_symbol_table(l_table_list, getchild(function, 1)->token);
    struct symbol_list *aux = entry->symbol_table->next;
    printf("define %s @_%s(", get_type(aux->type), entry->function_name);
    codegen_parameters(getchild(function, 2));
    printf(") {\n");
    // codegen_expression(getchild(function, 3));
    printf("  ret i32 %%%d\n", temporary-1);
    printf("}\n\n");
}

// code generation begins here, with the AST root node
void generator(struct node *program) {
    // getchar and putchar
    printf("declare i32 @getchar()\n");
    printf("declare i32 @putchar(i32)\n\n");

    // generate the functions and declarations
    struct node_list *function = program->children;
    while((function = function->next) != NULL)
        if(function->node->category == FunctionDefinition)
            codegen_function(function->node);
        else if(function->node->category == Declaration)
            codegen_declaration(function->node);

    // generate the entry point which calls main(integer) if it exists
    struct symbol_list *entry = search_symbol(symbol_table, "main", func);
    if(entry != NULL && entry->node->category == FunctionDefinition)
        printf("define i32 @main() {\n"
               "  %%1 = call i32 @_main()\n"
               "  ret i32 %%1\n"
               "}\n");
}
