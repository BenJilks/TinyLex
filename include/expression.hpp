#pragma once
#include "parser.hpp"

class ExpressionTable
{
public:
    ExpressionTable(Parser &parser);
    ~ExpressionTable();

    enum class OperationType
    {
        NONE,
        CONCAT
    };

    struct Node
    {
        Node *parent;
        Node *left, *right;
        OperationType operation;
        char value;
    };

protected:
    int state_count;
    int ending_state;
    char *data;

private:
    Node *parse_expression(Parser &parser, Node *parent);
    Node *parse_sub_expression(Parser &parser, Node *parent);
    Node *parse_value(Parser &parser, Node *parent, char c);
    Node *parse_term(Parser &parser, Node *parent);

};
