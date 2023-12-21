#include "ast.h"
#include "semantics.h"

enum type category_cast(enum type type1, enum type type2){
    if(type1 != type2){
        if((type1 == integer_type && type2 == short_type) || (type1 == short_type && type2 == integer_type))
            return integer_type;
        else if((type1 == char_type && type2 == integer_type) || (type1 == integer_type && type2 == char_type))
            return integer_type;
        else if((type1 == double_type && type2 == integer_type) || (type1 == integer_type && type2 == double_type))
            return double_type;
        else if((type1 == double_type && type2 == short_type) || (type1 == short_type && type2 == double_type))
            return double_type;
        else if((type1 == double_type && type2 == char_type) || (type1 == char_type && type2 == double_type))
            return double_type;
        else if((type1 == short_type && type2 == char_type) || (type1 == char_type && type2 == short_type))
            return short_type;
    }
    return type1;
}

int undef_op(struct node *node1, struct node *node2){
    if (node1->type == undef_type || node2->type == undef_type)
        return 1;
    if (node1->type == void_type || node2->type == void_type)
        return 1;
    if (node1->type == no_type || node2->type == no_type)
        return 1;
    return 0;
}

int compare_types(enum type type1, enum type type2){
    if(type1 == type2)
        return 1;
    else if(type1 == integer_type && type2 == short_type)
        return 1;
    else if(type1 == short_type && type2 == integer_type)
        return 1;
    else if(type1 == char_type && type2 == integer_type)
        return 1;
    else if(type1 == integer_type && type2 == char_type)
        return 1;
    return 0;
}

int compatible_types(enum type type2, enum type type1){
    if(type1 == type2)
        return 1;
    else if(type1 == integer_type && type2 == short_type)
        return 1;
    else if(type1 == short_type && type2 == integer_type)
        return 1;
    else if(type1 == char_type && type2 == integer_type)
        return 1;
    else if(type1 == integer_type && type2 == char_type)
        return 1;
    else if(type1 == double_type && type2 == integer_type)
        return 1;
    else if(type1 == double_type && type2 == short_type)
        return 1;
    else if(type1 == double_type && type2 == char_type)
        return 1;
    else if(type1 == short_type && type2 == char_type)
        return 1;
    else if(type1 == char_type && type2 == short_type)
        return 1;
    return 0;
}
