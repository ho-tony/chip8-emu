#include "chip8machine.h"
#include <fstream>

Chip8Machine::Chip8Machine(): cpu(gpu, ram), ram(4096, 0) {
  isRunning = true;
  
}

void Chip8Machine::readAndStoreRom() {
  std::string file_name = "2-ibm-logo.ch8";
  std::ifstream file(file_name, std::ios::binary);
  unsigned char byte;
  int i = 0x200; // 512 in decimal
  while (file.read(reinterpret_cast<char *>(&byte), 1)) {
    ram[i++] = byte;
    // Print each byte as a two-digit hexadecimal number
    // std::cout << std::hex << std::setfill('0') << std::setw(2)
    //           << (static_cast<int>(ram[i - 1])) << ' ';
  }
}
void Chip8Machine::initializeFont() {
  for (int i = 0; i < 80; i++) {
    ram[i + 50] = font[i];
  }
}

void Chip8Machine::cpuRetrieveNextInstruction() {
    cpu.retrieveNextInstruction();
}
void Chip8Machine::destroyGPU() { gpu.destroyGPU(); }
