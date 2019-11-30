#include <getopt.h>
#include <string.h>
#include "lexer.h"
#include "generator.h"

static struct option cmd_options[] = 
{
    {"out",         required_argument,  0,  'o'},
    {"language",    required_argument,  0,  'l'},
    {"help",        no_argument,        0,  'h'},
};

void show_use()
{
    printf("Usage: tinylex [options] file...\n");
    printf("Options: \n");
    printf("\t-h/--help\t\t\tShow command help information\n");
    printf("\t-l/--language <c,cpp>\t\tSet the output language\n");
    printf("\t-o/--out <file>\t\t\tOutput code into <file>\n");
}

int main(int argc, char **argv)
{
    char **input_files;
    int input_file_count;

    FILE *output_file;
    int use_output_file;

    char *lang = "c";

    // Read arguments
    input_file_count = 0;
    output_file = stdout;
    use_output_file = 0;
    for (;;)
    {
        int option_index = 0;
        int c = getopt_long(argc, argv, "o:l:h", 
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
            
            case 'l':
                lang = optarg;
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
        if (!stream.eof_flag)
        {
            lexer_parse(&stream, &lex);
            parser_close_stream(&stream);
        }
    }

    if (!strcmp(lang, "cpp"))
        generate_cpp(output_file, &lex);
    else
        generate_c(output_file, &lex);

    if (use_output_file)
        fclose(output_file);
    lexer_free(lex);
    return 0;
}
