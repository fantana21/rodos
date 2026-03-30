#include "rodos.h"

void MAIN() {
	int64_t x;
	x = 99_ns;      xprintf(" 99 ns = %lld\n", (long long)x);
	x = 99_us;      xprintf(" 99 us = %lld\n", (long long)x);
	x = 99_ms;      xprintf(" 99 ms = %lld\n", (long long)x);
	x = 99_s;       xprintf(" 99 s = %lld\n", (long long)x);
	x = 99_mins;    xprintf(" 99 mins = %lld\n", (long long)x);
	x = 99_h;       xprintf(" 99 h = %lld\n", (long long)x);
	x = 99_days;    xprintf(" 99 days = %lld\n", (long long)x);
	x = 99_weeks;   xprintf(" 99 weeks = %lld\n", (long long)x);
}

