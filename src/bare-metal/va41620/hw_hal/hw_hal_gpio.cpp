#include "cortex_m/nvic.h"
#include "hw/irq_handlers.h"
#include "hw_hal_gpio.h"
#include "peripheral_ctrl/peripheral_defs.h"
#include "rodos-debug.h"

#include <bit>
#include <cstdint>

namespace RODOS {

HW_HAL_GPIO::HW_HAL_GPIO(pin_config::VA41620_PIN pinIdx, HAL_GPIO& owner)
    : m_idx{pinIdx}
    , m_owner{owner}
    , m_portNum{pinIdx.getPortNum()}
    , m_gpio{gpioStructs[m_portNum]}
{}

int32_t HW_HAL_GPIO::init(bool isOutput, uint8_t numOfPins, uint32_t initVal){
    int returnValue = setNumPins(numOfPins);
    if(returnValue != 0){
        return returnValue;
    }
    sysconfigPeripheralBitband->GPIO_PORT_ENABLE[m_portNum].write(1);
    if(isOutput){
        setOutput(static_cast<uint16_t>(initVal));
        setMode(GPIO::MODE::OUTPUT);
    }
    return returnValue;
}

int32_t HW_HAL_GPIO::setMode(GPIO::MODE mode){
    switch(mode){
        case GPIO::MODE::INPUT:
            m_gpio.DIR.clearBits(m_pinMask);
            return 0;
        case GPIO::MODE::OUTPUT:
            // Enable reading from the pins even in output mode
            for(int8_t i = 0; i < m_numPins; ++i){
                ioconfig->PINx[m_idx + i].setBits(IOCONFIG_PINx::IEWO(1));
            }
            m_gpio.DIR.setBits(m_pinMask);
            return 0;
    }
    RODOS_ERROR("Invalid GPIO mode");
    return -1;
}

int32_t HW_HAL_GPIO::setNumPins(uint8_t numPins){
    m_numPins = numPins;
    uint32_t nOnes = (1ul<<numPins)-1;
    uint32_t pinMask = nOnes << (m_idx.getPinInPort());
    bool validPinmask = (pinMask & 0xFFFF'0000) == 0;
    m_pinMask = static_cast<uint16_t>(pinMask);
    return validPinmask ? 0 : -1;
}

int32_t HW_HAL_GPIO::setPull(pin_config::PULL pull){
    for(int8_t i = 0; i < m_numPins; ++i){
        ioconfig->PINx[m_idx + i].set(
            IOCONFIG_PINx::PEN(pull == pin_config::PULL::OFF ? 0 : 1),
            IOCONFIG_PINx::PLEVEL(pull == pin_config::PULL::DOWN ? 0 : 1)
        );
    }
    return 0;
}

void HW_HAL_GPIO::reset(){
    m_gpio.DIR.clearBits(m_pinMask);
    m_gpio.DATAOUT.clearBits(m_pinMask);

    disableInterrupts();
    m_gpio.IRQ_SEN.clearBits(m_pinMask);
    m_gpio.IRQ_EDGE.clearBits(m_pinMask);
    m_gpio.IRQ_EVT.clearBits(m_pinMask);
    (void)m_gpio.EDGE_STATUS.read();  // EDGE_STATUS is cleared by reading it
    m_interruptEventOccurred = false;
}

void HW_HAL_GPIO::setOutput(uint16_t val){
    m_gpio.SETOUT.write(static_cast<uint32_t>((val<<(m_idx.getPinInPort()))&m_pinMask));
    m_gpio.CLROUT.write(static_cast<uint32_t>((static_cast<uint16_t>(~val)<<(m_idx.getPinInPort()))&m_pinMask));
}

uint16_t HW_HAL_GPIO::readInput(){
    return static_cast<uint16_t>( (m_gpio.DATAIN.read()&m_pinMask) >> m_idx.getPinInPort());
}

void HW_HAL_GPIO::setIrqSensitivity(GPIO_IRQ_SENSITIVITY sensitivity)
{
    switch(sensitivity){
        case GPIO_IRQ_SENS_RISING:
            m_gpio.IRQ_EDGE.clearBits(m_pinMask);
            m_gpio.IRQ_EVT.setBits(m_pinMask);
            break;
        case GPIO_IRQ_SENS_FALLING:
            m_gpio.IRQ_EDGE.clearBits(m_pinMask);
            m_gpio.IRQ_EVT.clearBits(m_pinMask);
            break;
        case GPIO_IRQ_SENS_BOTH:
            m_gpio.IRQ_EDGE.setBits(m_pinMask);
            break;
    }
}

void HW_HAL_GPIO::enableInterrupts(){
    if(m_idx >= GPIO_096){
        RODOS_ERROR("Port G does not have interrupt capability");
        return;
    }
    m_gpio.IRQ_ENB.setBits(m_pinMask);

    for(int8_t i = 0; i < m_numPins; ++i){
        contextArray[m_idx + i] = this;
        auto irqNumber = getPinIrqNumber(m_idx + i);
        NVIC::clearPendingIRQ(irqNumber);
        NVIC::enableIRQ(irqNumber);
    }
}

void HW_HAL_GPIO::disableInterrupts(){
    m_gpio.IRQ_ENB.clearBits(m_pinMask);

    for(int8_t i = 0; i < m_numPins; ++i){
        contextArray[m_idx + i] = nullptr;
        auto irqNumber = getPinIrqNumber(m_idx + i);
        NVIC::disableIRQ(irqNumber);
        NVIC::clearPendingIRQ(irqNumber);
    }
}

void HW_HAL_GPIO::irqHandler(){
    m_interruptEventOccurred = true;
    m_owner.upCallDataReady();
}


uint8_t HW_HAL_GPIO::getPinIrqNumber(int8_t pinIdx){
    static constexpr auto pa0IrqNumber = 78;
    return static_cast<uint8_t>(pa0IrqNumber + pinIdx);
}


template<int8_t pinIdx>
inline void PinIrqHandler(){
    if(HW_HAL_GPIO::contextArray[pinIdx] != nullptr){
        HW_HAL_GPIO::contextArray[pinIdx]->irqHandler();
    }
    NVIC::clearPendingIRQ(HW_HAL_GPIO::getPinIrqNumber(pinIdx));
}

}

extern "C" {
// Port A
void PA0_IRQHandler()  { RODOS::PinIrqHandler<0>(); }
void PA1_IRQHandler()  { RODOS::PinIrqHandler<1>(); }
void PA2_IRQHandler()  { RODOS::PinIrqHandler<2>(); }
void PA3_IRQHandler()  { RODOS::PinIrqHandler<3>(); }
void PA4_IRQHandler()  { RODOS::PinIrqHandler<4>(); }
void PA5_IRQHandler()  { RODOS::PinIrqHandler<5>(); }
void PA6_IRQHandler()  { RODOS::PinIrqHandler<6>(); }
void PA7_IRQHandler()  { RODOS::PinIrqHandler<7>(); }
void PA8_IRQHandler()  { RODOS::PinIrqHandler<8>(); }
void PA9_IRQHandler()  { RODOS::PinIrqHandler<9>(); }
void PA10_IRQHandler() { RODOS::PinIrqHandler<10>(); }
void PA11_IRQHandler() { RODOS::PinIrqHandler<11>(); }
void PA12_IRQHandler() { RODOS::PinIrqHandler<12>(); }
void PA13_IRQHandler() { RODOS::PinIrqHandler<13>(); }
void PA14_IRQHandler() { RODOS::PinIrqHandler<14>(); }
void PA15_IRQHandler() { RODOS::PinIrqHandler<15>(); }
// Port B
void PB0_IRQHandler()  { RODOS::PinIrqHandler<16>(); }
void PB1_IRQHandler()  { RODOS::PinIrqHandler<17>(); }
void PB2_IRQHandler()  { RODOS::PinIrqHandler<18>(); }
void PB3_IRQHandler()  { RODOS::PinIrqHandler<19>(); }
void PB4_IRQHandler()  { RODOS::PinIrqHandler<20>(); }
void PB5_IRQHandler()  { RODOS::PinIrqHandler<21>(); }
void PB6_IRQHandler()  { RODOS::PinIrqHandler<22>(); }
void PB7_IRQHandler()  { RODOS::PinIrqHandler<23>(); }
void PB8_IRQHandler()  { RODOS::PinIrqHandler<24>(); }
void PB9_IRQHandler()  { RODOS::PinIrqHandler<25>(); }
void PB10_IRQHandler() { RODOS::PinIrqHandler<26>(); }
void PB11_IRQHandler() { RODOS::PinIrqHandler<27>(); }
void PB12_IRQHandler() { RODOS::PinIrqHandler<28>(); }
void PB13_IRQHandler() { RODOS::PinIrqHandler<29>(); }
void PB14_IRQHandler() { RODOS::PinIrqHandler<30>(); }
void PB15_IRQHandler() { RODOS::PinIrqHandler<31>(); }
// Port C
void PC0_IRQHandler()  { RODOS::PinIrqHandler<32>(); }
void PC1_IRQHandler()  { RODOS::PinIrqHandler<33>(); }
void PC2_IRQHandler()  { RODOS::PinIrqHandler<34>(); }
void PC3_IRQHandler()  { RODOS::PinIrqHandler<35>(); }
void PC4_IRQHandler()  { RODOS::PinIrqHandler<36>(); }
void PC5_IRQHandler()  { RODOS::PinIrqHandler<37>(); }
void PC6_IRQHandler()  { RODOS::PinIrqHandler<38>(); }
void PC7_IRQHandler()  { RODOS::PinIrqHandler<39>(); }
void PC8_IRQHandler()  { RODOS::PinIrqHandler<40>(); }
void PC9_IRQHandler()  { RODOS::PinIrqHandler<41>(); }
void PC10_IRQHandler() { RODOS::PinIrqHandler<42>(); }
void PC11_IRQHandler() { RODOS::PinIrqHandler<43>(); }
void PC12_IRQHandler() { RODOS::PinIrqHandler<44>(); }
void PC13_IRQHandler() { RODOS::PinIrqHandler<45>(); }
void PC14_IRQHandler() { RODOS::PinIrqHandler<46>(); }
void PC15_IRQHandler() { RODOS::PinIrqHandler<47>(); }
// Port D
void PD0_IRQHandler()  { RODOS::PinIrqHandler<48>(); }
void PD1_IRQHandler()  { RODOS::PinIrqHandler<49>(); }
void PD2_IRQHandler()  { RODOS::PinIrqHandler<50>(); }
void PD3_IRQHandler()  { RODOS::PinIrqHandler<51>(); }
void PD4_IRQHandler()  { RODOS::PinIrqHandler<52>(); }
void PD5_IRQHandler()  { RODOS::PinIrqHandler<53>(); }
void PD6_IRQHandler()  { RODOS::PinIrqHandler<54>(); }
void PD7_IRQHandler()  { RODOS::PinIrqHandler<55>(); }
void PD8_IRQHandler()  { RODOS::PinIrqHandler<56>(); }
void PD9_IRQHandler()  { RODOS::PinIrqHandler<57>(); }
void PD10_IRQHandler() { RODOS::PinIrqHandler<58>(); }
void PD11_IRQHandler() { RODOS::PinIrqHandler<59>(); }
void PD12_IRQHandler() { RODOS::PinIrqHandler<60>(); }
void PD13_IRQHandler() { RODOS::PinIrqHandler<61>(); }
void PD14_IRQHandler() { RODOS::PinIrqHandler<62>(); }
void PD15_IRQHandler() { RODOS::PinIrqHandler<63>(); }
// Port E
void PE0_IRQHandler()  { RODOS::PinIrqHandler<64>(); }
void PE1_IRQHandler()  { RODOS::PinIrqHandler<65>(); }
void PE2_IRQHandler()  { RODOS::PinIrqHandler<66>(); }
void PE3_IRQHandler()  { RODOS::PinIrqHandler<67>(); }
void PE4_IRQHandler()  { RODOS::PinIrqHandler<68>(); }
void PE5_IRQHandler()  { RODOS::PinIrqHandler<69>(); }
void PE6_IRQHandler()  { RODOS::PinIrqHandler<70>(); }
void PE7_IRQHandler()  { RODOS::PinIrqHandler<71>(); }
void PE8_IRQHandler()  { RODOS::PinIrqHandler<72>(); }
void PE9_IRQHandler()  { RODOS::PinIrqHandler<73>(); }
void PE10_IRQHandler() { RODOS::PinIrqHandler<74>(); }
void PE11_IRQHandler() { RODOS::PinIrqHandler<75>(); }
void PE12_IRQHandler() { RODOS::PinIrqHandler<76>(); }
void PE13_IRQHandler() { RODOS::PinIrqHandler<77>(); }
void PE14_IRQHandler() { RODOS::PinIrqHandler<78>(); }
void PE15_IRQHandler() { RODOS::PinIrqHandler<79>(); }
// Port F
void PF0_IRQHandler()  { RODOS::PinIrqHandler<80>(); }
void PF1_IRQHandler()  { RODOS::PinIrqHandler<81>(); }
void PF2_IRQHandler()  { RODOS::PinIrqHandler<82>(); }
void PF3_IRQHandler()  { RODOS::PinIrqHandler<83>(); }
void PF4_IRQHandler()  { RODOS::PinIrqHandler<84>(); }
void PF5_IRQHandler()  { RODOS::PinIrqHandler<85>(); }
void PF6_IRQHandler()  { RODOS::PinIrqHandler<86>(); }
void PF7_IRQHandler()  { RODOS::PinIrqHandler<87>(); }
void PF8_IRQHandler()  { RODOS::PinIrqHandler<88>(); }
void PF9_IRQHandler()  { RODOS::PinIrqHandler<89>(); }
void PF10_IRQHandler() { RODOS::PinIrqHandler<90>(); }
void PF11_IRQHandler() { RODOS::PinIrqHandler<91>(); }
void PF12_IRQHandler() { RODOS::PinIrqHandler<92>(); }
void PF13_IRQHandler() { RODOS::PinIrqHandler<93>(); }
void PF14_IRQHandler() { RODOS::PinIrqHandler<94>(); }
void PF15_IRQHandler() { RODOS::PinIrqHandler<95>(); }
// Port G does not support interrupts
}
