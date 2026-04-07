#include <rodos_scoped.h>


constexpr auto pa9            = RODOS::GPIO_009;
constexpr auto pa10           = RODOS::GPIO_010;
constexpr auto pf4            = RODOS::GPIO_084;
constexpr auto pf5            = RODOS::GPIO_085;
auto           ledPin1        = RODOS::HAL_GPIO(pa9);
auto           ledPin2        = RODOS::HAL_GPIO(pa10);
auto           pf4Pin         = RODOS::HAL_GPIO(pf4);
auto           pf5Pin         = RODOS::HAL_GPIO(pf5);
auto           debugUartRxPin = RODOS::HAL_GPIO(RODOS::UART_DEBUG_RX);


class HelloVa41620 : public RODOS::StaticThread<> {
    void run() {
        ledPin1.init(/*isOutput=*/true, /*numOfPins=*/1, /*initVal=*/0);
        ledPin2.init(/*isOutput=*/true, /*numOfPins=*/1, /*initVal=*/0);
        ledPin1.setPins(1); // Bottom LED
        ledPin2.setPins(1); // Top LED
        RODOS::AT(RODOS::NOW() + 1 * RODOS::SECONDS);

        RODOS::PRINTF("Hello VA41620\n");
        RODOS::PRINTF("s = %s, d = %017d, c = %c, f = %11.5f\n", "string", 42, 'X', 3.14);

        static constexpr auto toggleInterval = 250 * RODOS::MILLISECONDS;
        TIME_LOOP(0, toggleInterval) {
            if(ledPin1.readPins() == 0) {
                ledPin1.setPins(1); // Bottom LED
                ledPin2.setPins(0); // Top LED
            } else {
                ledPin1.setPins(0); // Bottom LED
                ledPin2.setPins(1); // Top LED
            }
        }
    }
} helloVa41620;


class InterruptCounter : public RODOS::IOEventReceiver {
  public:
    auto onDataReady() -> void override { ++nInterrupts; }
    int  nInterrupts = 0;
};


class InterruptTest : public RODOS::StaticThread<> {
  private:
    auto run() -> void override {
        static constexpr auto interruptCountingInterval = 200 * RODOS::MILLISECONDS;

        debugUartRxPin.init(/*isOutput=*/false, /*numOfPins=*/1, /*initVal=*/0);
        // pf4Pin.init(/*isOutput=*/false, /*numOfPins=*/1, /*initVal=*/0);
        // pf5Pin.init(/*isOutput=*/false, /*numOfPins=*/1, /*initVal=*/0);
        debugUartRxPin.config(RODOS::GPIO_CFG_TYPE::GPIO_CFG_IRQ_SENSITIVITY, RODOS::GPIO_IRQ_SENS_BOTH);
        // pf4Pin.config(RODOS::GPIO_CFG_TYPE::GPIO_CFG_IRQ_SENSITIVITY, RODOS::GPIO_IRQ_SENS_BOTH);
        // pf5Pin.config(RODOS::GPIO_CFG_TYPE::GPIO_CFG_IRQ_SENSITIVITY, RODOS::GPIO_IRQ_SENS_BOTH);

        RODOS::PRINTF("\n");
        RODOS::PRINTF("Waiting for both edge interrupts on debug UART RX pin\n");
        auto interruptCounter = InterruptCounter{};
        // auto interruptCounterPf4 = InterruptCounter{};
        // auto interruptCounterPf5 = InterruptCounter{};
        while(true) {
            interruptCounter.nInterrupts = 0;
            // interruptCounterPf4.nInterrupts = 0;
            // interruptCounterPf5.nInterrupts = 0;
            debugUartRxPin.resetInterruptEventStatus();
            // pf4Pin.resetInterruptEventStatus();
            // pf5Pin.resetInterruptEventStatus();

            debugUartRxPin.setIoEventReceiver(&interruptCounter);
            // pf4Pin.setIoEventReceiver(&interruptCounterPf4);
            // pf5Pin.setIoEventReceiver(&interruptCounterPf5);

            debugUartRxPin.interruptEnable(true);
            // pf4Pin.interruptEnable(true);
            // pf5Pin.interruptEnable(true);

            debugUartRxPin.suspendUntilDataReady(RODOS::END_OF_TIME);
            // pf4Pin.suspendUntilDataReady(RODOS::END_OF_TIME);
            RODOS::AT(RODOS::NOW() + interruptCountingInterval);

            debugUartRxPin.interruptEnable(false);
            // pf4Pin.interruptEnable(false);
            // pf5Pin.interruptEnable(false);

            debugUartRxPin.setIoEventReceiver(nullptr);
            // pf4Pin.setIoEventReceiver(nullptr);
            // pf5Pin.setIoEventReceiver(nullptr);
            RODOS::PRINTF("%3i interrupt(s) occured\n", interruptCounter.nInterrupts);
            // RODOS::PRINTF("%3i interrupt(s) occured on PF4\n", interruptCounterPf4.nInterrupts);
            // RODOS::PRINTF("%3i interrupt(s) occured on PF5\n", interruptCounterPf5.nInterrupts);
        }
    }
} interruptTest;
