


#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "time.h"

// Place this macro at the end of a test suite's main method.
#define SUITE_END(start) do {\
    clock_t end = clock();\
    double duration = ((double)end - start)/CLOCKS_PER_SEC;\
    printf("%d in %.2f ms ", completed_tests, duration * 1000);\
    return 0;\
} while(0);


unsigned int completed_tests = 0;

// Place this macro at the beginning of each test suite method.
#define TEST_PASSED do {\
    printf("OK: %s\n", __func__);\
    completed_tests++;\
    return;\
} while (0);

#endif

