#include <stdio.h>
#include <string.h>

#include "monom.h"

void (*sadd[]) (monom**, monom*) = {sub, add};

int main() {
    char operation[2];
    if(  scanf("%1s", operation) != 1 ) return 1;
    if( !strchr("+-*=", operation[0]) ) return 2;

    monom* a = 0;
    monom* b = 0;
    
    int returned_value = 0;
    if( !read_sp_form(&a) || !read_sp_form(&b) ) {
        returned_value = 3;
        goto deleting;
    }

    if(operation[0] == '+' || operation[0] == '-') {
        sadd[operation[0]=='+'](&a, b);
        print_monom(a); printf("\n"); 
        b = 0;
    }
    else if(operation[0] == '*') {
        monom* result = mul(a, b);
        print_monom(result); printf("\n"); 
          del_monom(result);
    }
    else if(operation[0] == '=') {
        simplify(&a);
        simplify(&b);

        if( !is_equal(&a, b) ) printf("not ");
        printf("equal"); 
        printf("\n");  
    }
    else returned_value = 4;

    
deleting:
    if(a) del_monom(a);
    if(b) del_monom(b);

    return returned_value;
}
 