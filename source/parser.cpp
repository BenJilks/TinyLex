#include "parser.hpp"
#include "debug.h"
#include <iostream>

#define RED     "\033[1;31m"
#define ORANGE  "\033[01;33m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"

Parser::Parser(const char *file_path)
{
    // Open file stream and get the first char
    in = std::ifstream(file_path);
    eof_flag = false;
    column = 0;
    line = "";
    next_char();

    LOG("Opened source file '%s'\n", file_path);
}

void Parser::skip_white_space()
{
    // Read until a char that is not a space is reached
    while (isspace(look))
        next_char();
}

char Parser::get_look()
{
    return look;
}

char Parser::next_char()
{
    // Return the current look and get the next one
    char temp = look;
    
    look = in.get();
    if (look == '\n')
    {
        output_line_errors();
        line = "";
        column = 0;
    }
    else
    {
        line += look;
        column += 1;
    }

    if (look == EOF)
        eof_flag = true;
    return temp;
}

string Parser::next_word()
{
    // Read the next string of numbers or letters
    string word = "";
    while (isalnum(look) || look == '_' || look == '.')
    {
        word += look;
        next_char();
    }

    return word;
}

void Parser::output_line_errors()
{
    for (auto error : errors)
    {
        std::cout << RED << "Error: " << error.first << std::endl;
        std::cout << WHITE << "\t" << line << std::endl;
        
        std::cout << "\t";
        for (int i = 0; i < error.second; i++)
            std::cout << " ";
        std::cout << RED << "~" << RESET << std::endl;
    }
    errors.clear();
}

void Parser::match(char c)
{
    if (look != c)
    {
        error("Expected '" + string(&c, 1) + "' got '" + 
            string(&look, 1) + "' instead");
    }

    next_char();
}

void Parser::error(string msg)
{
    errors.push_back(std::make_pair(msg, column - 1));
}

Parser::~Parser()
{
    in.close();
}
