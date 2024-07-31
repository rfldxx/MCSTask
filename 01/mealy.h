#ifndef INCLUDEMACRO_MACHINE
#define INCLUDEMACRO_MACHINE

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


class table {
public:
    // у переходов игнорируем первую букву и сохраняем только число
    bool read_json(std::string filename) {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename, pt);

        for(const auto& [q, v] : pt.get_child("transitions")) {
            int q_indx = upg(q);

            for(const auto& [z, vv] : v) {                
                // проверка "алфавитов"
                if( z[0] != 'z' ) // continue;
                    throw std::runtime_error("uncorrect  input prefix: " + z + " for state " + q);
                auto w  = vv.get<std::string>("output");
                if( w[0] != 'w' ) 
                    throw std::runtime_error("uncorrect output prefix: " + w + " for transition "  + q + "[" + z + "]");


                auto q2 = vv.get<std::string>( "state");
                int  q2_indx = upg(q2);
                
                // добавляем связь q -> q2 (in: z / out: w)
                assert( state[q_indx].count(stoi( z.substr(1) )) == 0 );
                state[q_indx][stoi( z.substr(1) )] = {q2_indx, stoi( w.substr(1) )};
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
                    << " [label=\"z" << z << "/w" << tr.output << "\"]\n";
            }
        }
        out << "}\n";
    }


    int initial_pos;

    std::map<std::string, int> state2indx;
    std::map<int, std::string> indx2state; // здесь по идеи можно использовать vector вместо map
                                           // ( или вообще хранить это в векторе state )
    struct transition { int pos; int output; }; 
    std::vector<std::map<int, transition>> state; 

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
