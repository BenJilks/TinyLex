#include "lexer.hpp"

void Lexer::parse_statement(Parser &parser, Generator &generator)
{
    parser.skip_white_space();
    string name = parser.next_word();
    ExpressionTable *expression = new ExpressionTable(parser);
    generator.add_expression(name, expression);
}

Lexer::Lexer(Parser &parser, Generator &generator)
{
    parse_statement(parser, generator);
}
