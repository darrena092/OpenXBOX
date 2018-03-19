#pragma once

#include "../char.h"

namespace openxbox {

class LinuxSerialDriver : public CharDriver {
public:
    LinuxSerialDriver(uint8_t portNum);
    bool Init() override;
    int Write(const uint8_t *buf, int len) override;
    void AcceptInput() override;
    void Stop() override;

    // IOCTLs
    void SetBreakEnable(bool breakEnable) override;
    void SetSerialParameters(SerialParams *params) override;
};

}