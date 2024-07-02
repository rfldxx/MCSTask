#ifndef INCLUDE_MACRO_2
#define INCLUDE_MACRO_2

#include <bitset>

#include <string>
#include <malloc.h>

#include <climits>  // for CHAR_BIT
#define BYTE_BIT 8  // кол-во байт в бите


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
        // (уж не будем предполагать что CHAR_BIT может быть не кратен BYTE_BIT)
        data  =  (need_malloc && blocks) ? malloc(blocks * (CHAR_BIT / BYTE_BIT)) : 0;
    }

    bits(std::size_t n, bool need_malloc = 1) { alloc_bits(n, need_malloc); }

//    - конструктор (разрядность, массив переменных интегрального типа);
    template <typename T>
    bits(std::size_t n, const T* src, bool raw_copy = 0) {
        alloc_bits(n);

        if( !raw_copy ) {  // копируем, "платформо-защищено" через mask = 1..11
            reap(src, (char*)data, blocks, CHAR_BIT);
            return;
        }
        
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
    bits& set(int k, const bits& filler) {
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
        return *this;
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
        dst[0]    =  src[0]  << delta;
        
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
    T get_in_var(int k, int l, bool right_alignment = 1) const {
        // Выравнивание по правому краю для l = 5 (right_alignment = 1): 
        //            | ........ | ...10110 |
        // По левому: | 10110... | .........|

        bits  x  = get(k, l);
        T result = 0;

        // относительно наивная реализация
        unsigned char* src = (unsigned char*) x.data; 
        for(int i = 0; i < x.blocks-1; i++, src++) {
            int shift = l - (i+1)*CHAR_BIT;
            result |= ((T)src[0]) << shift;
        }
        int tail = (x.noideal ? l % CHAR_BIT : CHAR_BIT);
        result |= src[0] >> (CHAR_BIT - tail); 

        if( !right_alignment && (l != sizeof(T)*BYTE_BIT) ) result <<= (sizeof(T)*BYTE_BIT - l);
        return result;
    }


//    - получить строковое представление в 16-ричном виде.
// ( Последовательно просматривает по 4 бита из data,                          )
// ( если size не кратен 4, то data смотрится дальше, пока не станет кратным.  )
    std::string str(const int width = 4) const {
        // width  -  кол-во бит на число 0..15 = xxxx
        sweep();  // Чтобы в конце data были нули

        int n = (N / width) + (N%width != 0);
        std::string result(n, '-');

        reap((char*)data, (char*)(result.data()), n, width);

        for(int i = 0; i < n; i++) result[i] = bits_to_char(result[i]);
        return result;
    } 

// ставим нули в конце data ( где мусорные значения, по сути нужно только для .str() )
    void sweep() const {
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

    char bits_to_char(char bits) const { return ( bits < 10 ? '0' : 'a'-10 ) + bits; }

    template <typename T>
    void reap(const T* src, char* dst, std::size_t count, int width) const {
        T mask = 0;  // width-штук единичек в конце: mask = 00001111 (при width = 4)
        for(int j = 0; j < width; j++) {
            mask <<= 1;
            mask  |= 1;
        }

        // сколько блоков размера width-бит вместиться в src[0]
        int multiplicity = sizeof(T) * BYTE_BIT / width;

        for(int i = 0; i < count; src++) {
            for(int j = 0; i < count && j < multiplicity-1; i++, j++) {
                int shift = (multiplicity-1 - j)*width;

                // дополнительно в конце делаем & mask, чтобы после сдвига влево
                // знаковой переменной отбросить созданный "мусор" 
                dst[i] = ((src[0] & (mask << shift)) >> shift) & mask;
            }

            // Последнюю итрецию проводим отдельно,
            // чтобы коректно отработать случай когда sizeof(T)*BYTE_BIT == width
            // для знаковой переменной T
            // Потому что даже когда делается `mask << 0` ловится:
            //   runtime error: left shift of negative value -1
            if( i < count ) {
                dst[i++] = src[0] & mask;
            }
        }
    }
};

#endif  // INCLUDE_MACRO_2
