#include "generator.h"
#include <unistd.h>
#include <stdlib.h>

static const char *search_paths[] =
{
    "",
    "/include/",
    "/usr/include/",
    "/usr/local/include/",
};

void write_file(
    FILE *output, 
    const char *file_path)
{
    char file[80];
    char path[80];
    int found, len;

    // Find template file path
    sprintf(file, "tinylex/%s", file_path);
    path[0] = '\0';

    found = 0;
    for (int i = 0; i < sizeof(search_paths) / sizeof(char *); i++)
    {
        sprintf(path, "%s%s", search_paths[i], file);
        if (access(path, F_OK) != -1)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Error: Could not find template file '%s'", 
            file_path);
        return;
    }

    // Open file and find its length
    FILE *in = fopen(path, "rb");
    fseek(in, 0L, SEEK_END);
    len = ftell(in);
    rewind(in);

    // Read data to buffer
    char *data = malloc(len);
    fread(data, sizeof(char), len, in);
    fclose(in);

    // Write it to the out file
    fwrite(data, sizeof(char), len, output);
    free(data);
}
