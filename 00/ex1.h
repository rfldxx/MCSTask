#ifndef INCLUDE_MACRO_1
#define INCLUDE_MACRO_1

#define BYTE_BIT 8  // кол-во байт в бите

// ==========================================================================
// Реализовать функцию от аргумента a интегрального типа, возвращающую
// признак того, что a является степенью двойки:
//    f (a) == ((\exists i \in \N \cup {0}): a == 2^i)

template<typename T>
bool  is_pow_of_2(T a) {
    std::size_t i = 0;
    T  power = 1; // = 2^i
    while( 1 ) {
        if( a == power ) return 1;

        i++;
        if( i == (sizeof(T))*BYTE_BIT ) return 0;
        power <<= 1;
    }
}

#endif  // INCLUDE_MACRO_1
