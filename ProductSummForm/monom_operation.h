#ifndef INCULDE_MONOM_OPERATION
#define INCULDE_MONOM_OPERATION

#include "monom_type.h"

// находит слагаемое с тем же набором переменных, что и в sample
// (коэфициент перед перед слагаемым никак не влияет)
monom** find_monom(monom** src, const monom* sample) {
    // if( !sample ) return 0;
    while(*src && !is_list_equals((*src)->variables, sample->variables))
        src = &( (*src)->next );
    return src;
}

void simplify(monom** a) {
    while( *a ) {
        monom** finds = &((*a)->next);
        while( (finds = find_monom(finds, *a)) && (*finds) ) {
            (*a)->coef += (*finds)->coef;
            erase_monom(finds);
        }

        if( (*a)->coef == 0 ) erase_monom(a);
        else  a = &( (*a)->next );
    }
}


// === [   =   ] =========================
// (коректная работа только для канонических sp-форм)
//  Перебираются поочередно слагаемые из >b<,
//  и если в >a< оказалось такое же слагаемое, то оно  удаляется из >a<
//  Возращается указатель, на первый элемент из >b< которого нет  в >a<
const monom* trancate(monom** a, const monom* b) {
    // Проходим по всем слагаемым в >b< и удаляем их в >a<
    for( ;  b;  b = b->next ) {
        monom** finds = find_monom(a, b);
        if(  !(*finds) || (*finds)->coef != b->coef ) break;
        erase_monom(finds);
    }
    return b;
}

// (коректная работа только для канонических sp-форм)
int is_equal(monom** a, const monom* b) { return (trancate(a, b) == 0) && (*a == 0); }



// === [ + / - ] =========================
// выполняется:  a += b  (после функции сущность monom* b - будет упразднена)
void add(monom** a, monom* b) {
    monom** start = a;
    while( *a )  a = &( (*a)->next );
    *a = b;
    simplify(start);
}

// выполняется:  a *= -1
void inverse(monom* a) { while(a) a->coef = -a->coef, a = a->next; }

// выполняется:  a -= b 
void sub(monom** a, monom* b) { inverse(b); add(a, b); }



// === [   *   ] =========================
// перемножает два монома
monom* simple_mul(const monom* a, const monom* b) {
    monom* neww = init_monom(0);
    neww->coef = a->coef * b->coef;
    neww->variables = collect_list(a->variables, b->variables);
    return neww;
}

// выполняется:  a * b 
monom* mul(const monom* a, const monom* b) {
    monom*  result   = 0;
    monom** scroller = &result;

    // перебираем все пары
    for( ; a; a = a->next)
        for(const monom* p = b; p; p = p->next) {
            *scroller = simple_mul(a, p);
             scroller = &( (*scroller)->next );
        }

    simplify(&result);
    return    result ;
}


#endif  //  INCULDE_MONOM_OPERATION
