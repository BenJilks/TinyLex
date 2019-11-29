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

static Node *parse_value(
    Stream *stream)
{
    Node *node;
    char c;

    node = malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->operation = OPERATION_NONE;

    // Read a single char as a value
    c = parser_next_char(stream);
    node->value_count = 1;
    node->values[0] = (NodeValue) {c, c};
    return node;
}

static Node *parse_escape(
    Stream *stream)
{
    parser_match(stream, '\\');
    return parse_value(stream);
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

static EndingStates compile_node(ExpressionTable *table, 
    Node *node, EndingStates from);

static EndingStates compile_value(
    ExpressionTable *table,
    Node *node, 
    EndingStates from_states)
{
    NodeValue value;
    int i, j, to;

    // Create the new state
    to = table->state_count;
    table->state_count += 1;
    table->table = (char*)realloc(table->table, table->state_count * 128);
    memset(table->table + (table->state_count - 1) * 128, -1, 128);

    // Create transitions to the new state
    for (i = 0; i < node->value_count; i++)
    {
        value = node->values[i];
        for (j = 0; j < from_states.count; j++)
        {
            int start, len, from;

            from = from_states.states[j];
            start = from * 128 + value.from;
            len = value.to -  value.from;
            memset(table->table + start, to, len);
        }
    }

    return (EndingStates) { 1, to };
}

static EndingStates compile_concat(
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    EndingStates state_a, state_b;

    state_a = compile_node(table, node->left, from);
    state_b = compile_node(table, node->right, state_a);
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
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    EndingStates state_a, state_b;

    state_a = compile_node(table, node->left, from);
    state_b = compile_node(table, node->right, from);
    return combine_states(state_a, state_b);
}

static EndingStates compile_optional(
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    EndingStates option;

    option = compile_node(table, node->left, from);
    return combine_states(from, option);
}

static EndingStates compile_one_or_more(
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    EndingStates ends;
    int i, j;

    ends = compile_node(table, node->left, from);
    for (i = 0; i < 128; i++)
    {
        char to = table->table[from.states[0] * 128 + i];
        if (to != -1)
        {
            for (j = 0; j < ends.count; j++)
                table->table[ends.states[j] * 128 + i] = from.states[0];
        }
    }

    return ends;
}

static EndingStates compile_any(
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    EndingStates ends;

    ends = compile_one_or_more(table, node, from);
    return combine_states(from, ends);
}

static EndingStates compile_node(
    ExpressionTable *table,
    Node *node, 
    EndingStates from)
{
    switch(node->operation)
    {
        case OPERATION_CONCAT: return compile_concat(table, node, from);
        case OPERATION_OR: return compile_or(table, node, from);
        case OPERATION_OPTIONAL: return compile_optional(table, node, from);
        case OPERATION_ONE_OR_MORE: return compile_one_or_more(table, node, from);
        case OPERATION_ANY: return compile_any(table, node, from);
        default: return compile_value(table, node, from);
    }
}

static void compile_expression_tree(
    ExpressionTable *table,
    Node *root)
{
    table->table = (char*)malloc(128);
    memset(table->table, -1, 128);

    table->ending_states = compile_node(table, 
        root, (EndingStates) { 1, 0 });
}

ExpressionTable expression_parse(
    Stream *stream)
{
    ExpressionTable expression;
    expression.ending_states.count = 0;
    expression.state_count = 0;

    Node *root = parse_expression_node(stream);
    compile_expression_tree(&expression, root);
    free_node(root);
    return expression;
}

void expression_free(
    ExpressionTable expression)
{
    free(expression.table);
}
