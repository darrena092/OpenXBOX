#include "char_serial.h"

namespace openxbox {

LinuxSerialDriver::LinuxSerialDriver(uint8_t portNum) {

}

bool LinuxSerialDriver::Init() {
    // Nothing to initialize
    printf("Initializing serial stuff!\n");
    return true;
}

int LinuxSerialDriver::Write(const uint8_t *buf, int len) {
    // Discard everything
    return 1;
}

void LinuxSerialDriver::AcceptInput() {
    // Nothing to do
}

void LinuxSerialDriver::Stop() {
    // Nothing to do
}

// ----- IOCTLs ---------------------------------------------------------------

void LinuxSerialDriver::SetBreakEnable(bool breakEnable) {
    // Nothing to do
}

void LinuxSerialDriver::SetSerialParameters(SerialParams *params) {
    // Nothing to do
}

}
