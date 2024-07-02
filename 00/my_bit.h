#include <climits>
#include <string>
#include <bitset>

// кол-во бит в байте
#define BYTE_BIT 8


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

        if( i == (sizeof(T))*BYTE_BIT ) return 0;
        power <<= 1;
        i++;
    }
}




// ==========================================================================
// Реализовать класс битового вектора произвольной разрядности.

class bits {
public:

// (я бы сделал это конструктором, но вроде нельзя вызывать контруктор "самому")
// (т.е. например нельзя вызвать что-то по типу:   this->bits(n)               )
    void alloc_bits(std::size_t n, bool need_malloc = 1) {
        N       =  n;
        noideal = (N % CHAR_BIT != 0);
        blocks  = (N / CHAR_BIT) + noideal;

        // malloc - принимает сколько байт необходимо аллоцировать
        // однако я работаю с данными размера char (котыре не факт что один байт)
        // поэтому тут есть множитель (CHAR_BIT / BYTE_BIT) 
        //  (уж не будем предполагать что CHAR_BIT может быть не кратен BYTE_BIT)
        data  =  (need_malloc && blocks) ? malloc(blocks * (CHAR_BIT / BYTE_BIT)) : 0;
    }

    bits(std::size_t n, bool need_malloc = 1) { alloc_bits(n, need_malloc); }

//    - конструктор (разрядность, массив переменных интегрального типа);
// ( можно было бы принимать контейнер через template и обходить все его элементы, )
// ( но кажется это относительно бесполезно                                        )
    bits(std::size_t n, const void* src) {
        alloc_bits(n);

        for(int i = 0; i < blocks; i++) {
            ((char*)data)[i] = ((char*)src)[i];
        }
    }

//    - для тестирования
#ifdef TEST_MODE
    bits(std::vector<bool> v) {
        alloc_bits(v.size());
        
        // добавим в конец нулей чтоб не париться далее
        for(int i = 0; i < CHAR_BIT; i++) v.push_back(0);

        char* dst = (char*) data;
        for(int i = 0; i < blocks; i++) {
            char package = 0;

            for(int j = 0; j < CHAR_BIT; j++) {
                package |= ( v[i*CHAR_BIT + j] << (CHAR_BIT-1 - j) );
            }

            *dst++ = package;
        }
    }
#endif

    ~bits() { if(blocks) free(data); }


//    - разрядность ();
    size_t size() const { return N; }


//    - установить битовое поле (смещение, битвектор);
    void set(int k, const bits& filler) {
        unsigned char* dst = ((unsigned char*)data) + (k / CHAR_BIT);
        unsigned char* src =  (unsigned char*)filler.data;

        int shift = CHAR_BIT - (k % CHAR_BIT);
        int  tail = (filler.noideal ? filler.N%CHAR_BIT : CHAR_BIT);
        bool   tt = ( tail <= shift );

        unsigned char prefix = dst[0] >> shift;
        for(int i = 0; i < filler.blocks - tt; i++) {
            *dst++ = char_combine(prefix, shift, src[0]);
            prefix = *src++; 
        }

        int delta; // длина последнего перфикса
        if( tt ) {
            delta  = CHAR_BIT - shift + tail;
            prefix = (prefix << tail) | (src[0] >> (CHAR_BIT -  tail));
        } else {
            delta  =  tail - shift;
            prefix = prefix >> (CHAR_BIT - tail);
        }

        dst[0] = char_combine(prefix, CHAR_BIT - delta, dst[0] << delta);
    }


//    - получить битовое поле (смещение, разрядность);
    bits get(int k, int l) const {
        // по сути эта операция эквивалентна set
        // только сейчас у нас dst и filler поменяны местами
        //  + нам нужен не весь filler а только его часть, вмещающаюсю в dst
        // => для этого можно в ручную захордкодить длинну: this->N = ...

        bits result(l);
        
        unsigned char* src = ((unsigned char*)data) + (k / CHAR_BIT);
        unsigned char* dst = ((unsigned char*)result.data);

        int delta =  k%CHAR_BIT;
        int shift =  CHAR_BIT - delta;
        dst[0]    =  src[0] << delta;
        
        if( l <= shift ) {
            return result;
        }

        // создаем копию "себя", чтобы при изменении длинны не потерять констатность метода
        bits copy(l - shift, false);
        copy.data = (void*) ( src + 1 );

        result.set(shift, copy);
        
        copy.data = 0; // чтоб деструктор ничего не стирал
        return result;
    }


//    - получить битовое поле (смещение, разрядность) в переменную
// интегрального типа: отдельный вариант для битовых полей малой
// разрядности (до 64 разрядов);
    template<typename T = long long int>
    T get_in_var(int k, int l) const {
        bits x = get(k, l);
        
        // да-а, конечно было бы меньше возни если бы set и get
        // работали бы с void* а не с bits
        T result;
        for(int i = 0; i < (sizeof(T)) * BYTE_BIT / CHAR_BIT; i++) {
            ((char*)&result)[i] = ((char*)x.data)[i];
        }

        // Сдвиг - чтобы информативные биты были выравнены по правому краю:
        //  было (l == 3) :  | 10110... | .........|
        //  функция выдаст:  | ........ | ...10110 |
        return result >> ( (sizeof(T))*BYTE_BIT - l );
    }


//    - получить строковое представление в 16-ричном виде.
// ( Последовательно просматривает по 4 бита из data,                          )
// ( если size не кратен 4, то data смотрится дальше, пока не станет кратным.  )  
// ( Чтобы в конце data были нули надо дополнительно применить .sweep()        )
    std::string str(const int width = 4) const {
        // width  -  кол-во бит на число 0..15 = xxxx
        // !!! CHAR_BIT должен быть кратен width !!! (а то запариться можно)
        const int multiplicity = CHAR_BIT / width;

        char mask = 0;   // создается mask = 11110000 (при width = 4) 
        for(int j = 0; j < width; j++) {
            mask <<= 1;
            mask  |= 1;
        }
        
        int n = (N / width) + (N%width != 0);
        std::string result(n, '-');

        char* src = (char*)data;
        for(int i = 0; i < n; ) {
            char curr = *src++;

            for(int j = i+multiplicity-1; j >= i; j--) {
                if(j < n)
                    result[j] = bits_to_char( (curr & mask) );
                curr >>= width;
            }
            i += multiplicity;
        }

        return result;
    } 

    // ставим нули в конце data ( где мусорные значения, по сути нужно только для .str() )
    void sweep() {
        if( noideal ) {
            int unusing = CHAR_BIT - (N % CHAR_BIT);
            unsigned char last   = ((unsigned char*)data)[blocks-1];
            ((unsigned char*)data)[blocks-1] = (last >> unusing) << unusing;
        }
    }

#ifndef TEST_MODE
private:
#endif

    void*  data;
    size_t N;
    size_t blocks;   // сколько блоков размера CHAR_BIT замалоченно
    bool   noideal;  // показывает, что последний блок заполнен не полностью

    inline unsigned char char_combine(unsigned char a, int ka, unsigned char b) {
        return (a << ka) | (b >> (CHAR_BIT - ka));
    }

    char bits_to_char(char bits) const { return ( bits < 10 ? '0' : 'A'-10 ) + bits; }
};




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
#define GEN_random_select(container, insertion, ...)                                \
template <typename T comma_tpn(typename, __VA_ARGS__) >                             \
container<T comma_tpn(,__VA_ARGS__)>                                                \
random_select(const container<T comma_tpn(,__VA_ARGS__)>& A, int k) {               \
    auto take = random_take(A.size(), k);                                           \
    container<T comma_tpn(,__VA_ARGS__)> result;                                    \
    int i = 0;                                                                      \
    for(auto e : A) {                                                               \
        if( take[i++] ) result. insertion (e);                                      \
    }                                                                               \
    return result;                                                                  \
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

