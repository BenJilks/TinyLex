#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <fstream>
using std::string;
using std::pair;
using std::vector;

class Parser
{
public:
    Parser(const char *file_path);
    ~Parser();

    inline bool is_eof() const { return eof_flag; }
    void skip_white_space();
    char get_look();
    char next_char();
    string next_word();

    void match(char c);
    void error(string msg);

private:
    void output_line_errors();
    std::string line;
    int column;

    char look;
    bool eof_flag;
    std::ifstream in;
    vector<pair<string, int>> errors;

};
