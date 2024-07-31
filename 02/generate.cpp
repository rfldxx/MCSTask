#include "../01/mealy.h"
#include "../01/mealy_settings.h"
#include <random>


using namespace std;

// std::map<std::string, int> state2indx;
// std::map<int, std::string> indx2state; // здесь по идеи можно использовать vector вместо map
//                                        // ( или вообще хранить это в векторе state )
// struct transition { int pos; int output; }; 
// std::vector<std::map<int, transition>> state; 

int main(int argc, char* argv[]) {
    machine_settings settings;
    table mealy;
try {
    settings.read_command_line(argc, argv);
    if( !settings.is_actual ) return 0;

    if( settings.outfile == "" ) {
        settings.outfile =  "generated.json";
    }

} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }


// для прототипа (потом изменить)
    std::random_device rd; // Инициализация генератора случайных чисел
    std::mt19937 mt(rd()); // Mersenne Twister 19937 генератор случайных чисел
    int maxN = (settings.n_states.max == -1) ? 200 : settings.n_states.max;
    std::uniform_int_distribution<unsigned> dist(settings.n_states.min, maxN); // Определение диапазона
 

    // число состояний в автомате
    unsigned N = dist(mt);

    // делаем слои:  x -> x -> x  длинны до mlt от ещё неиспользованных состояний
    const unsigned min_in_layer = 10;
    double mlt = 0.25;
    unsigned M = N; // еще не исполльзованные состояния
    unsigned c = 0;
    while( M ) {
        // кол-во вершин на текущем слое
        unsigned n = M;
        if( (M-1)*mlt + 1 > min_in_layer) {
            std::uniform_int_distribution<int> dist_n(min_in_layer, (M-1)*mlt + 1);
            n = dist_n(mt);
        }
        M -= n;


        cout << "layer: " << n << endl;
        c += n;
    }

    cout << c << " =?= " << N << endl;    
}