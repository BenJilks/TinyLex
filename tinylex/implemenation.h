#include <stdio.h>

static FILE *in;

void tinylex_open(const char *file_path)
{
    in = fopen(file_path, "r");
}

void tinylex_close()
{
    fclose(in);
}

Token tinylex_next()
{
    Token token;

    int buffer_pointer = 0;
    int state = 0;
    int table = 0;
    for (;;)
    {
        char c = fgetc(in);
        printf("S%i => %c => ", state, c);
        state = TEST[state * 128 + c];
        printf("S%i\n", state);

        if (state == -1 || state == TEST_ENDING_STATE)
            break;
    }

    if (state == TEST_ENDING_STATE)
        printf("Match!\n");

    printf("%i\n", state);
    return token;
}
