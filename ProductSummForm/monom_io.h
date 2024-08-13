#ifndef INCULDE_MONOM_IO
#define INCULDE_MONOM_IO

#include "monom_type.h"

#ifndef SHOW_WITH_POWER
#define SHOW_WITH_POWER 0
#else
#undef  SHOW_WITH_POWER
#define SHOW_WITH_POWER 1
#endif

// печать в формате: "- x + 7*y*z + 1 + a"
void print_monom(const monom* a) {
    if( !a ) printf("0");
    for(int start = 1;  a;  a = a->next, start = 0) {
        if( !start ) printf(" ");
        if( !start || a->coef < 0 ) printf("%c ", "+-"[a->coef < 0]);

        int coef  = abs(a->coef);
        if( coef != 1 || (a->variables == 0) ) printf("%d", coef);

        int first = (coef == 1);
        for(list* p = a->variables;  p;  first = 0) {
            if(!first) printf("*");

            int  power   = 1;
            char element = p->elem;
            while( (p = p->next) && SHOW_WITH_POWER && (p->elem == element) ) power++;
            
            printf("%c", element);
            if(power > 1) printf("^%d", power);
        }
    }
}
 

// считывает первый не пробельный символ (только a != ' ')
int read_char(char* a) {
    *a = ' ';
    while( (*a == ' ') && (scanf("%c", a) == 1) ); 
    return  *a != ' ';
}

int is_letter(char a) { return ('a' <= a && a <= 'z'); }


// Обнаружено, что scanf("%d") работает не много не очевидно,
// когда считывается: " + " или " - "
// в этом случае scanf возвращает 0,
// хоть при этом символ знака считан из потока 
// 
// По сути цель создания scan_number - это считать знак
// Возвращаемое значение:   1 - считано число
//                         -1 - считан только знак
//                          0 - иначе
int scan_number(int* val, int* sign, char* symbol) {
    int r = 0, ans = 0, eof = 0;
    char a[2] = {0};
    scanf("%1s", a); // чтобы скипнуть "пробельные" символы

    *sign = 1;
    if(a[0] == '+' || a[0] == '-') {
        *sign = (a[0] == '+' ? +1 : -1);
        scanf("%c", a);
        ans = -1;
    }

    while('0' <= a[0] && a[0] <= '9') {
        r *= 10;
        r += (a[0] - '0');
        ans = 1;
        if( scanf("%c", a) != 1 ) { eof = 1; break; }
    }

    if(ans == 1) *val = r;
    *symbol = !eof ? a[0] : '\n';

    return ans;
}


#define MONOM_LAST  2
#define MONOM_ERROR 3

// функция считывает только мономы вида:  -8*...  или  x*... (т.е. где нет знака перед слагаемым: "- a...")
//
// возвращает знак поcле этого монома (т.е. либо "+1", либо "-1")
// или, если это было последнее слагаемое, то вернется MONOM_LAST
// в случае ошибки форматирования вернется MONOM_ERROR
int read_monom(monom* a, int sign, char* symbol) {
    int coef = 1, sign1 = 1;
    int coef_get = scan_number(&coef, &sign1, symbol);
    if( coef_get == -1 ) return MONOM_ERROR; // для ошибок вида:  "x + + y"
    a->coef = sign*sign1*coef; // для обработки  "+ -3*x"

    // Если моном начинается с числа, то дальше мы ожидаем  "*x"  (иначе  ожидаем  "x")
    int need_read = (coef_get && (*symbol == ' '));
    char delimetr = (coef_get  ?  *symbol  : '*' );
    for( ;  1;  need_read = 1, coef_get = 1) {
        if( need_read &&  !read_char(&delimetr) ) return MONOM_LAST;

        if( delimetr ==  '+' || delimetr == '-' ) return ((const int[2]){-1, +1}[delimetr ==  '+']);
        if( delimetr == '\n' || delimetr ==  13 ) return MONOM_LAST; // на винде в конце строки ^M\n
        if( delimetr != '*' ) return MONOM_ERROR;
        
        // считываем  x
        if( (coef_get && !read_char(symbol)) || !is_letter(*symbol) ) return MONOM_ERROR;
        insert(&(a->variables), *symbol);
    }
}


int read_sp_form(monom** dst) {
    monom* a = init_monom(0);
    *dst = a;

    // вся возня из-за первого монома:  он может быть вида  " - a..."
    // и вся проблема в этом начальном знаке
    int   coef = 1, sign = 1;
    char  symbol;
    int  coef_get = (scan_number(&coef, &sign, &symbol) == 1);
    int   var_get = (!coef_get && symbol != ' ');

    if( var_get ) {
        if( !is_letter(symbol) ) return 0;
        insert(&(a->variables), symbol);
        symbol = ' ';
    }

    int new_monom = 0;
    if( coef_get || var_get ) {
        // в этом случае, возможны варианты:  a*..  или  a + ...  или  a #
        a->coef = sign*coef;

        if( symbol == ' ' && !read_char(&symbol) ) return 1;  // это:  a #

        if( symbol == '+' || symbol == '-' ) { // переходим к следующему моному
            sign = (symbol == '-' ? -1 : +1);
            coef_get = 0;
            new_monom = 1;
        } else  if( symbol !=  '*') { // дальше нет монома
            return (symbol == '\n' || symbol ==  13); // на винде в конце строки ^M\n
        }
    }

    for( ;  1; coef_get = 0, new_monom = 1 ) {
        if(new_monom) {
            a->next = init_monom(0);
            a = a->next;
        }

        int prev_sign = sign;
        sign = read_monom(a, sign, &symbol);

        if(coef_get) a->coef = prev_sign*coef;

        if( sign == MONOM_LAST || sign == MONOM_ERROR )  return (sign == MONOM_LAST);
    }
}

#undef MONOM_LAST
#undef MONOM_ERROR

#endif  //  INCULDE_MONOM_IO
