#include <stdio.h>
#include <stdlib.h>

static const char *template_dir;

void compile_template(
    const char *file_name, 
    const char *out_name,
    const char *name)
{
    char in_path[128];
    FILE *in, *out;
    size_t len, read;
    char *line;
    int i;

    line = NULL;
    sprintf(in_path, "%s/%s", template_dir, file_name);
    printf("Generating template '%s' => '%s'\n", file_name, out_name);
    in = fopen(in_path, "rb");
    out = fopen(out_name, "wb");

    fprintf(out, "const char *template_%s = \n", name);
    while ((read = getline(&line, &len, in)) != -1)
    {
        fputs("\t\"", out);
        for (i = 0; i < read - 1; i++)
        {
            if (line[i] == '"')
                fputs("\\\"", out);
	    else if (line[i] == '\\')
		fputs("\\\\", out);
            else
                fputc(line[i], out);
        }
        fputs("\\n\"\n", out);
    }
    fprintf(out, ";\n");
    
    fclose(in);
    fclose(out);
    free(line);
}

int main(int argc, char *argv[])
{
    template_dir = argv[1];

    compile_template("c/c_header.txt", "c_header.h", "c_header");
    compile_template("c/c_implement.txt", "c_implement.h", "c_implement");
    compile_template("cpp/cpp_header.txt", "cpp_header.hpp", "cpp_header");
    compile_template("cpp/cpp_implement.txt", "cpp_implement.hpp", "cpp_implement");
    compile_template("cpp/cpp_tinylib.txt", "cpp_tinylib.hpp", "cpp_tinylib");
 
    return 0;
}
