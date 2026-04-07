#include "rodos-debug.h"
#include "hw_specific.h"
#include "yprintf.h"


extern "C" int _write(int file, char * ptr, int len);


namespace RODOS {
int putchar(int ic) {
    char c = static_cast<char>(ic);
    if (c == '\n') {
        putchar('\r');
    }
    _write(1, &c, 1);
    return c;
}

int puts(const char * str) {
    while (*str) {
        putchar(*str++);
    }
    return 0;
}

void abort(void) { while(1); }

void sp_partition_yield() {}

void FFLUSH() {}
}
