#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

typedef struct _EndingStates
{
    int count;
    unsigned STATE_SIZE states[80];
} EndingStates;

typedef struct _Rule
{
    char name[80];
    unsigned STATE_SIZE *table;

    EndingStates ending_states;
    int state_count;
} Rule;

Rule expression_parse(Stream *stream);
void expression_free(Rule expression);

#endif // EXPRESSION_H
