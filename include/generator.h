#ifndef GENORATOR_H
#define GENORATOR_H

#include "lexer.h"

void write_file(FILE *output, const char *file_path);
char *to_upper(char *str);
char *to_lower(char *str);

void generate_cpp(FILE *output, Lexer *lex);
void generate_c(FILE *output, Lexer *lex);

#endif // GENORATOR_H
