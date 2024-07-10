// json -> автомат mealy -> dot (for graphviz)
// 
// read json  in   boost::property_tree::ptree,  and
// then print like-json format by pass through ptree
// 



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

template <typename T, typename Container>
T max_in_layer(const Container& Cont, T (*get_size)(const typename Container::value_type&)) {
    T result = 0;
    for(const auto& e : Cont) result = std::max(result, get_size(e));
    return result;
}

void print_ptree(std::ostream& out, const boost::property_tree::ptree& t, int shift = 0) {
    const std::string ender = "\n";
    bool no_first = 0;

    int width = max_in_layer<std::size_t>(t, [](const auto& p){return p.first.size();});
    for(auto& [k, v] : t) {
        if( no_first ) out << ender << std::string(shift, ' ');
        no_first = 1;

        out << "[" << std::left << std::setw(width) << k << "]: ";
        
        if( !v.empty() ) print_ptree(out, v, shift+width+4);  // + 4 символа на: "[]: "
        else             out << v.get_value<std::string>( );
    }
}

std::ostream& operator << (std::ostream& out, const boost::property_tree::ptree& t) {
    if( t.empty() )  out << "Empty ptree";
    else print_ptree(out, t);
    return out;
}

#include <map>
#include <vector>
using namespace std;
class table {
public:
    template <typename Map, typename Vect>
    int upg(Map& promo, Vect& v, string name) {  // updated pos get
        if( !promo.count(name) ) {
            pos2state[v.size()] = name;
            promo[name] = v.size();
            v.push_back({});
        }
        return promo[name];
    }

    bool read_json(string filename) {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename, pt);

        int count_state = 0, count_input = 0;

        // вопрос: умеется ли оптимизироваться такое итерирование
        //         for(auto& x : pt.get_child("transitions"))  ??
        const auto transitions = pt.get_child("transitions");
        for(const auto& [q, v] : transitions) {
            int q_indx = upg(state2pos, state, q);

            for(const auto& [z, vv] : v) {
                // int input_indx = upg(input2pos, state[q_indx], z);

                auto w  = vv.get<string>("output");
                auto q2 = vv.get<string>( "state");
                int  q2_indx = upg(state2pos, state, q2);
                
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

int main(int argc, char* argv[]) {
    std::string filename = (const char* []){"mealy.json", argv[1]}[argc != 1];

    table mealy;
    mealy.read_json(filename);
    cout << mealy;
    return 0;

    boost::property_tree::ptree pt;
    try { boost::property_tree::read_json(filename, pt); }
    catch(...) { std::cout << "Can't read JSON from \"" << filename << "\"\n"; return 1; }

    auto g = pt.get_child("transitions");

    std::cout << "File \"" << filename << "\":\n";
    std::cout << g;
    std::cout << std::endl;
    
    return 0;
}
