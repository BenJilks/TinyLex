#include "parser.h"
#include "debug.h"
#include <stdlib.h>
#include <ctype.h>

#define RED     "\033[1;31m"
#define ORANGE  "\033[01;33m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"

#define ERROR(stream, pattern, ...) \
{ \
    int i; \
     \
    printf(RED "Error(%i:%i): " pattern "\n", \
        stream->line_no, stream->column_no, \
        __VA_ARGS__); \
     \
    printf(WHITE "\t%s\t", stream->line_buffer); \
    for (i = 0; i < stream->column_no - 1; i++) \
        printf(" "); \
    printf(ORANGE "~\n" RESET); \
}

// Loads the next line in a stream into it's line buffer
static void load_next_line(
    Stream *stream)
{
    size_t read = 0;
    int len;

    // The getline function will reallocate the memory 
    // based on how much is needed
    len = getline(&stream->line_buffer, 
        &read, stream->file);

    // If there's no more lines left, then set eof flag
    if (len == -1)
        stream->eof_flag = 1;

    stream->line_length = len;
    stream->line_no += 1;
    stream->column_no = 0;
}

// Load the next char in the stream into look
static void load_next_char(
    Stream *stream)
{
    // If the end of the line is reached, load the next line
    if (stream->column_no >= stream->line_length)
        load_next_line(stream);
    
    if (!stream->eof_flag)
    {
        stream->look = stream->line_buffer[stream->column_no];
        stream->column_no += 1;
    }
}

// Read file into buffer until a white space 
// char is reached
void parser_next_word(
    Stream *stream, 
    char *buffer)
{
    int buffer_pointer;

    buffer_pointer = 0;
    while (!isspace(stream->look) && !stream->eof_flag)
    {
        buffer[buffer_pointer] = parser_next_char(stream);
        buffer_pointer += 1;
    }

    buffer[buffer_pointer] = '\0';
}

// Opens a new file stream for parsing
Stream parser_open_stream(
    const char *file_path)
{
    Stream stream;
    stream.file = fopen(file_path, "r");
    if (!stream.file)
    {
        printf(RED "Error: Could not open file '%s'\n" RESET, 
            file_path);

        stream.eof_flag = 1;
        return stream;
    }

    stream.line_no = 0;
    stream.column_no = 0;
    stream.eof_flag = 0;
    stream.error_flag = 0;
    LOG("Opened new file stream '%s'\n", file_path);

    // Prime the stream with the first line
    stream.line_buffer = NULL;
    stream.line_length = -1;
    load_next_char(&stream);
    return stream;
}

void parser_close_stream(
    Stream *stream)
{
    if (stream->line_buffer)
        free(stream->line_buffer);
    fclose(stream->file);
}

// Verify that the loaded char is correct 
// and load the next one
void parser_match(
    Stream *stream, 
    char c)
{
    if (stream->look != c)
    {
        ERROR(stream, "Expected '%c', got '%c' instead",
            c, stream->look);
    }

    load_next_char(stream);
}

// Load the next char that's not whitespace
void parser_skip_white_space(
    Stream *stream)
{
    while (isspace(stream->look) && !stream->eof_flag)
        load_next_char(stream);
}

// Get the currently loaded char and load the next one
char parser_next_char(
    Stream *stream)
{
    char temp;

    temp = stream->look;
    load_next_char(stream);
    return temp;
}
