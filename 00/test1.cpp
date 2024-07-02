#include <iostream>
#include <vector>
#include <random>

#define  TEST_MODE 
#include "ex1.h"

using namespace std;

#define SHOW_LINE_IF(cond, ...) if( cond ) {                \
    cout << "  error in " << __LINE__ << " line";           \
    printf(":   " __VA_ARGS__);  cout << endl;              \
    error_count++;                                          \
}

#define TEST_FUNC(f) cout << "\nSTART TEST " #f << endl;    \
if( temp = test_##f () ) {                                  \
    cout << "Total error's: " << temp << endl;              \
    ret++;                                                  \
} else cout << "OK" << endl;


// ==========================================================================
// test_is_pow_of_2

// возвращает:  0 - OK,  1 - ошибка
template <typename T>
bool test_exact_powers_T() {
    T one = 1, zero = 0;
    for(int i = 0; i < sizeof(T)*BYTE_BIT; i++) {
        if( is_pow_of_2(one << i) != ( (one<<i) > zero) )
            return 1;
    }
    
    return 0;
}

// возвращает кол-во не удачных тестов
int test_exact_powers() {
    int error_count = 0;

    SHOW_LINE_IF(test_exact_powers_T<char>())
    SHOW_LINE_IF(test_exact_powers_T<int>())
    SHOW_LINE_IF(test_exact_powers_T<long long int>())
    
    // SHOW_LINE(test_exact_powers_T<__int128>())
    
    return error_count;
}

int test_special() {
    int error_count = 0;

    int      a1 = (1<<31);
    SHOW_LINE_IF(is_pow_of_2(a1) != 0, "a1 = %11d", a1)

    unsigned b1 = (1<<31);
    SHOW_LINE_IF(is_pow_of_2(b1) != 1, "b1 = %11u", b1);

    return error_count;
}


int main() {
    int temp, ret = 0;

    TEST_FUNC(exact_powers)
    TEST_FUNC(special)

    std::cout << "--------" << std::endl;
    std::cout << "Total invalid test's: " << ret << std::endl << std::endl;

    return ret;
}

#undef TEST_FUNC
#undef SHOW_LINE

