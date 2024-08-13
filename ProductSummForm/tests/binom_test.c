#include "../monom.h"

// создает:  a-b
monom* create_factor() {
    monom* factor = init_monom(0);
    // +a
    factor->coef = 1;
    factor->variables = create('a', 0);
    factor->next = init_monom(0);
    // -b
    factor->next->coef = -1;
    factor->next->variables = create('b', 0);
    return factor;
}

// Узнает сколько вначале элементов как первый.
// В переменую >p< сохраняется указатель на первый отличившийся элемент.
int count_of_elements(const list* a, const list** p) {
    int  a_power = 0;
    for( *p = a; *p && ((*p)->elem == a->elem); (*p) = (*p)->next ) a_power++;
    return a_power;
}


// как будто в моей реализации в умножении порядок
// слагаемых в списке оказывается "каноничным" для бинома:
//      a^n - n*a^(n-1)*b + n*(n-1)/2 * ...
int is_correct_binom(const monom* a, int n) {
    int   coef = 1;
    const list*  p;
    for(int i = 0; i < n; i++) {
        if( !a ) return 0;

        if( a->coef != coef ) return 0;

        const list* p;
        if( count_of_elements(a->variables, &p) != n-i ) return 0; // a ^ (n-i)
        if( count_of_elements(p           , &p) != i   ) return 0; // b ^ i
        if( count_of_elements(p           , &p) != 0   ) return 0; // больше ничего нет

        coef = -coef*(n-i)/(i+1);
        a = a->next;
    }
    
    if( !a || a->next != 0 ) return 0; // это должно быть последним слагаемым
    if( count_of_elements(a->variables, &p) != n ) return 0; // в конце b^n
    if( count_of_elements(p           , &p) != 0 ) return 0; // больше ничего нет
    
    return 1;
}


int main() {
// factor = a-b
    monom* factor = create_factor();
    
// переменая, накапливающая (a-b)^n
    monom* binom  = init_monom(0);
    binom->coef = 1;
    

    int   result  =  1;
    const int  n  = 29;
    for(int power =  1; result && power <= n; power++) {
        monom* prev = binom;
        binom = mul(factor, prev);
        del_monom(prev);

        result &= is_correct_binom(binom, power);

        if(!result) printf("\033[91mError\033[0m in power: %d\n", power);
    }

    del_monom(factor);
    del_monom(binom );

    if( result ) printf("Binom (a-b)^%d - \033[92mwork CORRECTLY!\033[0m\n", n);
    return !result;
} 
