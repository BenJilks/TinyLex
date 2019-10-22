#include "generator/generator.hpp"
#include <iostream>

Generator::Generator() :
    out(std::cout) {}

Generator::Generator(string file_path) :
    out(out_file)
{
    out_file = std::ofstream(file_path);
}

void Generator::add_expression(string name, ExpressionTable *expression)
{
    expressions.push_back(std::make_pair(name, expression));
}

void Generator::write_string(string msg)
{
    out << msg;
}

void Generator::write_line(string line)
{
    out << line << std::endl;
}

Generator::~Generator()
{
    if (out_file.is_open())
        out_file.close();
    
    for (auto exp : expressions)
        delete exp.second;
}
