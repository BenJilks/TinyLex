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

void Generator::write_file(string file_path)
{
    // Open file and find its length
    std::ifstream in("../tinylex/" + file_path);
    in.seekg(0L, std::ifstream::end);
    int len = in.tellg();
    in.seekg(0L, std::ifstream::beg);

    // Read data to buffer
    vector<char> data(len);
    in.read(&data[0], len);
    in.close();

    // Write it to the out file
    out.write(&data[0], data.size());
}

Generator::~Generator()
{
    if (out_file.is_open())
        out_file.close();
    
    for (auto exp : expressions)
        delete exp.second;
}
