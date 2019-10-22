#define TINYLEX_IMPLEMENATION
#include "tinylex.h"

int main()
{
    tinylex_open("test.txt");
    tinylex_next();
    tinylex_close();
    return 0;
}
