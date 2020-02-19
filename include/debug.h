#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG           0
#define DEBUG_TABLE     0

#if DEBUG

#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__)

#else

#define LOG(...) ;

#endif

#endif // DEBUG_H
