#include <rodos_scoped.h>


constexpr auto pa9     = RODOS::GPIO_009;
constexpr auto pa10    = RODOS::GPIO_010;
auto           ledPin1 = RODOS::HAL_GPIO(pa9);
auto           ledPin2 = RODOS::HAL_GPIO(pa10);


class HelloVa41620 : public RODOS::StaticThread<> {
    void run() {
        ledPin1.init(/*isOutput=*/true, /*numOfPins=*/1, /*initVal=*/0);
        ledPin2.init(/*isOutput=*/true, /*numOfPins=*/1, /*initVal=*/0);
        ledPin1.setPins(1); // Bottom LED
        ledPin2.setPins(1); // Top LED
        RODOS::AT(RODOS::NOW() + 1 * RODOS::SECONDS);

        static constexpr auto toggleInterval = 250 * RODOS::MILLISECONDS;
        auto                  led1IsOn       = true;
        TIME_LOOP(0, toggleInterval) {
            if(led1IsOn) {
                ledPin1.setPins(0); // Bottom LED
                ledPin2.setPins(1); // Top LED
                led1IsOn = false;
            } else {
                ledPin1.setPins(1); // Bottom LED
                ledPin2.setPins(0); // Top LED
                led1IsOn = true;
            }
        }
    }
} helloVa41620;
