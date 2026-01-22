#include <rodos_scoped.h>


class HelloLinux : public RODOS::StaticThread<> {
    void run() {
        RODOS::PRINTF("Hello Linux!\n");
        RODOS::hwResetAndReboot();
    }
} helloLinux;
