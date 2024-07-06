#ifndef CPU_H
#define CPU_H

#include <stack>
#include <cstdint>
#include <vector>
#include "gpu.h"

class CPU {
public:
    CPU(GPU& gpu, std::vector<uint8_t> &ram);
    void decode(uint16_t instruction);
    void retrieveNextInstruction();
    void updateTimers();
private: 
    std::vector<uint8_t> ram; // initializes ram
    int prevTime = 0;
    GPU& gpu;
    uint16_t programCounter = 0x200;
    std::stack<uint16_t> dataStk;
    uint8_t registers[16];
    uint16_t idxRegister;
    uint8_t delayTimer = 0;
    uint8_t soundTimer;
    uint8_t keysPressed[16];
    Uint32 TIMER_INTERVAL = 1000 / 60;
  
};

#endif // CPU_H