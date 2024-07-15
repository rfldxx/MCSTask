// json -> автомат mealy -> dot (for graphviz)
// -----------------------------------------------------------------------

const char information_format[] = 
"REQUIRE FORMAT:                    \n"
"{                                  \n"
"  \"transitions\": {               \n"
"      \"q_i1\": {                  \n"
"         \"z_j\": { \"state\": \"q_2\",  \"output\": \"w_j\" }, \n"
"          ...                      \n"
"         \"z_n\": { ... }          \n"
"      },                           \n"
"      \"q_i2\": { ... }            \n"
"       .                           \n"
"       .                           \n"
"  },                               \n"
"                                   \n"
"  \"initial_state\": \"q_0\"       \n"
"}                                  \n";

const char information_example[] = 
"EXAMPLE:                                     "        " | CODE:\n"
"{ \"transitions\": {                           "      " | table mealy;\n"
"    \"q1\": {                                  "      " | mealy.read_json(filename);\n"
"      \"a\": {\"state\": \"q3\" ,  \"output\": \"w2\"}, | print_dot_format(cout, mealy);\n"
"      \"b\": {\"state\": \"q2\" ,  \"output\": \"w2\"}  ├────────────────────────\n"
"    },                                       "        " | OUTPUT:\n"
"                                             "        " | digraph G {\n"
"    \"q2\": {                                  "      " |   q2 [color=\"blue\"]\n"
"      \"y\": {\"state\": \"end\",  \"output\": \"w2\"}  | \n"
"    },                                       "        " |   q1 -> q3 [label=\"a/w2\"]\n"
"    \"q2\": {                                  "      " |   q1 -> q2 [label=\"b/w2\"]\n"
"      \"x\": {\"state\": \"q2\" ,  \"output\": \"w1\"}  |   q2 -> q2 [label=\"x/w1\"]\n"
"    },                                       "        " |   q2 -> end [label=\"y/w2\"]\n"
"                                             "        " |   end -> end [label=\"i/qq\"]\n"
"    \"end\": {                                 "      " | }\n"
"      \"i\": {\"state\": \"end\",  \"output\": \"qq\"}  |\n"
"    }                                        "        " |\n"
"  },                                         "        " |\n"
"  \"initial_state\": \"q2\" }                    "    " |\n";



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

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

string change_extension(const string& filename) {
    int l = filename.find_last_of('.');
    if( filename.substr(l, string::npos) == ".dot" ) return {};
    return filename.substr(0, l) + ".dot";
}

int main(int ac, char* av[]) {
    string ifile, ofile;
try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help"    , "produce help message")
        ("input,i" , po::value<string>(&ifile), "file with Mealy machine configuration")
        ("output,o", po::value<string>(&ofile), "file to save DOT format representation");

    po::variables_map vm;        
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    

    if ( vm.count("help")) { 
        cout << desc << "\nNeed information about:\n  * REQUIRE FORMAT [R]\n  * EXAMPLE [E]\n";
        char answer = '-';
        scanf("%c", &answer);
        if(answer == 'R') cout << information_format  << "\n";
        if(answer == 'E') cout << information_example << "\n";
        return 0; 
    }

    if (!vm.count("input")) {
        cout << "Required input file (--input).\n";
        return 2;
    }

    if (!vm.count("output")) {
        if( (ofile = change_extension(ifile)) == "" ) {
            cout << "Input file have extension .dot, but output file undefined.\n";
            return 3;
        }
        cout << "Using to output file: " << ofile << "\n";
    }
    
} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }


    table mealy;
    mealy.read_json(ifile); 

    std::ofstream out(ofile);
    print_dot_format(out, mealy);

    return 0;
}
