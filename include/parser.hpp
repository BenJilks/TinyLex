#pragma once
#include <string>
#include <fstream>
using std::string;

class Parser
{
public:
    Parser(const char *file_path);
    ~Parser();

    void skip_white_space();
    char get_look();
    char next_char();
    string next_word();

    inline bool is_eof() const { return eof_flag; }

private:
    char look;
    bool eof_flag;
    std::ifstream in;

};
