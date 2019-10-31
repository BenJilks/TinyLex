#pragma once
#include "generator.hpp"
#include <functional>

typedef pair<string, ExpressionTable*> Expression;
class CGenerator : public Generator
{
public:
    CGenerator() {}
    CGenerator(string file_path) :
        Generator(file_path) {}
    
    virtual void generate();

private:
    void gen_for_each_type(string name, 
        std::function<string(Expression)> func);
    void gen_expression(string name, ExpressionTable *exp);
    void gen_token_type();
    void gen_type_table();

};
