#ifndef INCLUDEMACRO_MACHINE
#define INCLUDEMACRO_MACHINE


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


class table {
public:
    bool read_json(std::string filename) {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename, pt);

        // вопрос: умеется ли оптимизироваться такое итерирование
        //         for(auto& x : pt.get_child("transitions"))  ??
        const auto transitions = pt.get_child("transitions");
        for(const auto& [q, v] : transitions) {
            int q_indx = upg(q);

            for(const auto& [z, vv] : v) {
                // int input_indx = upg(input2pos, state[q_indx], z);

                auto w  = vv.get<std::string>("output");
                auto q2 = vv.get<std::string>( "state");
                int  q2_indx = upg(q2);
                
                // добавляем связь q -> q2 (in: z / out: w)
                assert( state[q_indx].count(z) == 0 );
                state[q_indx][z] = {q2_indx, w};
            }
        }

        initial_pos = state2indx.at(pt.get<std::string>("initial_state"));
        return 1;
    }


    void print_dot_format(std::string filename) const {
        std::ofstream out(filename);
        if( !out ) throw std::runtime_error("can't open file to out: \"" + filename + "\"");

        out << "digraph G {\n";

        // выделяем начальную вершину
        out << "  " << indx2state.at(initial_pos) << " [color=\"blue\"]\n\n";

        // проходим все связи
        for(int i = 0; i < state.size(); i++) {
            const std::string& q = indx2state.at(i);
            for(const auto [z, tr] : state[i]) {
                out << "  " << q << " -> " << indx2state.at(tr.pos)
                    << " [label=\"" << z << '/' << tr.output << "\"]\n";
            }
        }
        out << "}\n";
    }


    int initial_pos;

    // map<string, int> input2pos;
    std::map<std::string, int> state2indx;
    std::map<int, std::string> indx2state; // здесь по идеи можно использовать vector вместо map
                                           // ( или вообще хранить в векторе state )
    struct transition { int pos; std::string output; }; 
    std::vector<std::map<std::string, transition>> state; 

private:
    int upg(const std::string& name) {
       if( !state2indx.count(name) ) {
            state2indx[name] = state.size();
            indx2state[state.size()] = name;
            state.push_back({});
        }
        return state2indx[name]; 
    }
};


#endif  // INCLUDEMACRO_MACHINE
