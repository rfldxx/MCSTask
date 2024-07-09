// json -> ptree -> json
// 
// read json  to   boost::property_tree::ptree,  and
// then print like-json format by pass through ptree
// 
// EXAMPLE:                                  | OUTPUT:
// { "name": {                               |  [name]: [aa ]: 1
//     "aa" : 1,   "d": "44",                |          [d  ]: 44
//     "abc": { "1": "s", "dv": 3},          |          [abc]: [1 ]: s
//       "x":                                |                 [dv]: 3
//         { "my" : "name", "test": "lain",  |          [x  ]: [my  ]: name
//           "o p": 0.01  , "exp" : 2.0   }  |                 [test]: lain
// }, "age": "123" }                         |                 [o p ]: 0.01
//                                           |                 [exp ]: 2.0
//                                           |  [age ]: 123


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

template <typename T, typename Container>
T max_in_layer(const Container& Cont, T (*get_size)(const typename Container::value_type&)) {
    T result = 0;
    for(const auto& e : Cont) result = std::max(result, get_size(e));
    return result;
}

void print(std::ostream& out, const boost::property_tree::ptree& t, int shift = 0) {
    const std::string ender = "\n";
    bool no_first = 0;

    int width = max_in_layer<std::size_t>(t, [](const auto& p){return p.first.size();});
    for(auto& [k, v] : t) {
        if( no_first ) out << ender << std::string(shift, ' ');
        no_first = 1;

        out << "[" << std::left << std::setw(width) << k << "]: ";
        
        if( !v.empty() ) print(out, v, shift+width+4);  // + 4 символа на: "[]: "
        else             out << v.get_value<std::string>();
    }
}

std::ostream& operator << (std::ostream& out, const boost::property_tree::ptree& t) {
    if( t.empty() ) out << "Empty ptree";
    else print(out, t);
    return out;
}

int main(int argc, char* argv[]) {
    char *o = argv[1];
    char *p = o;
    std::string filename = (char*)((const char* []){"test.json", o})[0];

    boost::property_tree::ptree pt;
    boost::property_tree::read_json("t.json", pt);

    std::cout << "EXAMPLE:\n";
    std::cout << pt;
    std::cout << std::endl;
    
    return 0;
}