#include "lexer.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 80

static void parse_expression_rule(
    Lexer *lex, 
    Stream *stream, 
    char *name)
{
    Rule rule;

    // Parse and set table name
    LOG("   => Parsing rule '%s'\n", name);
    rule = expression_parse(stream);
    strcpy(rule.name, name);

    // Add rule to lexer
    lex->rules[lex->rule_count] = rule;
    lex->rule_count += 1;

    // If the buffer has been filled, expand it
    if (lex->rule_count >= lex->rule_buffer_size)
    {
        lex->rule_buffer_size += BUFFER_SIZE;
        lex->rules = realloc(lex->rules, 
            sizeof(Rule) * lex->rule_buffer_size);
    }

    parser_skip_white_space(stream);
}

Lexer lexer_create()
{
    Lexer lex;
    lex.rules = malloc(sizeof(Rule) * BUFFER_SIZE);
    lex.rule_count = 0;
    lex.rule_buffer_size = BUFFER_SIZE;

    lex.table = NULL;
    lex.table_size = -1;
    LOG("Created new lexer\n");
    return lex;
}

void lexer_parse(
    Stream *stream,
    Lexer *lex)
{
    char name[80];
    LOG("Started parsing\n");

    parser_skip_white_space(stream);
    while (!stream->eof_flag)
    {
        // Fetch the first word of the statement
        parser_next_word(stream, name);
        parser_skip_white_space(stream);

        // If it's a keyword, do the appropriate action, 
        // otherwise parse an expression rule
        if (!strcmp(name, "project"))
        {
            parser_next_word(stream, lex->project_name);
            parser_skip_white_space(stream);
        }
        else
            parse_expression_rule(lex, stream, name);
    }
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

static void allocate_link_table(
    Lexer *lex)
{
    int i, j;
    Rule *rule;

    // Find table size
    lex->table_size = 0;
    for (i = 0; i < lex->rule_count; i++)
    {
        rule = &lex->rules[i];
        lex->table_size += rule->state_count;
    }

    // Allocate table data
    lex->table = malloc(CHAR_COUNT * sizeof(STATE_SIZE) * lex->table_size);
    lex->end_states = malloc(lex->table_size);
    memset(lex->table, -1, CHAR_COUNT * sizeof(STATE_SIZE) * lex->table_size);
    memset(lex->end_states, -1, lex->table_size);
}

static void mark_end_state(
    Lexer *lex,
    Rule *rule,
    int state,
    int rule_id)
{
    if (lex->end_states[state] != -1)
    {
        printf("Linker Warning: The tokens '%s' and '%s' "
            "share an ending state (of %i). "
            "The second will always be chosen.\n",
            rule->name, lex->rules[lex->end_states[state]].name, state);
        
        return;
    }

    lex->end_states[state] = rule_id;
}

static void follow_clash(
    Lexer *lex,
    Rule *rule,
    int rule_state,
    int global_state,
    int *state_pointer,
    char *states_processed,
    int rule_id)
{
    unsigned STATE_SIZE curr_transition;
    unsigned STATE_SIZE new_transition;
    int global_index;
    int i;

    // If the state as already been processed, don't 
    // do it again. This avoids loops as well
    if (states_processed[global_state])
        return;
    states_processed[global_state] = 1;

    for (i = 0; i < CHAR_COUNT; i++)
    {
        // For each transition, get the information for both 
        // what's there and what we want to add 
        global_index = global_state * CHAR_COUNT + i;
        curr_transition = lex->table[global_index];
        new_transition = rule->table[rule_state * CHAR_COUNT + i];

        // If we don't want to add a new transition, then skip this one
        if (new_transition == NONE)
            continue;

        if (curr_transition == NONE)
        {
            // If there's no clash, point into the rest of the token
            lex->table[global_index] = (*state_pointer) + new_transition;
        }
        else
        {
            // Otherwise, resolve the next clash
            follow_clash(lex, rule, 
                new_transition, curr_transition,
                state_pointer, states_processed, rule_id);
        }
    }

    // If this is an end state, mark it as so
    for (i = 0; i < rule->ending_states.count; i++)
    {
        unsigned STATE_SIZE end_state;

        end_state = rule->ending_states.states[i];
        if (end_state == rule_state)
            mark_end_state(lex, rule, global_state, rule_id);
    }
}

static void link_rule(
    Lexer *lex, 
    Rule *rule,
    int *state_pointer,
    char *states_processed,
    int index)
{
    int i, j;
    int start_state;

    // Copy all transitions into the global table
    follow_clash(lex, rule, 0, 0, state_pointer, states_processed, index);
    start_state = *state_pointer;
    for (i = 0; i < rule->state_count; i++)
    {
        for (j = 0; j < CHAR_COUNT; j++)
        {
            unsigned STATE_SIZE transition;

            transition = rule->table[i * CHAR_COUNT + j];
            if (transition != NONE)
                transition += start_state;
 
            lex->table[(*state_pointer) * CHAR_COUNT + j] = 
                transition;
        }
        
        *state_pointer += 1;
    }

    // Mark endings states
    for (i = 0; i < rule->ending_states.count; i++)
    {
        mark_end_state(lex, rule, 
            start_state + rule->ending_states.states[i], 
            index);
    }
}

void lexer_link(
    Lexer *lex)
{
    int i, state_pointer;
    char *states_processed;
    Rule *rule;

    // Link tables together
    allocate_link_table(lex);
    states_processed = malloc(lex->table_size);
    state_pointer = 0;
    for (i = 0; i < lex->rule_count; i++)
    {
        rule = &lex->rules[i];
        memset(states_processed, 0, lex->table_size);
        link_rule(lex, rule, &state_pointer, 
            states_processed, i);
    }

    free(states_processed);

#if DEBUG_TABLE
    for (i = 0; i < lex->table_size; i++)
    {
        printf("%i) ", i);
        for (int j = 0; j < CHAR_COUNT; j++)
        {
            if (lex->table[i * CHAR_COUNT + j] != NONE)
                printf("%c -> %i, ", j, lex->table[i * CHAR_COUNT + j]);
        }

        if (lex->end_states[i] != -1)
        {
            printf("%s end", lex->rules[lex->end_states[i]].name);
        }
        printf("\n");
    }

    printf("Table size: %lu KB\n", 
        sizeof(STATE_SIZE) * CHAR_COUNT * lex->table_size / 1024);
#endif
}

void lexer_free(
    Lexer lexer)
{
    free(lexer.rules);
    if (lexer.table != NULL)
        free(lexer.table);
}
