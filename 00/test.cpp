#include <iostream>
#include <vector>
#include <random>

#define  TEST_MODE 
#include "my_bit.h"


#define SHOW_LINE(cond, ...) if( cond ) {                           \
    std::cout << "  error in " << __LINE__ << " line";              \
    printf(":   " __VA_ARGS__);  std::cout << std::endl;            \
    error_count++;                                                  \
}

#define TEST_FUNC(f) std::cout << "\nSTART TEST " #f << std::endl;  \
if( temp = test_##f () ) {                                          \
    std::cout << "Total error's: " << temp << std::endl;            \
    ret++;                                                          \
} else std::cout << "OK" << std::endl;




// ==========================================================================
// test_is_pow_of_2

// возвращает:  0 - OK,  1 - ошибка
template <typename T>
bool test_exact_powers() {
    T one = 1;
    for(int i = 0; i < sizeof(T)*BYTE_BIT; i++) 
        if( !is_pow_of_2(one << i) )
            return 1;
    
    return 0;
}

// возвращает кол-во не удачных тестов
int test_is_pow_of_2() {
    int error_count = 0;

    SHOW_LINE(test_exact_powers<char>())
    SHOW_LINE(test_exact_powers<int>())
    SHOW_LINE(test_exact_powers<long long int>())
    
    // SHOW_LINE(test_exact_powers<__int128>())
    
    return error_count;
}




// ==========================================================================
// class bits

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> distribution(0, 1);

std::vector<bool> random_vector(int n) {
    std::vector<bool> v(n);
    for(int i = 0; i < n; i++) v[i] = distribution(gen);
    return v;
}

void set_vct(std::vector<bool>& vct, const std::vector<bool>& plt, int k) {
    for(int i = 0; i < plt.size(); i++) vct[i+k] = plt[i];
}

std::vector<bool> get_vct(const std::vector<bool>& vct, int k, int l) {
    std::vector<bool> result(l);
    for(int i = 0; i < l; i++) result[i] = vct[i+k];
    return result;
}

bool operator == (const std::vector<bool>& v, const bits& b) {
    if( v.size() != b.size() ) return 0;

    unsigned char* dst = (unsigned char*)b.data;
    for(int i = 0; i < b.size(); dst++) {
        for(int j = 0; i < b.size() && j < CHAR_BIT; j++, i++) {
            bool bit = ( dst[0] >> (CHAR_BIT-1-j) ) & 1;

            if( v[i] != bit ) return 0;
        }
    }
    return 1;
}

int test_set_all_diapasons(int N, int M) {
    std::vector<bool> init_cv = random_vector(N); // classic vector
    std::vector<bool> palette = random_vector(M);

    bits mbv(init_cv); // my bit vector
    bits plt(palette);

    if( !(init_cv == mbv) ) return 1;
    
    for(int shift = 0; shift <= N-M; shift++) {
        set_vct(init_cv, palette, shift);
        mbv.set(shift, plt);

        if( !(init_cv == mbv) )  return 1;
    }

    return 0;
}

int test_get_all_diapasons(int N, int M) {
    std::vector<bool> init_cv = random_vector(N); // classic vector
    bits mbv(init_cv); // my bit vector

    if( !(init_cv == mbv) ) return 1;

    for(int shift = 0; shift <= N-M; shift++) {
        auto ideal = get_vct(init_cv, shift, M);
        auto getts = mbv.get(shift, M);

        if( !(ideal == getts) ) return 1;
    }

    return 0;
}

int test_class_bits() {
    int error_count = 0;

    for(int dn = 1; dn < 150; dn++)
        for(int M = 1; M < 150; M++) {
            SHOW_LINE( test_set_all_diapasons(M+dn, M), "[N=%d M=%d]", M+dn, M );
            SHOW_LINE( test_get_all_diapasons(M+dn, M), "[N=%d M=%d]", M+dn, M );
        }

    return error_count;
}


int test_hand_check() {
    bits x(10, "\0\0");  // : 0000000000
    char c = -1;
    bits b1(1, &c);      // : 1
    bits b2(2, &c);      // : 11

    x.set(2, b2);        // : 0011000000
    x.set(3, b2);        // : 0011100000
    x.set(8, b2);        // : 0011100011

    x.set(6, b1);        // : 0011101011
    
    x.sweep();
    auto s1 = x.str( );  // : "3ac"
    auto s2 = x.str(2);  // : "03223"
    auto s3 = x.str(1);  // : "0011101011"

    int error_count = 0;
    
    SHOW_LINE(s1 != std::string("3AC")       )
    SHOW_LINE(s2 != std::string("03223")     )
    SHOW_LINE(s3 != std::string("0011101011"))

    return error_count; 
}




// ==========================================================================
// не стал делать тестирование, просто убедимся вручную что работает
int test_random_select() {
// random_select from VECTOR
    int k = 3;
    std::vector<std::pair<int, char>> x{ {1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}, {6, 'f'}, {7, 'g'} };
    auto rx = random_select(x, k);

    std::cout << "VECTOR (random " << k << "):\n";
    for(auto [k, v] : rx) {
        std::cout << "  {" << k << ": " << v << "}\n";
    }


// random_select from SET
    k = 4;
    std::set<std::pair<int, char>> y{ {1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}, {6, 'f'}, {7, 'g'} };
    auto ry = random_select(y, k);

    std::cout << "\nSET (random " << k << "):\n";
    for(auto [k, v] : ry) {
        std::cout << "  {" << k << ": " << v << "}\n";
    }


// random_select from MAP
    k = 7;
    std::map<int, char> z{ {1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}, {6, 'f'}, {7, 'g'} };
    auto rz = random_select(z, k);

    std::cout << "\nMAP (random " << k << "):\n";
    for(auto [k, v] : rz) {
        std::cout << "  {" << k << ": " << v << "}\n";
    }

    return 0;
}



int main() {
    int temp, ret = 0;

    TEST_FUNC(is_pow_of_2)

    TEST_FUNC(hand_check)
    TEST_FUNC(class_bits)

    std::cout << "--------" << std::endl;
    std::cout << "Total invalid test's: " << ret << std::endl << std::endl;


// это чисто "показательные" тесты
    TEST_FUNC(random_select);
    std::cout << "--------" << std::endl;

    return ret;
}

#undef TEST_FUNC
#undef SHOW_LINE

