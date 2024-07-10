// json -> автомат mealy -> dot (for graphviz)
// -----------------------------------------------------------------------
// 
// REQUIRE FORMAT:
// "transitions": q_i1: z_j: "state" : q_2  
//                 |         "output": w_j
//                 |    ...
//                 └    z_n: ...
//                q_i2: ...
//                 └    ...
//                 .
//                 .
// 
// "initial_state": q_0
// -----------------------------------------------------------------------
// 
// EXAMPLE:                                      | CODE:
// { "transitions": {                            | table mealy;
//     "q1": {                                   | mealy.read_json(filename);
//       "a": {"state": "q3" ,  "output": "w2"}, | cout << mealy;
//       "b": {"state": "q2" ,  "output": "w2"}  ├────────────────────────
//     },                                        | OUTPUT:
//                                               | (q1):
//     "q2": {                                   |     a/w2 -> q3
//       "y": {"state": "end",  "output": "w2"}  |     b/w2 -> q2
//     },                                        | (q3):
//     "q2": {                                   | {q2}:
//       "x": {"state": "q2" ,  "output": "w1"}  |     x/w1 -> q2
//     },                                        |     y/w2 -> end
//                                               | (end):
//     "end": {                                  |     i/qq -> end
//       "i": {"state": "end",  "output": "qq"}  |
//     }                                         |
//   },                                          |
//   "initial_state": "q2" }                     |



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class table {
public:
    // template <typename Map, typename Vect>
    // int upg(Map& promo, Vect& v, string name) {  // updated pos get
    //     if( !promo.count(name) ) {
    //         pos2state[v.size()] = name;
    //         promo[name] = v.size();
    //         v.push_back({});
    //     }
    //     return promo[name];
    // }

    int upg(const string& name) {
       if( !state2pos.count(name) ) {
            state2pos[name] = state.size();
            pos2state[state.size()] = name;
            state.push_back({});
        }
        return state2pos[name]; 
    }

    bool read_json(string filename) {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename, pt);

        int count_state = 0, count_input = 0;

        // вопрос: умеется ли оптимизироваться такое итерирование
        //         for(auto& x : pt.get_child("transitions"))  ??
        const auto transitions = pt.get_child("transitions");
        for(const auto& [q, v] : transitions) {
            int q_indx = upg(q);

            for(const auto& [z, vv] : v) {
                // int input_indx = upg(input2pos, state[q_indx], z);

                auto w  = vv.get<string>("output");
                auto q2 = vv.get<string>( "state");
                int  q2_indx = upg(q2);
                
                // добавляем связь q -> q2 (in: z / out: w)
                assert( state[q_indx].count(z) == 0);
                state[q_indx][z] = {q2_indx, w};
            }
        }

        initial_pos = state2pos.at(pt.get<string>("initial_state"));
        return 1;
    }
    

    int initial_pos;

    // map<string, int> input2pos;
    map<string, int> state2pos;
    map<int, string> pos2state;
    struct transition { int pos; string output; }; 
    vector<map<string, transition>> state; 
};

std::ostream& operator << (std::ostream& out, const table& t) {
    for(int i = 0; i < t.state.size(); i++) {
        bool init = (i == t.initial_pos);
        out << "({"[init] << t.pos2state.at(i) << ")}"[init] << ":";
        out << "\n";
        for(const auto& [z, tr] : t.state[i]) {
            out << "    " << z << "/" << tr.output << " -> " << t.pos2state.at(tr.pos) << "\n";
        }
    }
    return out;
}

void print_dot_format(std::ostream& out, const table& t) {
    out << "digraph G {\n";

    // выделяем начальную вершину
    out << "  " << t.pos2state.at(t.initial_pos) << " [color=\"blue\"]\n\n";

    // проходим все связи
    for(int i = 0; i < t.state.size(); i++) {
        const string& q = t.pos2state.at(i);
        for(const auto [z, tr] : t.state[i]) {
            out << "  " << q << " -> " << t.pos2state.at(tr.pos)
                << " [label=\"" << z << '/' << tr.output << "\"]\n";
        }
    }
    out << "}\n";
}

int main(int argc, char* argv[]) {
    std::string filename = (const char* []){"mealy.json", argv[1]}[argc != 1];

    table mealy;
    mealy.read_json(filename);
    // cout << mealy;

    print_dot_format(cout, mealy);

    return 0;
}
