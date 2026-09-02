
#include <stdio.h>

typedef int Status;

// TODO: remove after testing formatter script for gdb and lldg
#define STATUS_GOOD 0
#define STATUS_BAD 1
#define STATUS_NORMAL 2

// The formatter should show the value like an enum not the literal
int main(void)
{
    Status good =  STATUS_GOOD;
    Status bad =  STATUS_BAD;
    Status normal =  STATUS_NORMAL;

    printf("%u\n", good);
    printf("%u\n", bad);
    printf("%u\n", normal);
    (void)fflush(stdout);

    return 0;
}