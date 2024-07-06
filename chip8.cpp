#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include "chip8machine.h"

int keyMap[256];
bool keysPressed[16];

void readAndStoreRom();
void initializeFont();
void updateTimers();
void intializeKeys();
void decode(uint16_t instruction);
void handleKeyPress(SDL_Keycode keycode, bool isKeyDown);

int main() {
  std::cout << " potato chip running\n";
  Chip8Machine chip8;
  intializeKeys();

  
  // timing should be 700 cycles per second
  SDL_Event e;
  while (chip8.isRunning) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        chip8.isRunning = false;
      }
      if (e.type == SDL_KEYDOWN) {
        handleKeyPress(e.key.keysym.sym, true);
      }
      if (e.type == SDL_KEYUP) {
        handleKeyPress(e.key.keysym.sym, false);
      }
    }
    chip8.cpuRetrieveNextInstruction();
    SDL_Delay(1); // prevents high cpu usage
  }
  chip8.destroyGPU();
  SDL_Quit();

  return 0;
}

void handleKeyPress(SDL_KeyCode sdl_key, bool isKeyDown) {
  if (sdl_key < 256 && keyMap[sdl_key] != -1) {
    keysPressed[keyMap[sdl_key]] = isKeyDown;
  }
}

void initalizeKeys() {
  keyMap[SDLK_1] = 0x1;
  keyMap[SDLK_2] = 0x2;
  keyMap[SDLK_3] = 0x3;
  keyMap[SDLK_4] = 0xC;
  keyMap[SDLK_q] = 0x4;
  keyMap[SDLK_w] = 0x5;
  keyMap[SDLK_e] = 0x6;
  keyMap[SDLK_r] = 0xD;
  keyMap[SDLK_a] = 0x7;
  keyMap[SDLK_s] = 0x8;
  keyMap[SDLK_d] = 0x9;
  keyMap[SDLK_f] = 0xE;
  keyMap[SDLK_z] = 0xA;
  keyMap[SDLK_x] = 0x0;
  keyMap[SDLK_c] = 0xB;
  keyMap[SDLK_v] = 0xF;
  return;
}

/*
 * Probably have to redo this function for effective animations
 *
 */

