#pragma once
#include "expression.hpp"
#include <fstream>
#include <vector>
#include <tuple>
using std::vector;
using std::pair;

class Generator
{
public:
    Generator();
    Generator(string file_path);
    ~Generator();

    void add_expression(string name, ExpressionTable *expresstion);
    virtual void generate() = 0;

protected:
    vector<pair<string, ExpressionTable*>> expressions;
    void write_string(string msg);
    void write_line(string line);

private:
    std::ofstream out_file;
    std::ostream &out;

};
