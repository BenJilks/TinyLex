#include <getopt.h>
#include "lexer.h"
#include "generator.h"

static struct option cmd_options[] = 
{
    {"out",     required_argument,  0,  'o'},
    {"help",    no_argument,        0,  'h'},
};

void show_use()
{
    printf("Usage: tinylex [options] file...\n");
    printf("Options: \n");
    printf("\t-h/--help\t\tShow command help information\n");
    printf("\t-o/--out <file>\t\tOutput code into <file>\n");
}

int main(int argc, char **argv)
{
    char **input_files;
    FILE *output_file;
    int input_file_count;
    int use_output_file;

    // Read arguments
    input_file_count = 0;
    output_file = stdout;
    use_output_file = 0;
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
                output_file = fopen(optarg, "wb");
                use_output_file = 1;
                break;
            
            // Display usage
            case 'h':
                show_use();
                return 0;
        }
    }

    // Read input files
    input_files = argv + optind;
    input_file_count = argc - optind;

    if (input_file_count <= 0)
    {
        printf("Error: No input files given\n");
        printf("Try 'tinylex --help' for more information\n");
        return -1;
    }

    Lexer lex;
    int i;

    lex = lexer_create();
    for (i = 0; i < input_file_count; i++)
    {
        Stream stream = parser_open_stream(input_files[i]);
        lexer_parse(&stream, &lex);
        parser_close_stream(&stream);
    }
    generate_cpp(output_file, &lex);

    if (use_output_file)
        fclose(output_file);
    lexer_free(lex);
    return 0;
}
