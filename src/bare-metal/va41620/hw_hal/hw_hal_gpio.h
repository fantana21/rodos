#pragma once
#include "hal/hal_gpio.h"
#include "peripheral_ctrl/register_maps/gpio_registers.h"
#include "peripheral_ctrl/pin_config/enums.h"

#include <cstdint>

namespace RODOS {

namespace GPIO {
    enum class MODE : uint8_t {
        INPUT = 0,
        OUTPUT,
    };
} // namespace GPIO


class HW_HAL_GPIO {
public:
    static inline HW_HAL_GPIO* contextArray[pin_config::TOTAL_PINS] = {};

    HW_HAL_GPIO(pin_config::VA41620_PIN pinIdx, HAL_GPIO& owner);

    int32_t init(bool isOutput, uint8_t numOfPins, uint32_t initVal);
    int32_t setMode(GPIO::MODE mode);
    int32_t setNumPins(uint8_t numPins);
    int32_t setPull(pin_config::PULL pull);

    void reset();
    void setOutput(uint16_t val);
    uint16_t readInput();

    void setIrqSensitivity(GPIO_IRQ_SENSITIVITY sensitivity);
    void enableInterrupts();
    void disableInterrupts();
    void irqHandler();
    static std::uint8_t getPinIrqNumber(std::int8_t pinIdx);

    bool m_interruptEventOccurred { false };

private:
    pin_config::VA41620_PIN m_idx;
    HAL_GPIO& m_owner;
    uint8_t m_numPins;
    int8_t m_portNum;
    uint16_t m_pinMask;
    GPIOStruct& m_gpio;
};

}
