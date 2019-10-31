#include "expression.hpp"
#include "debug.h"
#include <memory.h>
#include <ctype.h>
#include <vector>
using std::vector;

struct Transition
{
    int to;
    char value_min, value_max;
};

#define Node ExpressionTable::Node
typedef ExpressionTable::OperationType OperationType;
typedef vector<Transition> State;

Node *ExpressionTable::parse_sub_expression(Parser &parser, Node *parent)
{
    Node *node;

    // An expression surrounded by parentheses 
    node = parse_expression(parser, parent);
    parser.match(')');

    return node;
}

Node *ExpressionTable::parse_range(Parser &parser, Node *parent)
{
    Node *node = new Node;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->operation = OperationType::NONE;

    char from, to;
    while ((from = parser.next_char()) != ']' && !parser.is_eof())
    {
        to = from;
        if (parser.get_look() == '-')
        {
            parser.match('-');
            to = parser.next_char();
        }

        node->values.push_back(std::make_pair(from, to));
    }

    return node;
}

Node *ExpressionTable::parse_all_value(Parser &parser, Node *parent)
{
    Node *node = new Node;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->operation = OperationType::NONE;
    node->values.push_back(std::make_pair(0, 127));
    return node;
}

Node *ExpressionTable::parse_value(Parser &parser, Node *parent, char c)
{
    // Read a single char as a value
    Node *node = new Node;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->operation = OperationType::NONE;
    node->values.push_back(std::make_pair(c, c));
    return node;
}

Node *ExpressionTable::parse_unary_op(Parser &parser, Node *node)
{
    while (!parser.is_eof())
    {
        OperationType op = OperationType::NONE;
        switch (parser.get_look())
        {
            case '?': op = OperationType::OPTIONAL; break;
            case '+': op = OperationType::ONE_OR_MORE; break;
            case '*': op = OperationType::ANY; break;
        }

        if (op == OperationType::NONE)
            break;
        parser.next_char();

        Node *operation = new Node;
        operation->parent = node->parent;
        operation->left = node;
        operation->right = NULL;
        operation->operation = op;
        node->parent = operation;
        node = operation;
    }

    return node;
}

Node *ExpressionTable::parse_escape(Parser &parser, Node *parent)
{
    char c = parser.next_char();
    if (c == 'n')
        return parse_value(parser, parent, '\n');
    
    return parse_value(parser, parent, c);
}

Node *ExpressionTable::parse_term(Parser &parser, Node *parent)
{
    // Get the next value that's not white space
    parser.skip_white_space();
    char c = parser.next_char();
    
    // Parse that value
    Node *node = NULL;
    switch (c)
    {
        case '(': node = parse_sub_expression(parser, parent); break;
        case '[': node = parse_range(parser, parent); break;
        case '\\': node = parse_escape(parser, parent); break;
        case '.': node = parse_all_value(parser, parent); break;
        default: node = parse_value(parser, parent, c); break;
    }

    return parse_unary_op(parser, node);
}

#define IS_CHAR_VALUE(c) (c != '\n' && c != ')' && c != EOF)
#define IS_CHAR_FACTOR(c) ((c) == '|')

Node *ExpressionTable::parse_operation(Node *left, Node *right, Node *parent, OperationType op)
{
    Node *operation = new Node;
    operation->parent = parent;
    operation->right = right;
    operation->left = left;
    operation->left->parent = operation;
    operation->operation = op;
    return operation;
}

static OperationType decode_factor_op(char c)
{
    switch(c)
    {
        case '|': return OperationType::OR;
        default: return OperationType::NONE;
    }
}

Node *ExpressionTable::parse_factor(Parser &parser, Node *parent)
{
    Node *left = parse_term(parser, parent);
    while (IS_CHAR_FACTOR(parser.get_look()))
    {
        OperationType op = decode_factor_op(parser.next_char());
        Node *right = parse_term(parser, parent);
        left = parse_operation(left, right, parent, op);
    }

    return left;
}

Node *ExpressionTable::parse_expression(Parser &parser, Node *parent)
{
    Node *left = parse_factor(parser, parent);
    while (IS_CHAR_VALUE(parser.get_look()))
    {
        Node *right = parse_factor(parser, parent);
        left = parse_operation(left, right, parent, OperationType::CONCAT);
    }

    return left;
}

// Create and export FSMs
class FSM
{
public:
    FSM(Node *node)
    {
        State start;
        states.push_back(start);
        ending_states = compile_node(node, { 0 });
    }

    // Convert the FSM into a char table
    char *gen_table() const
    {
        // Allocate the new table
        char *table = new char[states.size() * 128];
        memset(table, -1, states.size() * 128);

        // Load values into the table
        int index = 0;
        for (State state : states)
        {
            for (Transition transition : state)
            {
                for (int i = transition.value_min; i <= transition.value_max; i++)
                    table[index * 128 + i] = transition.to;
            }
            index++;
        }

        return table;
    }

    inline vector<int> get_end_states() const { return ending_states; }
    inline int get_size() const { return states.size(); }

private:
    vector<State> states;
    vector<int> ending_states;

    // 
    vector<int> compile_node(Node *node, vector<int> from)
    {
        switch(node->operation)
        {
            case OperationType::CONCAT: return compile_concat(node, from);
            case OperationType::OR: return compile_or(node, from);
            case OperationType::OPTIONAL: return compile_optional(node, from);
            case OperationType::ONE_OR_MORE: return compile_one_or_more(node, from);
            case OperationType::ANY: return compile_any(node, from);
            default: return compile_value(node, from);
        }
    }

    vector<int> compile_value(Node *node, vector<int> from)
    {
        State state;

        for (auto range : node->values)
        {
            Transition transition;
            transition.value_min = range.first;
            transition.value_max = range.second;
            transition.to = states.size();
            for (int i : from)
                states[i].push_back(transition);
        }

        states.push_back(state);
        return { (int)states.size() - 1 };
    }

    vector<int> compile_concat(Node *node, vector<int> from)
    {
        vector<int> state_a = compile_node(node->left, from);
        vector<int> state_b = compile_node(node->right, state_a);
        return state_b;
    }

    vector<int> compile_or(Node *node, vector<int> from)
    {
        vector<int> state_a = compile_node(node->left, from);
        vector<int> state_b = compile_node(node->right, from);
        state_b.insert(state_b.end(), state_a.begin(), state_a.end());
        return state_b;
    }

    vector<int> compile_optional(Node *node, vector<int> from)
    {
        vector<int> option = compile_node(node->left, from);
        option.insert(option.end(), from.begin(), from.end());
        return option;
    }

    vector<int> compile_one_or_more(Node *node, vector<int> from)
    {
        vector<int> ends = compile_node(node->left, from);
        for (Transition t : states[from[0]])
        {
            for (int end : ends)
                states[end].push_back(t);
        }

        return ends;
    }

    vector<int> compile_any(Node *node, vector<int> from)
    {
        vector<int> ends = compile_one_or_more(node, from);
        ends.insert(ends.end(), from.begin(), from.end());
        return ends;
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
    ending_states = fsm.get_end_states();
    state_count = fsm.get_size();
    LOG("   => Generated tables\n");
}

ExpressionTable::~ExpressionTable()
{
    delete data;
}
