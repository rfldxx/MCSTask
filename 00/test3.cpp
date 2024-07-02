#include "ex3.h"
#include <iostream>
#include <cmath>

// ==========================================================================
// не стал делать особого тестирования, просто убедимся глазами что работает

using namespace std;

void test_frequency(unsigned n, unsigned k, unsigned repeats = 1'000) {
    vector<int> f(n, 0);
    vector<int> A(n);
    for(int i = 0; i < n; i++) A[i] = i;

    for(int i = 0; i < repeats; i++) {
        auto sample = random_select(A, k);
        for(auto j : sample) f[j]++;
    }

    unsigned minv = repeats, maxv = 0, mean = 0; 
    for(int i = 0; i < n; i++) {
        if(f[i] < minv) minv = f[i];
        if(f[i] > maxv) maxv = f[i];

        mean += f[i];
    }

    unsigned mean_delta = 0; // mean_delta := 1/n \sum_i |x_i - <x>|
    for(int i = 0; i < n; i++) {
        mean_delta += abs( (int) (n*f[i] - mean) );
    }

    printf("(next value's in procents)\n");
    printf("Ideal mean for (n = %d k = %d):  k/n = %8.2f\n", n, k, k*100./n);
    printf("     min  <---| mean | --->  max\n");
    printf("%8.2f     %8.2f   %8.2f\n", minv*100./(repeats), mean*100./(n*repeats), maxv*100./(repeats));
    printf("mean abs delta = %.3f\n", mean_delta * 100. / n/n/repeats);
}



#define  unwrap(...) __VA_ARGS__

// столько раз передаю аргументы в макросах - чтобы __LINE__ нормально "раскрылся"
#define   MACRO(a1, a2) tMACRO(a1, a2, __LINE__)
#define  tMACRO(...)   ttMACRO(__VA_ARGS__)
#define ttMACRO(container_name, container_declaration, num)     \
  unwrap container_declaration x##num{ {1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}, {6, 'f'}, {7, 'g'} }; \
  auto r##num = random_select(x##num, k);                       \
  std::cout << container_name << " (random " << k << "):\n";    \
  for(auto [k, v] : r##num) { std::cout << "  {" << k << ": " << v << "}\n"; }


void test_random_select() {
// random_select from VECTOR
    int k = 3;
    MACRO("VECTOR", (std::vector<std::pair<int, char>>))

// random_select from SET
    k = 4;
    MACRO("SET", (std::set<std::pair<int, char>>))

// random_select from MAP
    k = 7;
    MACRO("MAP", (std::map<int, char>))
}



int main() {
// это чисто "показательные" тесты
    
    test_frequency(200, 180);
    
    std::cout << "--------" << std::endl;

    test_random_select();
}


// === [OUTPUT:] ============================================================
// (next value's in procents)
// Ideal mean for (n = 200 k = 180):  k/n =    90.00
//      min  <---| mean | --->  max
//    87.20        90.00      93.00
// mean abs delta = 0.725
// --------
// VECTOR (random 3):
//   {4: d}
//   {5: e}
//   {6: f}
// SET (random 4):
//   {1: a}
//   {2: b}
//   {3: c}
//   {6: f}
// MAP (random 7):
//   {1: a}
//   {2: b}
//   {3: c}
//   {4: d}
//   {5: e}
//   {6: f}
//   {7: g}

