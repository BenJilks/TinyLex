#include <iostream>

#define TINYLEX_IMPLEMENT
#define TEST_IMPLEMENT
#include "lexer.h"

using namespace Test;
using std::string;

int main()
{
    Lexer lex("test.txt");

    while (!lex.is_eof())
    {
        Token token = lex.next();
        string data = lex.read_string(token.data_index, token.len);
        printf("%s,%s\n", data.c_str(), token.type_name);
    }

    return 0;
}
