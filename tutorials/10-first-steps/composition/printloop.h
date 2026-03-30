#include "rodos.h"

class PrintLoop : public StaticThread<> {

public:
    PrintLoop(const char* name) : StaticThread<>(name) {}

    void run() {
        TIME_LOOP(1 * SECONDS, 2 * SECONDS) {
            PRINTF("time: %3.9f, I am %s \n", SECONDS_NOW(), this->name );
        }
    }

};
