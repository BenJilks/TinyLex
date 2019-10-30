#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG

#ifdef DEBUG

#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__)

#else

#define LOG(...) ;

#endif

#endif // DEBUG_H
