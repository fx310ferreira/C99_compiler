#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "errors.h"
#include "comparators.h"

int semantic_errors = 0;

struct symbol_list *symbol_table;
struct symbol_lists *l_table_list;

void check_function(struct node *function) {
    struct node *type = getchild(function, 0);
    struct node *id = getchild(function, 1);
    struct symbol_lists *temp;
    struct symbol_list *local_symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    local_symbol_table->next = NULL;
    struct symbol_list *symbol = search_symbol(symbol_table, id->token, func);
    if(symbol == NULL) {
        insert_symbol(symbol_table, id->token, category_type(type->category), function, func);
        insert_symbol_table(l_table_list, id->token, function, local_symbol_table);
        insert_symbol(local_symbol_table, "return", category_type(type->category), newNode(Declaration, NULL, 0, 0), var);
    } else if(symbol->node->category == FunctionDeclaration) {
        if(compare_params(getchild(symbol->node, 2), getchild(function, 2)) && type->category == getchild(symbol->node, 0)->category){
            symbol->node = function;
            free(local_symbol_table);
            temp = search_symbol_table(l_table_list, id->token);
            if(!temp)
                printf("THE CODE SHOULD NEVER GET HERE\nIF YOU GOT HERE CONGRATS\n");
            local_symbol_table = temp->symbol_table;
            temp->function = function;
            insert_symbol(local_symbol_table, "return", category_type(type->category), newNode(Declaration, NULL, 0, 0), var);
        }
        else {
            function_error(function, symbol->node);
            semantic_errors++;
            return;
        }
    } else {
        printf("Line %d, column %d: Symbol %s already defined\n", id->line, id->column , id->token);
        semantic_errors++;
        return; //? is this supposed to be here? remove and look at the first test
    }

    check_parameters(getchild(function, 2), local_symbol_table);
    struct node_list *body = getchild(function, 3)->children;
    while((body = body->next))
        check_body(body->node, local_symbol_table); //this can be optimized
}

void check_function_declaration(struct node *function) {
    struct node *type = getchild(function, 0);
    struct node *id = getchild(function, 1);
    struct node *parameters = getchild(function, 2);
    struct symbol_list *symbol = search_symbol(symbol_table, id->token, func);
    struct symbol_list *scoped_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    scoped_table->next = NULL;
    if(!symbol) {
        if(check_parameters(parameters, NULL)){
            insert_symbol(symbol_table, id->token, category_type(type->category), function, func);
            insert_symbol_table(l_table_list, id->token, function, scoped_table);
        }
    } else {
        if(!compare_params(parameters, getchild(search_symbol(symbol_table, id->token, func)->node, 2)) || type ->category != getchild(search_symbol(symbol_table, id->token, func)->node, 0)->category){
            function_error(function, symbol->node);
        }
            
    }
}

void check_body(struct node *expression, struct symbol_list *scoped_table){
    struct node_list *child = expression->children;
    struct node *aux;
    switch (expression->category)
    {
    case Declaration:
        check_declaration(expression, scoped_table);
        break;
    case While:
        aux = getchild(expression, 0);
        check_expression(aux, scoped_table);
        if(compare_types(aux->type, integer_type) == 0){
            printf("Line %d, column %d: Conflicting types (got %s, expected int)\n", aux->line, aux->column, type_name(aux->type));
            semantic_errors++;
        }
        child = child->next;
        while((child = child->next))
            check_body(child->node, scoped_table);
        break;
    case If:
        check_expression((aux = getchild(expression, 0)), scoped_table);
        if(compare_types(aux->type, integer_type) == 0){
            printf("Line %d, column %d: Conflicting types (got %s, expected int)\n", aux->line, aux->column, type_name(aux->type));
            semantic_errors++;
        }
        child = child->next;
        while((child = child->next))
            check_body(child->node, scoped_table);
        break;
    case Return:
        aux = getchild(expression, 0);
        check_expression(aux, scoped_table);
        if(compatible_types(aux->type, search_symbol(scoped_table, "return", var)->type) == 0){
            printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", aux->line, aux->column, type_name(aux->type), type_name(search_symbol(scoped_table, "return", var)->type));
            semantic_errors++;
        }
        break;
    case StatList:
        while((child = child->next))
            check_body(child->node, scoped_table);
        break;
    default:
        check_expression(expression, scoped_table);
        break;
    }
}

void check_expression(struct node *expression, struct symbol_list *scoped_table){
    struct symbol_list *symbol;
    int expected_params = 0, received_params = 0;
    switch (expression->category)
    {
    case Null:
        expression->type = void_type;
        break;
    case Natural:
        expression->type = integer_type;
        break;
    case ChrLit:
        expression->type = integer_type;
        break;
    case Decimal:
        expression->type = double_type;
        break;
    case Add:
        check_operation(expression, scoped_table);
        expression->type = category_cast(getchild(expression, 0)->type, getchild(expression, 1)->type);
        if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "+", expression->line, expression->column);
            expression->type = undef_type;
        }

        break;
    case Sub:
        check_operation(expression, scoped_table);
        expression->type = category_cast(getchild(expression, 0)->type, getchild(expression, 1)->type);
        if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "-", expression->line, expression->column);
            expression->type = undef_type;
        }
        break;
    case Mul:
        check_operation(expression, scoped_table);
        expression->type = category_cast(getchild(expression, 0)->type, getchild(expression, 1)->type);
        if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "*", expression->line, expression->column);
            expression->type = undef_type;
        }
        break;
    case Div:
        check_operation(expression, scoped_table);
        expression->type = category_cast(getchild(expression, 0)->type, getchild(expression, 1)->type);
        if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "/", expression->line, expression->column);
            expression->type = undef_type;
        }
        break;
    case Mod:   
        check_operation(expression, scoped_table);
        if(getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "%", expression->line, expression->column);
        else if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "%", expression->line, expression->column);
        }
        expression->type = integer_type;
        break;
    case Store:
        check_operation(expression, scoped_table);
        expression->type = getchild(expression, 0)->type;
        if(getchild(expression, 1)->type == double_type && getchild(expression, 0)->type != double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "=", expression->line, expression->column);
        else if(undef_op(getchild(expression, 0), getchild(expression, 1))){
            op_error(getchild(expression, 0), getchild(expression, 1), "=", expression->line, expression->column);
            // expression->type = undef_type;
        }else if(getchild(expression, 0)->category != Identifier){
            printf("Line %d, column %d: Lvalue required\n", getchild(expression, 0)->line, getchild(expression, 0)->column);
            semantic_errors++;
        }
        expression->type = getchild(expression, 0)->type;
        break;
    case Eq:
        check_operation(expression, scoped_table);
        if(getchild(expression, 0)->type == getchild(expression, 1)->type);
        else if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "==", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Ne:
        check_operation(expression, scoped_table);
        if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "=!", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Lt:
        check_operation(expression, scoped_table);
        if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "<", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Gt:
        check_operation(expression, scoped_table);
        if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), ">", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Le:
        check_operation(expression, scoped_table);
        if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "<=", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Ge:
        check_operation(expression, scoped_table);
        if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), ">=", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case And:
        check_operation(expression, scoped_table);
        expression->type = integer_type;
        if(getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "&&", expression->line, expression->column);
        else if(undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "&&", expression->line, expression->column);
        break;
    case Or:
        check_operation(expression, scoped_table);
        expression->type = integer_type;
        if(getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "||", expression->line, expression->column);
        else if (undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "||", expression->line, expression->column);
        break;
    case BitWiseAnd:
        check_operation(expression, scoped_table);
        expression->type = integer_type;
        if(getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "&", expression->line, expression->column);
        else if (undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "&", expression->line, expression->column);
        break;
    case BitWiseOr:
        check_operation(expression, scoped_table);
        expression->type = integer_type;
        if(getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "|", expression->line, expression->column);
        else if (undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "|", expression->line, expression->column);
        break;
    case BitWiseXor:
        check_operation(expression, scoped_table);
        expression->type = integer_type;
        if(getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type)
            op_error(getchild(expression, 0), getchild(expression, 1), "^", expression->line, expression->column);
        else if (undef_op(getchild(expression, 0), getchild(expression, 1)))
            op_error(getchild(expression, 0), getchild(expression, 1), "^", expression->line, expression->column);
        break;
    case Comma:
        check_operation(expression, scoped_table);
        if(getchild(expression, 0)->type == undef_type || getchild(expression, 0)->type == no_type ){
            op_error(getchild(expression, 0), getchild(expression, 1), ",", expression->line, expression->column);
        }
        if(getchild(expression, 1)->type == undef_type || getchild(expression, 1)->type == no_type ){
            op_error(getchild(expression, 0), getchild(expression, 1), ",", expression->line, expression->column);
            expression->type = undef_type;
        }
        break;
    case Not:
        check_expression(getchild(expression, 0), scoped_table);
        if (getchild(expression, 0)->type == undef_type || getchild(expression, 0)->type == void_type || getchild(expression, 0)->type == double_type || getchild(expression, 0)->type == no_type)
            op_error(getchild(expression, 0), NULL, "!", expression->line, expression->column);
        expression->type = integer_type;
        break;
    case Minus:
        check_expression(getchild(expression, 0), scoped_table);
        if (getchild(expression, 0)->type == undef_type || getchild(expression, 0)->type == void_type || getchild(expression, 0)->type == no_type)
            op_error(getchild(expression, 0), NULL, "-", expression->line, expression->column);
        expression->type = getchild(expression, 0)->type;
        break;
    case Plus:
        check_expression(getchild(expression, 0), scoped_table);
        if (getchild(expression, 0)->type == undef_type || getchild(expression, 0)->type == void_type || getchild(expression, 0)->type == no_type)
            op_error(getchild(expression, 0), NULL, "+", expression->line, expression->column);
        expression->type = getchild(expression, 0)->type;
        break;
    case Identifier:
        if((symbol = search_symbol(scoped_table, expression->token, var))){
            expression->type = symbol->type;
        }
        else if ((symbol = search_symbol(symbol_table, expression->token, var))){
            expression->type = symbol->type;
        }else if ((symbol = search_symbol(symbol_table, expression->token, func))){
            link_function(expression, symbol->node);
        }else{
            expression->type = undef_type;
            unknown_symbol_error(expression);
        }
    break;
    case Call:
        symbol = search_symbol(symbol_table, getchild(expression, 0)->token, func);
        if(!symbol)
            symbol = search_symbol(scoped_table, getchild(expression, 0)->token, func);
        if(!symbol)
            symbol = search_symbol(symbol_table, getchild(expression, 0)->token, var);
        if(!symbol)
            symbol = search_symbol(scoped_table, getchild(expression, 0)->token, var);
        struct node_list *parameters = expression->children->next;
        while ((parameters = parameters->next)){
            received_params++;
            check_expression(parameters->node, scoped_table); //? check with the function
        }
        if(symbol){
            expression->type = symbol->type;
            link_function(getchild(expression, 0), symbol->node);
            if(getchild(symbol->node, 2)){
                struct node_list *params = getchild(symbol->node, 2)->children;
                while ((params = params->next)){
                    if(params->node->children->next->node->category != Void)
                        expected_params++;
                }
            }
            if(expected_params != received_params){
                printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required %d)\n", expression->line, expression->column, getchild(expression, 0)->token, received_params, expected_params);
                semantic_errors++;
            }else if(received_params != 0) { //? this can be improved remove the != 0  and enjoy the segfault
                struct node_list *params = getchild(symbol->node, 2)->children;
                struct node_list *parameters = expression->children->next;
                while ((params = params->next)){
                    //? error?
                    parameters = parameters->next;
                    if(!compatible_types(parameters->node->type, category_type(getchild(params->node, 0)->category))){
                        printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", parameters->node->line, parameters->node->column, 
                            type_name(parameters->node->type), type_name(category_type(getchild(params->node, 0)->category)));
                        semantic_errors++;
                    }
                }
            }
        }else{
            unknown_symbol_error(getchild(expression, 0));
            semantic_errors++;
            expression->type = undef_type; //? missing anotation for func call
            getchild(expression, 0)->type = undef_type;
            if(expected_params != received_params){
                printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required %d)\n", expression->line, expression->column, getchild(expression, 0)->token, received_params, expected_params);
                semantic_errors++;
            }
        }
        break;
    default:
        break;
    }
}

void check_operation(struct node *expression, struct symbol_list *scoped_table){
    struct node *left = getchild(expression, 0);
    struct node *right = getchild(expression, 1);
    check_expression(left, scoped_table);
    check_expression(right, scoped_table);
    expression->type = right->type;
}

int check_parameters(struct node *parameters, struct symbol_list *scoped_table) {
    struct node_list *child = parameters->children;
    int dirty = 0;
    while((child = child->next) != NULL) {
        struct node *type = getchild(child->node, 0);
        struct node *id = getchild(child->node, 1);
        if(type->category == Void && id){
            invalid_void_error(type);
            return 0;
        }else if (type->category == Void) {
            if(dirty == 1){
                invalid_void_error(type);
                return 0;
            }else{
                dirty = 1;
            }
        }else if(!id){
            dirty = 1;
        } else if(search_symbol(scoped_table, id->token, var) == NULL) { //? the first part is probably gonna be commented
            insert_symbol(scoped_table, id->token, category_type(type->category), child->node, var);
            dirty = 1;
        } else {
            already_declared_error(id);
            return 0;
        }
    }
    return 1;
}

// int check_declaration_parameters(struct node *parameters){
//     printf("herre\n");
//     return 1;
// }/

void check_declaration(struct node *declaration, struct symbol_list *scoped_table) {
    struct node *type = getchild(declaration, 0);
    struct node *id = getchild(declaration, 1);
    struct node *expression = getchild(declaration, 2);
    struct symbol_list *aux;
    if(expression)
        check_expression(expression, scoped_table);
    if (type->category == Void){
        invalid_void_error(id);
        return;
    }
    if(!scoped_table){
        aux = search_symbol(symbol_table, id->token, var);
        if(!aux) {
            insert_symbol(symbol_table, id->token, category_type(type->category), declaration, var);
        } else {
            // already_declared_error(id);//? WHYYYYYYYYYYYYYYYYYYYYYYYYYY
            if(!compare_types(category_type(aux->node->children->next->node->category), category_type(type->category)))
                printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->line, id->column, type_name(category_type(type->category)), type_name(category_type(aux->node->children->next->node->category)));
            else{
                ;//? change?
            }
        }
    }else{
        if(search_symbol(scoped_table, id->token, var) == NULL || search_symbol(symbol_table, id->token, var) == NULL) {
            insert_symbol(scoped_table, id->token, category_type(type->category), declaration, var);
        } else {
            already_declared_error(id);
        }
    }
    if(!expression)
        return;
    if(expression->type == undef_type)
        return;
    if(!compatible_types(expression->type, category_type(type->category))){
        printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->line, id->column, type_name(expression->type), type_name(category_type(type->category)));
        semantic_errors++;
    }
}

// semantic analysis begins here, with the AST root node
int check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;

    l_table_list = insert_symbol_table(l_table_list, NULL, NULL, symbol_table);

    struct node_list *child = program->children;
    add_putchar(symbol_table);
    add_getchar(symbol_table);
    while((child = child->next) != NULL){
        if(child->node->category == Declaration){
            check_declaration(child->node, NULL);
        }else if(child->node->category == FunctionDefinition){
            check_function(child->node);
        }else if(child->node->category == FunctionDeclaration){
            check_function_declaration(child->node);
        }
    }
    return semantic_errors;
}

struct symbol_lists *insert_symbol_table(struct symbol_lists *table_list, char *function_name, struct node *function, struct symbol_list *symbol_table){
    struct symbol_lists *new = (struct symbol_lists *) malloc(sizeof(struct symbol_lists));
    new->function_name = function_name;
    new->function = function;
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

struct symbol_lists *search_symbol_table(struct symbol_lists *table_list, char *function_name){
    if (!table_list)
        return NULL;
    struct symbol_lists *table;
    for(table = table_list->next; table != NULL; table = table->next)
        if(strcmp(table->function_name, function_name) == 0)
            return table;
    return NULL;
}

// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node, enum symbol_type symbol_type) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->symbol_type = symbol_type;
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
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier, enum symbol_type symbol_type) {
    if (!table)
        return NULL;
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0 && symbol->symbol_type == symbol_type)
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
        else if(temp_table_list->function->category == FunctionDefinition)
            printf("===== Function %s Symbol Table =====\n", temp_table_list->function_name);
        else{
            temp_table_list = temp_table_list->next;
            continue;
        }

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
                        printf(",");
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
    insert_symbol(symbol_table, "putchar", integer_type, node, func);
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
    insert_symbol(symbol_table, "getchar", integer_type, node, func);
}

int compare_params(struct node *params1, struct node *params2){

    struct node_list *params_list1 = params1->children;
    struct node_list *params_list2 = params2->children;

    while((params_list1 = params_list1->next)){
        if(!params_list1)
            return 0;
        params_list2 = params_list2->next;
        if(!params_list2)
            return 0;
        if(getchild(params_list1->node, 0)->category != getchild(params_list2->node, 0)->category){
            return 0;
        }
    }
    return 1;
}