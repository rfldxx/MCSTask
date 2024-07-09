// json -> ptree -> json
// 
// read json  to   boost::property_tree::ptree,  and
// then print like-json format by pass through ptree
// 
// EXAMPLE:                                  | OUTPUT:
// { "name": {                               | [name]: [aa ]: 1
//     "aa" : 1,     "d": "44",              |         [d  ]: 44
//     "abc": { "1": "s", "dv": 3},          |         [abc]: [1 ]: s
//       "x":                                |                [dv]: 3
//         { "my" : "name", "test": "lain",  |         [x  ]: [my  ]: name
//           "o p": 0.01  , "exp" : 2.0   }  |                [test]: lain
// }, "age": "123" }                         |                [o p ]: 0.01
//                                           |                [exp ]: 2.0
//                                           | [age ]: 123


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

int main(int argc, char* argv[]) {
    std::string filename = (const char* []){"test.json", argv[1]}[argc != 1];

    boost::property_tree::ptree pt;
    try { boost::property_tree::read_json(filename, pt); }
    catch(...) { std::cout << "Can't read JSON from \"" << filename << "\"\n"; return 1; }

    std::cout << "File \"" << filename << "\":\n";
    std::cout << pt;
    std::cout << std::endl;
    
    return 0;
}
