// json -> ptree -> json

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

std::ostream& operator << (std::ostream& out, boost::property_tree::ptree& t) {
for (auto it = t.begin(); it != t.end(); ++it) {
    // Доступ к ключу и значению каждого элемента
    std::string key = it->first;
    // std::string value = it->second.get_value<std::string>();

    out << "[" << key << ": " ; //<< value << "]\n";
    if(it->second.empty()) out << it->second.get_value<std::string>();
    else 
    for(auto [k, v]: it->second) {
        out << k << " ~ " << v.get_value<std::string>() << "; ";
    }
    out << "]\n";
    // Делайте что-то с ключом и значением
    }
    return out;
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