// генерация в формате .dot
// пока что генерируется раздельные слои:
//   x -> x -> x -> x
//   x -> x
//   x -> x -> x

// расматривается ли переходы, где различен только входной алфавит, например:
//  q1 -> q2 (in: z1 / out: w1)
//  q1 -> q2 (in: z2 / out: w1)

#include "../01/mealy.h"
#include "../01/mealy_settings.h"
#include "../00/ex3.h"
#include <random>

using namespace std;


vector<int> random_poses(int N, int k, int myself = -1) {
    vector<int> nums;
    for(int i = 0; i < N; i++)
        if( i != myself ) nums.push_back(i);
    cout << "CALL poses " << myself << "(" << k << ")" << endl;
    return random_select(nums, k);
}

// заполняем state2indx и indx2state для кооректного вывода
void zaglushka(table& a) {
    for(int i = 0; i < a.state.size(); i++) {
        a.indx2state[i] = "q" + to_string(i);
        // cout << 
        a.state2indx["q" + to_string(i)] = i;
    }
}

int main(int argc, char* argv[]) {
    machine_settings settings;
    table mealy;
try {
    settings.read_command_line(argc, argv);
    if( !settings.is_actual ) return 0;

    if( settings.outfile == "" ) {
        settings.outfile =  "generated.dot";
    }

} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }


// для прототипа (потом изменить)
    std::random_device rd; // Инициализация генератора случайных чисел
    std::mt19937 mt(rd()); // Mersenne Twister 19937 генератор случайных чисел
    unsigned minN = (settings.n_states.min ==  0) ? 5  : settings.n_states.min;
    unsigned maxN = (settings.n_states.max == -1) ? 50 : settings.n_states.max;
    std::uniform_int_distribution<unsigned> dist(minN, maxN); // Определение диапазона
 

    // число состояний в автомате
    unsigned N = dist(mt);
    mealy.state.resize(N);

    // делаем слои:  x -> x -> x  длинны до mlt от ещё неиспользованных состояний
    const unsigned min_in_layer = 5;
    double mlt  = 0.25;

    double k = 0.75; // "связь" между слоями (какая часть по сравнению с равномерно распределенным графом)

    if( settings.n_trans_out.max == -1) settings.n_trans_out.max = 10;
    if( settings.n_trans_out.min ==  0) settings.n_trans_out.min =  4;
    unsigned T  = (settings.n_trans_out.min + settings.n_trans_out.max) / 2; // среднее число исходящих связей
    unsigned M  = N; // еще не исполльзованные состояния
    unsigned pc = 0; // кол-во состояний на прошлом слое
    unsigned c  = 0; // кол-во уже использованных состояний
    unsigned t  = 0; // счетчик связей
    while( M ) {
        // кол-во вершин на текущем слое
        unsigned n = M;
        if( (M-1)*mlt + 1 > min_in_layer) {
            std::uniform_int_distribution<int> dist_n(min_in_layer, (M-1)*mlt + 1);
            n = dist_n(mt);
        }
        cout << "layer: " << n << endl;

        for(int i = 0; i < n-1; i++) {
            mealy.state[c+i][t++] = {c+i+1, 0}; 
        }


        // прокидываем связи между соседними слоями
        for(int i = 0; i < pc; i++) {
            std::uniform_int_distribution<int> dist_j(0, min((unsigned)(c*k*(T-1)/(N-1)), settings.n_trans_out.max));
            for(int j : random_poses(n, dist_j(mt), i)) {
                mealy.state[c-1 - i][t++] = {c+j, 0};
            }
        }

        M -= n;
        c += n;
        pc = n;
    }

    cout << c << " =?= " << N << endl;    

    
    // дозаполняем отсавшиеся trans_out
    for(int i = 0; i < N; i++) {
        std::uniform_int_distribution<int> dist_t(settings.n_trans_out.min, settings.n_trans_out.max);
        unsigned tt = dist_t(mt);

        // связей больше чем хотим  или  уже как надо
        if( mealy.state[i].size() >= tt ) {
            auto m = random_select(mealy.state[i], mealy.state[i].size() - tt);
            for(const auto& elem : m) {
                cout << "For state q" << i << " remove transition: z" << elem.first << " to state " << elem.second.pos << endl;
                mealy.state[i].erase(elem.first);
            }
            break;
        }

        set   <unsigned>    ocuped; // вершины в которые есть переход из текущего состояния
        vector<unsigned> available; // вершины между которыми будем выбирать для построения пути

        // for(const auto& [k, tr]: mealy.state[i]) ocuped.insert(tr.pos);
        // available.re
        
    }


try {
    zaglushka(mealy);
    mealy.print_dot_format(settings.outfile);
}catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
// std::map<std::string, int> state2indx;
// std::map<int, std::string> indx2state; // здесь по идеи можно использовать vector вместо map
//                                        // ( или вообще хранить это в векторе state )
// struct transition { int pos; int output; }; 
// std::vector<std::map<int, transition>> state; 

}