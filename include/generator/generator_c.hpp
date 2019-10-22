#pragma once
#include "generator.hpp"

class CGenerator : public Generator
{
public:
    CGenerator() {}
    CGenerator(string file_path) :
        Generator(file_path) {}
    
    virtual void generate();

};
