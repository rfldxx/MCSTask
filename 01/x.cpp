// json -> ptree -> json

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

template <typename Container>
std::size_t max_size_in_slice(const Container& Cont) { //, std::size_t *get_size(const Container::value_type)) {
    std::size_t result = 0;
    for(auto& e : Cont) //result = std::max(result, get_size(e));
        result = std::max(result, e.first.size());
    return result;
}

#define SPACE(n) std::string(n, ' ')
void print_ptree_helper(std::ostream& out, boost::property_tree::ptree::const_iterator it, int offset = 0, int width = 0) {
    const std::string key_delimetr = " : ";
    const std::string end_delimetr = ";\n";
    std::string key = it->first;

    out << SPACE(offset) << std::setw(width) << key << key_delimetr;

    if(it->second.empty()) {
        out << it->second.get_value<std::string>() << end_delimetr;
        return;
    }
    
    bool first_iteration = 1;
    std::size_t curr_width = max_size_in_slice(it->second); //, [](auto& p){return p.fisrt.size();}); 
    for (auto itt = it->second.begin(); itt != it->second.end(); ++itt) {
        print_ptree_helper(out, itt, offset, curr_width);

        if(first_iteration) {
            offset += key.size() + key_delimetr.size();
            first_iteration = 0;
        }
    }

    out << end_delimetr;
}

std::ostream& operator << (std::ostream& out, boost::property_tree::ptree& t) {

    if(t.empty()) out << "Empty ptree\n";
    else print_ptree_helper(out, t.begin());
    return out;

// for (auto it = t.begin(); it != t.end(); ++it) {
//     // Доступ к ключу и значению каждого элемента
//     std::string key = it->first;
//     // std::string value = it->second.get_value<std::string>();

//     out << "[" << key << ": " ; //<< value << "]\n";
//     if(it->second.empty()) out << it->second.get_value<std::string>();
//     else 
//     for(auto [k, v]: it->second) {
//         out << k << " ~ " << v.get_value<std::string>() << "; ";
//     }
//     out << "]\n";
//     // Делайте что-то с ключом и значением
//     }
//     return out;
}

int main()
{
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("t.json", pt);

    std::cout << pt;

    

    // // Пример чтения значений из JSON файла
    // std::string name = pt.get<std::string>("name");
    // int age = pt.get<int>("age");

    // std::cout << "Name: " << name << std::endl;
    // std::cout << "Age: " << age << std::endl;

    return 0;
}