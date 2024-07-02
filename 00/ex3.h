#ifndef INCLUDE_MACRO_3
#define INCLUDE_MACRO_3

// ==========================================================================
// Реализовать функцию, для данного набора входных элементов (A)
// возвращающую случайную выборку из k элементов (k <= size_of (A)). Все
// возможные выборки должны быть равновероятны. Элементы входного набора
// считать попарно различными.

#include <random>

std::vector<bool> random_take(int N, int k) {
    bool inv = (2*k >= N);
    if(inv) k = N-k;
    
    std::vector<bool> result(N, inv);
    
    for(int i = 0; i < k; i++) {
        int pos = rand() % (N-i);

        int accum = -1, j = -1;
        do {
            j++;
            if( result[j] == inv ) {
                accum++;
            }
        } while( accum != pos );

        result[j] = !inv;
    }

    return result;
}


// Дальше идет блок макросов, нацеленных в итоге на макрос comma_tpn
// (В g++ нет особенной работы с >,##__VA_ARGS__<, по сравнению с gcc)
// comma_tpn(prefix, ...):   xx, _ -> _
//                           xx, a -> , xx a
#define  CAT(a, b) a##b
#define wCAT(...) CAT(__VA_ARGS__)

#define  SECOND(a, b, ...) b
#define wSECOND(...) SECOND(__VA_ARGS__)

#define   eval(...) __VA_ARGS__
#define unwrap(...) __VA_ARGS__

#define colappse ~, ()
#define    is_tpn(prefix, ...) CAT(colap, __VA_ARGS__ pse), (, prefix __VA_ARGS__),
#define comma_tpn(...) eval( unwrap wSECOND(is_tpn(__VA_ARGS__)) )


// Пришлось столько городить, так как у контейнеров нет "одинаковой" вставки
// В vector - push_back,  в map/set - insert
#define GEN_random_select(container, insertion, ...)                    \
template <typename T comma_tpn(typename, __VA_ARGS__) >                 \
container<T comma_tpn(,__VA_ARGS__)>                                    \
random_select(const container<T comma_tpn(,__VA_ARGS__)>& A, int k) {   \
    auto take = random_take(A.size(), k);                               \
    container<T comma_tpn(,__VA_ARGS__)> result;                        \
    int i = 0;                                                          \
    for(auto e : A) {                                                   \
        if( take[i++] ) result. insertion (e);                          \
    }                                                                   \
    return result;                                                      \
}



#include <map>
#include <set>

GEN_random_select(std::vector, push_back)
GEN_random_select(std::set,    insert   )
GEN_random_select(std::map,    insert   , V)


#undef  CAT
#undef wCAT
#undef  SECOND
#undef wSECOND
#undef  eval
#undef unwrap
#undef colappse
#undef    is_tpn
#undef comma_tpn
#undef GEN_random_select


#endif  // INCLUDE_MACRO_3
