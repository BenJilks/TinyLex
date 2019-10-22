
typedef struct Token
{
    char data[80];
    int token_type;
} Token;

void tinylex_open(const char *file_path);
Token tinylex_next();
void tinylex_close();
