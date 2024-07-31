// json -> автомат mealy (+проверка флагов из терминала) -> dot (for graphviz)
// ----------------------------------------------------------------------------

// В ЭТОМ ФАЙЛЕ СДЕЛАТЬ:
// [✓] + считывание обязательного аргумента input_file
// [✓] + проверка автомата (class table)
// [✓] - пренести class table и print_dot_format в отдельный файл

#include "mealy_settings.h"
#include "mealy.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

int main(int argc, char* argv[]) {
    string infile;
    machine_settings settings;
    table mealy;

try {
    // создаем обязательный флаг для выходного файла
    po::options_description read_input("Required input file with configuration");
    read_input.add_options()("in", po::value<string>(&infile)->required(), "file with Mealy machine configuration");

    // парсим комадную строку, с учетом дополнительного po::options_description
    settings.read_command_line(argc, argv, read_input);
    if( !settings.is_actual ) return 0;

    // проверка, что имя входного файла не пусто
    if( infile == "" ) 
        throw std::runtime_error("empty name of input file");

    // если выходной файл не задан, то:  <выходной файл> := <входной файл(без расширения)>.dot
    if ( settings.outfile == "" ) {
        settings.outfile = change_extension(infile);
        cout << "Using to output file: \"" << settings.outfile << "\"\n";
    }

    if( infile == settings.outfile ) 
        throw std::runtime_error("same file for configuration and save: \"" + infile + "\"");

    mealy.read_json(infile);
} catch(exception& e) { cerr << "error: " << e.what() <<  "\n"; return 1; } 
  catch(...)          { cerr << "Exception of unknown type!\n"; return 1; }

    


// проверками корректности описания автомата
    if( !settings.is_correct_states(mealy.state.size()) ) {
        cout << "Uncorrect quantity of states: " << mealy.state.size() << endl;
    }

    bool show_all_err = 1;
    bool trans_out = 1, alph_out = 1, alph_in  = 1;
    // unsigned count_of_err_to_show = -1;
    for(int i = 0; i < mealy.state.size(); i++) {
        string introduce_state = "For state: \"" + mealy.indx2state.at(i) + "\" uncorrect ";

        if( trans_out && !settings.is_correct_trans_out(mealy.state[i].size()) ) {
            cout << introduce_state << "trans_out: " << mealy.state[i].size() << endl;
            trans_out = show_all_err;
        }

        for(const auto& [in, bond] : mealy.state[i]) {
            if( alph_in && !settings.is_correct_alph_in(in)) {
                cout << introduce_state << "alph_in:  \""  << in << "\"" << endl;
                alph_in = show_all_err;
            }
            if( alph_out && !settings.is_correct_alph_out(bond.output)) {
                cout << introduce_state << "alph_out: \"" << bond.output << "\"" << endl;
                alph_out = show_all_err;
            }
        }
    }


// сохраняем dot-представление автомата
    mealy.print_dot_format(settings.outfile);

    return 0;
}


