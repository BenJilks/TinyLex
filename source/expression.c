#include "expression.h"
#include "debug.h"
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#define IS_CHAR_VALUE(c) (c != '\n' && c != ')' && c != EOF)
#define IS_CHAR_FACTOR(c) ((c) == '|')

#define STATE_BUFFER_SIZE 80

typedef enum _OperationType
{
    OPERATION_NONE,
    OPERATION_CONCAT,
    OPERATION_OR,
    OPERATION_OPTIONAL,
    OPERATION_ONE_OR_MORE,
    OPERATION_ANY
} OperationType;

typedef struct _NodeValue
{
    char from;
    char to;
} NodeValue;

typedef struct _Node
{
    struct _Node *left, *right;
    OperationType operation;

    NodeValue values[80];
    int value_count;
} Node;

static Node *parse_expression_node(Stream *stream);

static Node *parse_sub_expression(
    Stream *stream)
{
    Node *node;

    // An expression surrounded by parentheses 
    parser_match(stream, '(');
    node = parse_expression_node(stream);
    parser_match(stream, ')');

    return node;
}

// Parse a list of value ranges surrounded by '[' ']'
static Node *parse_range(
    Stream *stream)
{
    char from, to;
    Node *node;

    // Create a new node with no operation
    node = malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->operation = OPERATION_NONE;
    node->value_count = 0;

    // While the ending bracket has not been reached, read a new range
    parser_match(stream, '[');
    while (stream->look != ']' && !stream->eof_flag)
    {
        // Parse single char range
        to = parser_next_char(stream);
        from = to;

        // If the next char is '-', then parse a range
        if (stream->look == '-')
        {
            parser_match(stream, '-');
            to = parser_next_char(stream);
        }

        // Add the range to the nodes value list
        node->values[node->value_count] = (NodeValue) { from, to };
        node->value_count += 1;
    }
    parser_match(stream, ']');

    return node;
}

static Node *parse_all_value(
    Stream *stream)
{
    Node *node;

    node = malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->operation = OPERATION_NONE;

    parser_match(stream, '.');
    node->value_count = 1;
    node->values[0] = (NodeValue) {0, 127};
    return node;
}

static Node *make_value(
    Stream *stream,
    char from,
    char to)
{
    Node *node;

    node = malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->operation = OPERATION_NONE;

    // Read a single char as a value
    node->value_count = 1;
    node->values[0] = (NodeValue) {from, to};
    return node;
}

static Node *parse_value(
    Stream *stream)
{
    char c;

    c = parser_next_char(stream);
    return make_value(stream, c, c);
}

static Node *parse_escape(
    Stream *stream)
{
    char c;

    parser_match(stream, '\\');
    c = parser_next_char(stream);
    switch(c)
    {
        case 'n': return make_value(stream, '\n', '\n');
        case 'd': return make_value(stream, '0', '9');
//        case 's': return make_value(stream, '\n', ' ');
        case 'w': return make_value(stream, 'A', 'z');
        default: return make_value(stream, c, c);
    }
}

// Parse single argument operations tailing a value
static Node *parse_unary_op(
    Stream *stream, 
    Node *node)
{
    Node *operation;
    OperationType op_type;

    while (!stream->eof_flag)
    {
        // Find the next operation type
        op_type = OPERATION_NONE;
        switch (stream->look)
        {
            case '?': op_type = OPERATION_OPTIONAL; break;
            case '+': op_type = OPERATION_ONE_OR_MORE; break;
            case '*': op_type = OPERATION_ANY; break;
        }

        // If there's no operation next, exit the loop
        if (op_type == OPERATION_NONE)
            break;

        // Create a new operation node
        operation = malloc(sizeof(Node));
        operation->left = node;
        operation->right = NULL;
        operation->operation = op_type;
        node = operation;
        parser_next_char(stream);
    }

    return node;
}

// Parse a single value from an expression
static Node *parse_term(
    Stream *stream)
{
    Node *node = NULL;

    switch (stream->look)
    {
        case '(': node = parse_sub_expression(stream); break;
        case '[': node = parse_range(stream); break;
        case '\\': node = parse_escape(stream); break;
        case '.': node = parse_all_value(stream); break;
        default: node = parse_value(stream); break;
    }

    return parse_unary_op(stream, node);
}

static Node *parse_operation(
    Node *left, 
    Node *right, 
    OperationType op)
{
    Node *operation;

    operation = malloc(sizeof(Node));
    operation->right = right;
    operation->left = left;
    operation->operation = op;
    return operation;
}

static Node *parse_factor(
    Stream *stream)
{
    Node *left, *right;

    left = parse_term(stream);
    while (IS_CHAR_FACTOR(stream->look))
    {
        OperationType op = OPERATION_NONE;
        switch(parser_next_char(stream))
        {
            case '|': op = OPERATION_OR; break;
        }

        right = parse_term(stream);
        left = parse_operation(left, right, op);
    }

    return left;
}

static Node *parse_expression_node(
    Stream *stream)
{
    Node *left, *right;

    left = parse_factor(stream);
    while (IS_CHAR_VALUE(stream->look))
    {
        right = parse_factor(stream);
        left = parse_operation(left, right, OPERATION_CONCAT);
    }

    return left;
}

static void free_node(
    Node *node)
{
    if (node->left) free_node(node->left);
    if (node->right) free_node(node->right);
    free(node);
}

static EndingStates compile_node(Rule *table, 
    Node *node, EndingStates from);

static void copy_state(
    Rule *rule,
    int start,
    int to,
    int len)
{
    int i;

    for (i = 0; i < len; i++)
        rule->table[start + i] = to;
}

static EndingStates compile_value(
    Rule *rule,
    Node *node, 
    EndingStates from_states)
{
    NodeValue value;
    int i, j, to;

    // Create the new state
    to = rule->state_count;
    rule->state_count += 1;
    rule->table = realloc(rule->table, rule->state_count * sizeof(STATE_SIZE) * CHAR_COUNT);
    memset(rule->table + (rule->state_count - 1) * CHAR_COUNT, -1, sizeof(STATE_SIZE) * CHAR_COUNT);

    // Create transitions to the new state
    for (i = 0; i < node->value_count; i++)
    {
        value = node->values[i];
        for (j = 0; j < from_states.count; j++)
        {
            int start, len, from;

            from = from_states.states[j];
            start = from * CHAR_COUNT + value.from;
            len = value.to - value.from + 1;
            copy_state(rule, start, to, len);
//            memset(rule->table + start, to, sizeof(STATE_SIZE) * len);
        }
    }

    return (EndingStates) { 1, to };
}

static EndingStates compile_concat(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    EndingStates state_a, state_b;

    state_a = compile_node(rule, node->left, from);
    state_b = compile_node(rule, node->right, state_a);
    return state_b;
}

static EndingStates combine_states(EndingStates a, EndingStates b)
{
    EndingStates ab;

    ab.count = a.count + b.count;
    memcpy(ab.states, a.states, a.count * sizeof(int));
    memcpy(ab.states + a.count, b.states, b.count * sizeof(int));
    return ab;
}

static EndingStates compile_or(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    EndingStates state_a, state_b;

    state_a = compile_node(rule, node->left, from);
    state_b = compile_node(rule, node->right, from);
    return combine_states(state_a, state_b);
}

static EndingStates compile_optional(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    EndingStates option;

    option = compile_node(rule, node->left, from);
    return combine_states(from, option);
}

static EndingStates compile_any(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    EndingStates ends;
    int i, j;

    ends = compile_node(rule, node->left, from);
    for (i = 0; i < CHAR_COUNT; i++)
    {
        char to = rule->table[from.states[0] * CHAR_COUNT + i];
        if (to != -1)
        {
            for (j = 0; j < ends.count; j++)
                rule->table[ends.states[j] * CHAR_COUNT + i] = from.states[0];
        }
    }

    return combine_states(from, ends);
}

static EndingStates compile_one_or_more(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    EndingStates ends;

    ends = compile_node(rule, node->left, from);
    ends = compile_any(rule, node, ends);
    return ends;
}

static EndingStates compile_node(
    Rule *rule,
    Node *node, 
    EndingStates from)
{
    switch(node->operation)
    {
        case OPERATION_CONCAT: return compile_concat(rule, node, from);
        case OPERATION_OR: return compile_or(rule, node, from);
        case OPERATION_OPTIONAL: return compile_optional(rule, node, from);
        case OPERATION_ONE_OR_MORE: return compile_one_or_more(rule, node, from);
        case OPERATION_ANY: return compile_any(rule, node, from);
        default: return compile_value(rule, node, from);
    }
}

static void compile_expression_tree(
    Rule *rule,
    Node *root)
{
    rule->table = malloc(sizeof(STATE_SIZE) * CHAR_COUNT);
    rule->state_count += 1;
    memset(rule->table, -1, sizeof(STATE_SIZE) * CHAR_COUNT);

    rule->ending_states = compile_node(rule, 
        root, (EndingStates) { 1, 0 });
}

Rule expression_parse(
    Stream *stream)
{
    Rule rule;
    rule.ending_states.count = 0;
    rule.state_count = 0;

    Node *root = parse_expression_node(stream);
    compile_expression_tree(&rule, root);
    free_node(root);
    return rule;
}

void expression_free(
    Rule rule)
{
    free(rule.table);
}
