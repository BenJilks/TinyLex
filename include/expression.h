#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

typedef struct _EndingStates
{
    int count;
    int states[80];
} EndingStates;

typedef struct _ExpressionTable
{
    char name[80];
    char *table;

    EndingStates ending_states;
    int state_count;
} ExpressionTable;

ExpressionTable expression_parse(Stream *stream);
void expression_free(ExpressionTable expression);

#endif // EXPRESSION_H
