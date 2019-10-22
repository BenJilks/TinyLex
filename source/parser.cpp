#include "parser.hpp"
#include "debug.h"

Parser::Parser(const char *file_path)
{
    in = std::ifstream(file_path);
    look = in.get();
    LOG("Opened source file '%s'\n", file_path);
}

void Parser::skip_white_space()
{
    // Read until a char that is not a space is reached
    while (isspace(look))
        look = in.get();
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
    return temp;
}

string Parser::next_word()
{
    string word = "";
    while (isalnum(look) || look == '_' || look == '.')
    {
        word += look;
        look = in.get();
    }

    return word;
}

Parser::~Parser()
{
    in.close();
}
