#ifndef GENORATOR_H
#define GENORATOR_H

#include "lexer.h"

void write_file(FILE *output, const char *file_path);
void generate_cpp(FILE *output, Lexer *lex);

/*
class Generator
{
public:
    Generator();
    Generator(string file_path);
    ~Generator();

    void add_expression(string name, ExpressionTable *expresstion);
    virtual void generate(string project_name) = 0;

protected:
    vector<pair<string, ExpressionTable*>> expressions;
    void write_string(string msg);
    void write_line(string line);
    void write_file(string file_path);

private:
    std::ofstream out_file;
    std::ostream &out;

};
*/
#endif // GENORATOR_H
