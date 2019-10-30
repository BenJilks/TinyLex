#pragma once
#include "generator.hpp"

class CGenerator : public Generator
{
public:
    CGenerator() {}
    CGenerator(string file_path) :
        Generator(file_path) {}
    
    virtual void generate();

private:
    void gen_expression(string name, ExpressionTable *exp);
    void gen_token_type();
    void gen_type_table();

};
