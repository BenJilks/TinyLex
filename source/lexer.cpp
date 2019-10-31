#include "lexer.hpp"

Lexer::Lexer(Generator &generator) :
    project_name(""), generator(generator) {}

void Lexer::parse_statement(Parser &parser, Generator &generator)
{
    parser.skip_white_space();

    if (!parser.is_eof())
    {
        string name = parser.next_word();
        if (name == "project")
        {
            if (project_name != "")
                parser.error("Project name already defined as '" + project_name + "'");

            parser.skip_white_space();
            project_name = parser.next_word();
        }
        else
        {
            ExpressionTable *expression = new ExpressionTable(parser);
            generator.add_expression(name, expression);
        }
    }
}

void Lexer::parse(Parser &parser)
{    
    while (!parser.is_eof())
        parse_statement(parser, generator);
}
