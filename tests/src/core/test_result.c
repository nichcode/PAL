
#include <stdio.h>
#include <stdint.h>

typedef int Status;
typedef uint32_t Result;

// TODO: remove after testing formatter script for gdb and lldg
#define STATUS_GOOD 0
#define STATUS_BAD 1
#define STATUS_NORMAL 2

#define RESULT_SUCCESS 0
#define RESULT_FAILED_IO (1 << 0)
#define RESULT_FAILED_HANDLE (1 << 1)
#define UNKNOWN1 (1 << 2)
#define UNKNOWN2 (1 << 3)

// The formatter should show the value like an enum not the literal
int main(void)
{
    Status good =  STATUS_GOOD;
    Status bad =  STATUS_BAD;
    Status normal =  STATUS_NORMAL;

    Result result1 = RESULT_SUCCESS;
    Result result2 = RESULT_FAILED_IO | RESULT_FAILED_HANDLE | UNKNOWN1 | UNKNOWN2;

    printf("%u\n", good);
    printf("%u\n", bad);
    printf("%u\n", normal);

    printf("%u\n", result1);
    printf("%u\n", result2);
    (void)fflush(stdout);

    return 0;
}