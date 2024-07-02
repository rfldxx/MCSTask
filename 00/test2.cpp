#include <iostream>
#include <vector>
#include <random>

using namespace std;
#define  TEST_MODE 
#include "ex2.h"
#include <bitset>

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

    for(int dn = 1; dn < 70; dn++)
        for(int M = 1; M < 70; M++) {
            SHOW_LINE_IF( test_set_all_diapasons(M+dn, M), "[N=%d M=%d]", M+dn, M );
            SHOW_LINE_IF( test_get_all_diapasons(M+dn, M), "[N=%d M=%d]", M+dn, M );
        }

    return error_count;
}


int test_hand_check() {
    char c = -1;
    bits b1(1, &c);      // : 1
    bits b2(2, &c);      // : 11  
    
    bits x(10, "\0\0");  // : 0000000000

    x.set(2, b2);        // : 0011000000
    x.set(3, b2);        // : 0011100000
    x.set(8, b2);        // : 0011100011

    x.set(6, b1);        // : 0011101011
    
    auto s1 = x.str( );  // : "3ac"
    auto s2 = x.str(2);  // : "03223"
    auto s3 = x.str(1);  // : "0011101011"

    int error_count = 0;
    
    SHOW_LINE_IF(s1 != std::string("3ac")       )
    SHOW_LINE_IF(s2 != std::string("03223")     )
    SHOW_LINE_IF(s3 != std::string("0011101011"))


    uint64_t init_arr[] = { 0x1122334455667708, 0xbabadedacafebeef, 0x4354659710928523, 0xcafebabe8794abcd };
    
    bits my_bits(256, init_arr);
    SHOW_LINE_IF(my_bits.str() != string("1122334455667708babadedacafebeef4354659710928523cafebabe8794abcd"))
    
    auto doubt = my_bits.set(10, my_bits.get(0,8)).str();
    SHOW_LINE_IF(  doubt       != string("1104734455667708babadedacafebeef4354659710928523cafebabe8794abcd"))

    return error_count; 
}


int main() {
    int temp, ret = 0;

    TEST_FUNC(class_bits)
    TEST_FUNC(hand_check)

    std::cout << "--------" << std::endl;
    std::cout << "Total invalid test's: " << ret << std::endl << std::endl;

    return ret;
}

#undef TEST_FUNC
#undef SHOW_LINE

