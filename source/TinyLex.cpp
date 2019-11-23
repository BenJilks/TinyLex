#include <iostream>
#include <getopt.h>
#include "lexer.hpp"
#include "generator/generator_cpp.hpp"

static struct option cmd_options[] = 
{
    {"out",     required_argument,  0,  'o'},
    {"help",    no_argument,        0,  'h'},
};

void show_use()
{
    std::cout << "Usage: tinylex [options] file..." << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "\t-h/--help\t\tShow command help information" << std::endl;
    std::cout << "\t-o/--out <file>\t\tOutput code into <file>" << std::endl;
}

int main(int argc, char **argv)
{
    vector<string> input_files;
    string out_file = "tinylex.hpp";

    // Read arguments
    for (;;)
    {
        int option_index = 0;
        int c = getopt_long(argc, argv, "o:h", 
            cmd_options, &option_index);
        if (c == -1)
            break;
        
        switch (c)
        {
            // Output file
            case 'o':
                out_file = optarg;
                break;
            
            // Display usage
            case 'h':
                show_use();
                return 0;
        }
    }

    // Read input files
    while (optind < argc)
        input_files.push_back(argv[optind++]);

    if (input_files.size() <= 0)
    {
        std::cout << "Error: No input files given" << std::endl;
        std::cout << "Try 'tinylex --help' for more information" << std::endl;
        return -1;
    }
 
    CGenerator generator(out_file);
    Lexer lexer(generator);
    for (string in : input_files)
    {
        Parser parser(in.c_str());
        lexer.parse(parser);
    }

    generator.generate(lexer.get_project_name());
    return 0;
}
