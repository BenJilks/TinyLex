#include "expression.hpp"
#include "debug.h"
#include <memory.h>
#include <ctype.h>
#include <vector>
using std::vector;

struct Transition
{
    int from, to;
    char value;
};

typedef ExpressionTable::Node Node;
typedef ExpressionTable::OperationType OperationType;
typedef vector<Transition> State;

Node *ExpressionTable::parse_sub_expression(Parser &parser, Node *parent)
{
    Node *node;

    parser.next_char(); // (
    node = parse_expression(parser, parent);
    parser.next_char(); // )

    return node;
}

Node *ExpressionTable::parse_value(Parser &parser, Node *parent, char c)
{
    Node *node = new Node;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->operation = OperationType::NONE;
    node->value = c;
    return node;
}

Node *ExpressionTable::parse_term(Parser &parser, Node *parent)
{
    parser.skip_white_space();
    char c = parser.next_char();
    
    switch (c)
    {
        case '(': return parse_sub_expression(parser, parent);
        default: return parse_value(parser, parent, c);
    }

    return NULL;
}

static int is_char_value(char c)
{
    return isalnum(c);
}

Node *ExpressionTable::parse_expression(Parser &parser, Node *parent)
{
    Node *left = parse_term(parser, parent);
    while (is_char_value(parser.get_look()))
    {
        Node *operation = new Node;
        operation->parent = parent;
        operation->right = parse_term(parser, operation);
        operation->left = left;
        operation->left->parent = operation;
        operation->operation = OperationType::CONCAT;
        left = operation;
    }

    return left;
}

class FSM
{
public:
    FSM(Node *node)
    {
        State start;
        states.push_back(start);
        ending_state = compile_node(node, 0);
    }

    char *gen_table() const
    {
        char *table = new char[states.size() * 128];
        memset(table, -1, states.size() * 128);

        int index = 0;
        for (State state : states)
        {
            for (Transition transition : state)
                table[index * 128 + transition.value] = transition.to;
            index++;
        }

        return table;
    }

    inline int get_end_state() const { return ending_state; }

private:
    vector<State> states;
    int ending_state;

    int compile_node(Node *node, int from)
    {
        switch(node->operation)
        {
            case OperationType::CONCAT: return compile_concat(node, from);
            default: return compile_value(node, from);
        }
    }

    int compile_value(Node *node, int from)
    {
        State state;
        Transition transition;

        transition.value = node->value;
        transition.to = states.size();
        state.push_back(transition);
        printf("%c\n", node->value);

        states.push_back(state);
        return transition.to;
    }

    int compile_concat(Node *node, int from)
    {
        int state_a = compile_node(node->left, from);
        int state_b = compile_node(node->right, state_a);
        return state_b;
    }
};

ExpressionTable::ExpressionTable(Parser &parser)
{
    LOG("Parsing expression\n");
    Node *exp = parse_expression(parser, NULL);
    LOG("   => Parsed\n");
    FSM fsm(exp);
    LOG("   => Generated FSM\n");

    data = fsm.gen_table();
    ending_state = fsm.get_end_state();
    LOG("   => Generated tables\n");
}

ExpressionTable::~ExpressionTable()
{
    delete data;
}
