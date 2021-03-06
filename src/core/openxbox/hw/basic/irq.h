#pragma once

#include <cstdint>

namespace openxbox {

class IRQHandler {
public:
    virtual void HandleIRQ(uint8_t irqNum, int level) = 0;
};

struct IRQ {
    IRQHandler *handler;
    uint8_t num;

    inline void Handle(uint8_t level) {
        if (handler != nullptr) {
            handler->HandleIRQ(num, level);
        }
    }
};

IRQ *AllocateIRQs(IRQHandler *handler, uint8_t numIRQs);

}
