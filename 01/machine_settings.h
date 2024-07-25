#ifndef INCLUDEMACRO_MACHINE_SETTINGS
#define INCLUDEMACRO_MACHINE_SETTINGS


#include <string>
#include <iostream>
#include <exception>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


std::string change_extension(const std::string& filename) {
    int l = filename.find_last_of('.');
    if( filename.substr(l, std::string::npos) == ".dot" ) return {};
    return filename.substr(0, l) + ".dot";
}

struct machine_settings {
    struct limits { unsigned min = 0, max = -1; };
    
    std::string outfile;
    limits n_states;
    limits n_trans_out;
    limits n_alph_in, n_alph_out;
    bool is_actual = 0;

    // machine_settings (int argc, char* argv[]) {
    void read(int argc, char* argv[]) {  //, string out_flag_desc = "file to save result of program") {
        po::options_description desc("Allowed options");

        #define   STR(a) #a
        #define tREAD_RANGE(name, side, desc) ( STR(name##_##side), po::value<unsigned>(&name.side), desc)
        #define  READ_RANGE(name, desc) tREAD_RANGE(name, min, "minimum " desc) tREAD_RANGE(name, max, "maximum " desc)
        desc.add_options()
            ("help"    , "produce help message")
            ("seed"    , "-")
            // ("input,i" , po::value<string>(&ifile), "file with Mealy machine configuration") ! сделать его отдельным, обязательным в json2dot
            ("output,o", po::value<std::string>(&outfile), "file to save DOT format representation")
            READ_RANGE(n_states,    "allowed number of states in machine")
            READ_RANGE(n_trans_out, "allowed number of transitions from the node")
            READ_RANGE(n_alph_in,   "allowed count  of characters in  input alphabet")
            READ_RANGE(n_alph_out,  "allowed count  of characters in output alphabet");
        #undef   STR
        #undef tREAD_RANGE
        #undef  READ_RANGE

        po::variables_map vm;        
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        if ( vm.count("help") ) { 
            std::cout << desc << "\nNeed information about:\n  * REQUIRE FORMAT [R]\n  * EXAMPLE [E]\n";
            char answer = '-';
            scanf("%c", &answer);
            if(answer == 'R') std::cout << information_format  << "\n";
            if(answer == 'E') std::cout << information_example << "\n";
            return; 
        }

        //throw std::runtime_error("An error occurred in constructor!");
        
        is_actual = 1;
    }


    /* static */ const char information_format[700] = 
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

    /* static */ const char information_example[1500] = 
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
};


#endif  // INCLUDEMACRO_MACHINE_SETTINGS
