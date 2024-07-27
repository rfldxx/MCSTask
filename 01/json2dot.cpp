// json -> автомат mealy (+проверка флагов из терминала) -> dot (for graphviz)
// ----------------------------------------------------------------------------

// В ЭТОМ ФАЙЛЕ СДЕЛАТЬ:
// [✓] + считывание обязательного аргумента input_file
// [ ] + проверка автомата (class table)
// [✓] - пренести class table и print_dot_format в отдельный файл

#include "mealy_settings.h"
#include "mealy.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

int main(int argc, char* argv[]) {
    string infile;
    machine_settings mealy_settings;
    table mealy;

try {
    po::options_description read_input("Required input file with configuration");
    read_input.add_options()("in", po::value<string>(&infile)->required(), "file with Mealy machine configuration");

    mealy_settings.read_command_line(argc, argv, read_input);
    if( !mealy_settings.is_actual ) return 0;

    if ( mealy_settings.outfile == "" ) {
        mealy_settings.outfile = change_extension(infile);
        cout << "Using to output file: \"" << mealy_settings.outfile << "\"\n";
    }


    if( infile == mealy_settings.outfile ) 
        throw std::runtime_error("same file for configuration and save: " + infile);

    // if( !infile.size() )


    mealy.read_json(infile);
} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }


    mealy.print_dot_format(mealy_settings.outfile);

    return 0;
}


