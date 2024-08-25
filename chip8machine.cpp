#include "chip8machine.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

Chip8Machine::Chip8Machine() :
  isRunning(true), ram(4096), gpu(), cpu(gpu, ram)
{ 
}


void Chip8Machine::readAndStoreRom() {
  // std::string file_name = "Tetris [Fran Dachille, 1991].ch8";
  // std::string file_name = "IBM Logo.ch8";
  std::string file_name = "INVADERS";
  std::ifstream file(file_name, std::ios::binary);
  unsigned char byte;
  int i = 0x200; // 512 in decimal
  while (file.read(reinterpret_cast<char *>(&byte), 1)) {
    this->cpu.ram[i++] = byte;
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
