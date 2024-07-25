// json -> автомат mealy (+проверка флагов из терминала) -> dot (for graphviz)
// ----------------------------------------------------------------------------

// В ЭТОМ ФАЙЛЕ СДЕЛАТЬ:
// + считывание обязательного аргумента input_file
// + проверка автомата (class table)
// - пренести class table и print_dot_format в отдельный файл

#include "machine_settings.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <vector>
#include <map>


using namespace std;

class table {
public:
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
                assert( state[q_indx].count(z) == 0 );
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
    string ofile, ifile;
    machine_settings given_settings;
try {
    given_settings.read(argc, argv);

    // if (!vm.count("input")) {
    //     cout << "Required input file (--input).\n";
    //     return 2;
    // }

    // if (!vm.count("output")) {
    //     if( (ofile = change_extension(ifile)) == "" ) {
    //         cout << "Input file have extension .dot, but output file undefined.\n";
    //         return 3;
    //     }
    //     cout << "Using to output file: " << ofile << "\n";
    // }
    
} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }

    if( !given_settings.is_actual ) return 0;

    table mealy;
    mealy.read_json(ifile); 

    std::ofstream out(ofile);
    print_dot_format(out, mealy);

    return 0;
}
