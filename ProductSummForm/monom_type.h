#ifndef INCULDE_MONOM_TYPE
#define INCULDE_MONOM_TYPE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// === [ список множителей ] ==================== 
struct list_ {
    char   elem;
    struct list_* next;
};
typedef struct list_ list;

int is_list_equals(const list* a, const list* b) {
    while( a && b && (a->elem == b->elem) ) a = a->next, b = b->next;
    return (!a && !b);
}

list* create(char a, list* next) {
    list* neww = malloc(sizeof(list));
    neww->elem = a;
    neww->next = next;
    return neww;
}

void insert(list** zero, char a) {
    while (*zero && (*zero)->elem < a) {
        zero  =  &( (*zero)->next );
    }

    *zero = create(a, *zero);
}

// объединяет копии списков a и b (сохраняя отсортированость)
list* collect_list(const list* a, const list* b) {
    list*  ans = 0;
    list** scr = &ans;

    while( 1 ) {
        const list** c = (const list** [2]){&a, &b} [ (!a) ? 1 : (b && b->elem < a->elem) ];

        if( !*c ) return ans; 

        *scr = create( (*c)->elem, 0);
         scr  =   &( (*scr)->next );
        (*c)   =       (*c)->next;
    }
}

void del_list(list* a) {
    while( a ) {
        list* next = a->next;
        free(a);
        a = next;
    }
}


// === [ список  слагаемых ] ==================== 
struct monom_ {
    int    coef;
    list*  variables;
    struct monom_* next;
};
typedef struct monom_ monom;

monom* init_monom(monom* a) {
    monom* neww = ( a ? a : malloc(sizeof(monom)) );
    neww->variables = 0;
    neww->next = 0;
    return neww;
}

void erase_monom(monom** prev) {
    monom* temp = (*prev);
    (*prev) = (*prev)->next;
    del_list(temp->variables);
    free(temp);
}

void del_monom(monom* a) {
    while( a ) {
        monom* next = a->next;
        del_list(a->variables);
        free(a);
        a = next;
    }
}

#endif  //  INCULDE_MONOM_TYPE
