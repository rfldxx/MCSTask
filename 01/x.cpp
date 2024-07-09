// json -> ptree -> json

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

template <typename Container>
std::size_t max_in_layer(const Container& Cont, std::size_t (*get_size)(const typename Container::value_type&)) {
    std::size_t result = 0;
    for(auto& e : Cont) 
        result = std::max(result, get_size(e));
        // result = std::max(result, e.first.size());
    return result;
}

void print(std::ostream& out, const boost::property_tree::ptree& t, int shift = 0) {
    const std::string ender = "\n";
    bool no_first = 0;

    int width = max_in_layer(t, [](const auto& p){return p.first.size();}); // + 2 символа на: "[]"
    for(auto& [k, v] : t) {
        if( no_first ) out << ender << std::string(shift, ' ');
        no_first = 1;

        out << "[" << std::left << std::setw(width) << k << "]: ";
        
        if( !v.empty() ) print(out, v, shift+width+4);
        else             out << v.get_value<std::string>();
    }
}

std::ostream& operator << (std::ostream& out, const boost::property_tree::ptree& t) {
    if( t.empty() ) out << "Empty ptree";
    else print(out, t);
    return out;
}

int main() {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("t.json", pt);

    std::cout << "EXAMPLE:\n";
    std::cout << pt;
    std::cout << std::endl;
    
    return 0;
}