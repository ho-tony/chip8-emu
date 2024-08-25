#include "cpu.h"
#include <SDL2/SDL_stdinc.h>
#include <iostream> 
#include <SDL2/SDL_timer.h>
#include <iomanip>  // for std::setw and std::setfill

CPU::CPU(GPU &gpuRef, std::vector<uint8_t> &ramRef) : ram(ramRef), gpu(gpuRef) {
  prevTime = SDL_GetTicks();

}

void CPU::updateTimers() {
  auto currentTime = SDL_GetTicks();
  // std::cout << "current " << currentTime << std::endl;
  // std::cout << "prev " << prevTime << std::endl;
  if (currentTime - prevTime < TIMER_INTERVAL) {
    return;
  }
  prevTime = currentTime;
  if (delayTimer > 0) {
    delayTimer--;
  }
  // todo: add sound timer
}
void CPU::retrieveNextInstruction() {
  
  // for (const auto& elem : ram) std::cout << elem << " "; std::cout << std::endl;
  uint8_t firstHalfInstruct = ram[programCounter];
  uint8_t secondHalfInstruct = ram[programCounter + 1];
  uint16_t instruction = static_cast<uint16_t>(firstHalfInstruct << 8);

  instruction = instruction | secondHalfInstruct;
  decode(instruction);
  programCounter += 2;
  updateTimers();
}
void CPU::decode(uint16_t instruction) {
  uint16_t lastThreeNibbles = instruction & 0x0FFF;
  uint8_t secondNibble = (0x0F00 & instruction) >> 8;
  uint8_t thirdNibble = (0x00F0 & instruction) >> 4;
  uint8_t fourthNibble = 0x000F & instruction;
  uint8_t lastTwoNibbles = 0x00FF & instruction;
  std::cout << "Instruction: 0x" 
              << std::hex << std::setw(4) << std::setfill('0') 
              << instruction << std::dec << std::endl;
  switch (instruction & 0xF000) {
  case 0x0000:
    switch ((0xFFFF & instruction)) {
    case 0x00E0:
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          gpu.display[i][j] = false;
        }
      }
      gpu.redrawScreen();
      return;
    // returns from subroutine
    case 0x00EE:
      programCounter = dataStk.top();
      dataStk.pop();
      break;
    default:
      return;
    }

  case 0x1000: // jump to memory address
    programCounter = lastThreeNibbles;
    return;
  case 0x2000:
    dataStk.push(programCounter);
    programCounter = lastThreeNibbles;
    return;
  case 0x3000:
    // if vx is equal nn (3xnn)
    if (registers[secondNibble] == lastTwoNibbles) {
      programCounter += 2;
    }

    return;
  case 0x4000:
    if (registers[secondNibble] != lastTwoNibbles) {
      programCounter += 2;
    }
    return;
  case 0x5000:
    if (registers[secondNibble] == registers[thirdNibble]) {
      programCounter += 2;
    }
    return;
  case 0x6000: // 6XNN sets register X to the number NN
    registers[secondNibble] = lastTwoNibbles;
    return;
  case 0x7000: // 7XNN adds register X by NN
    registers[secondNibble] += lastTwoNibbles;
    return;
  case 0x8000:
    switch (fourthNibble) {
    case 0:
      registers[secondNibble] = registers[thirdNibble];
      break;
    case 1:
      registers[secondNibble] |= registers[thirdNibble];
      break;
    case 2:
      registers[secondNibble] &= registers[thirdNibble];
      break;
    case 3:
      registers[secondNibble] ^= registers[thirdNibble];
      break;
    case 4: {

      int result = registers[secondNibble] + registers[thirdNibble];
      if (result > 255) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }
      registers[secondNibble] += registers[thirdNibble];
      break;
    }
    case 5: {
      if (registers[secondNibble] > registers[thirdNibble]) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }
      registers[secondNibble] -= registers[thirdNibble];
      break;
    }
    case 6:
      // optional set vx = vy
      registers[0xF] = registers[secondNibble] & 0x01;
      registers[secondNibble] >>= 1;

      // if (registers[secondNibble] == 0 || registers[secondNibble] == 1) {
      //   registers[0xF] = registers[secondNibble];
      // }
      // registers[secondNibble] = (registers[secondNibble] >> 1);
      break;
    case 7:
      if (registers[thirdNibble] > registers[secondNibble]) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }
      registers[secondNibble] =
          registers[thirdNibble] - registers[secondNibble];
      break;
    case 0xE:
      registers[secondNibble] = (registers[secondNibble] << 1);
      registers[0xF] = (registers[secondNibble] & 0x80) ? 1 : 0; // Set VF to the most significant bit before the shift
      break;
    }

    return;
  case 0x9000:
    if (registers[secondNibble] != registers[thirdNibble]) {
      programCounter += 2;
    }
    return;
  case 0xA000:
    idxRegister = lastThreeNibbles;
    return;
  case 0xB000:
    programCounter = lastThreeNibbles;
    return;
  case 0xC000: {
    int cRandNum = rand();
    uint8_t chipRandNum = lastTwoNibbles & cRandNum;
    registers[secondNibble] = chipRandNum;
    return;
  }
  case 0xE000:
    // skips if key press in vx is pressed down
    return;
  case 0xF000:
    switch (lastTwoNibbles) {
    case 0x07:
      registers[secondNibble] = delayTimer;
      return;
    case 0x15:
      delayTimer = registers[secondNibble];
      break;
    case 0x18:
      // todo sound timer
      return;
    case 0x1E:
      // check overflow
      idxRegister += registers[secondNibble];
      return;
    case 0x0A: {
      bool hasKeyBeenPressed = false;
      int keyPressed = -1;
      for (int i = 0; i < 16; i++) {
        if (keysPressed[i]) {
          hasKeyBeenPressed = true;
          keyPressed = i;
        }
      }
      if (!hasKeyBeenPressed) {
        programCounter -= 2;
      } else {
        registers[secondNibble] = keyPressed;
      }
      return;
    }

    case 0x29:
      idxRegister = 0x050 + registers[secondNibble] * 5;
      return;
    case 0x33:
      uint8_t num = registers[secondNibble];
      ram[idxRegister] = num / 100;
      ram[idxRegister + 1] = (num / 10) % 10;
      ram[idxRegister + 2] = num % 10;
      return;
    }
  case 0x55:
    for (int reg = 0; reg <= secondNibble; reg++) {
      ram[idxRegister + reg] = registers[reg];
    }
    return;
  case 0x65:
    for (int reg = 0; reg <= secondNibble; reg++) {
      registers[reg] = ram[idxRegister + reg];
    }
    return;
  case 0xD000: // display DXYN
    uint16_t x_coord = registers[secondNibble] % 64;
    uint16_t y_coord = registers[thirdNibble] % 32;

    registers[15] = 0;
    uint8_t height = fourthNibble;

    for (int r = 0; r < height; r++) {
      uint8_t spritePixel = ram[idxRegister + r];

      for (int c = 0; c < 8; c++) {
        int shift_x = (x_coord + c) % 64;
        int shift_y = (y_coord + r) % 32;
        bool isSpriteBitOn = (spritePixel & (0x80 >> c));
        if (isSpriteBitOn) {
          if (gpu.display[shift_y][shift_x]) {
            registers[15] = 1;
          }
          gpu.display[shift_y][shift_x] ^= true;

        }
      }
    }
    gpu.redrawScreen();
    return;
  }
}
