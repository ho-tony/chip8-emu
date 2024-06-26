#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <stack> 
#include <SDL2/SDL.h>
#include <fstream>

uint8_t registers[16];
uint16_t idxRegister = 0;          // used to point at locations in memory
std::vector<uint8_t> ram(4096, 0); // initializes ram
uint16_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
bool display[32][64];
std::stack<uint16_t> data_stk; //todo: stackoverflow handle	 
/*  */
/* uint8_t delayTimer = 0; //delays clock when above 0  */
/* uint8_t soundTimer = 0; //beeps when above 0  */
/*  */
/* //todo add key commands */
/* bool running = true; */
int programCounter = 0x200;
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 320;
SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;

void readAndStoreRom();
void initializeFont();
void redrawScreen();
void initalizeScreen();
void decode(uint16_t instruction);

int main() {
  std::cout << " potato chip running\n";
  initializeFont();
  readAndStoreRom();
  initalizeScreen();

  // timing should be 700 cycles per second
  bool running = true;
  SDL_Event e;
  programCounter = 0x200; // 512 in hex
  while (running) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }
    uint8_t firstHalfInstruct = ram[programCounter];
    uint8_t secondHalfInstruct = ram[programCounter + 1];
    uint16_t instruction = static_cast<uint16_t>(firstHalfInstruct << 8);

    instruction = instruction | secondHalfInstruct;
    decode(instruction);
    programCounter += 2;
    SDL_Delay(300);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void initalizeScreen() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return;
  }
  window = SDL_CreateWindow("Potato Chip8", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  ;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  // add error handling for window and renderer
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);
  Uint32 *pixels = nullptr;
  int pitch = 0;
  SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

  for (int y = 0; y < 320; y++) {
    for (int x = 0; x < 640; x++) {
      Uint32 color =
          SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 0, 255);
      pixels[y * (pitch / 4) + x] = color;
    }
  }
}
void initializeFont() {
  for (int i = 0; i < 80; i++) {
    ram[i + 50] = font[i];
  }
}

void readAndStoreRom() {
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

/*
 * Probably have to redo this function for effective animations
 *
 */

void redrawScreen() {
  SDL_RenderClear(renderer);
  for (int y = 0; y < 32; ++y) {
    for (int x = 0; x < 64; ++x) {
      bool isPixelTurnedOn = display[y][x];
      if (isPixelTurnedOn) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
      SDL_Rect block{x * 10, y * 10, 10, 10};
      SDL_RenderDrawRect(renderer, &block);
      SDL_RenderFillRect(renderer, &block);
    }
  }

  SDL_RenderPresent(renderer);
}
void decode(uint16_t instruction) {
  uint16_t lastThreeNibbles = instruction & 0x0FFF;
  uint8_t secondNibble = (0x0F00 & instruction) >> 8;
  uint8_t thirdNibble = (0x00F0 & instruction) >> 4;
  uint8_t fourthNibble = 0x000F & instruction;
  uint8_t lastTwoNibbles = 0x00FF & instruction;

  switch (instruction & 0xF000) {
  case 0x0000:
    switch ((0xFFFF & instruction)) {
    case 0x00E0:
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          display[i][j] = false;
        }
      }
      redrawScreen();
      return;
    //returns from subroutine 
    case 0x00EE: 
      programCounter = data_stk.top();
      data_stk.pop();
      break;
    default:
      return;
    }

  case 0x1000: // jump to memory address
    programCounter = lastThreeNibbles;
    return;
  case 0x2000:  
    data_stk.push(programCounter);
    programCounter = lastThreeNibbles;
    return;
  case 0x3000:
    //if vx is equal nn (3xnn)
    if (registers[secondNibble] == lastTwoNibbles) {
      programCounter+=2;
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
        registers[secondNibble] &= regitsers[thirdNibble];
        break;
      case 3:
        registers[secondNibble] ^= registers[thirdNibble];
        break;
      case 4: 
        uint16_t result = registers[secondNibble] + registers[thirdNibble];
        if (result > 255) {
          registers[0xF] = 1; 
        } else {
          registers[0xF] = 0;
        } 
        registers += registers[thirdNibble];
        break;
      case 5:
        if (registers[secondNibble] > registers[thirdNibble]) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        uint16_t result = registers[secondNibble] - registers[thirdNibble];
        registers[secondNibble] -= registers[thirdNibble];
        break;
      case 6:
        //todo shift
        break;
      case 7:
        if (registers[thirdNibble] > registers[secondNibble]) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        uint16_t result = registers[secondNibble] - registers[thirdNibble];
        registers[secondNibble] = registers[thirdNibble] - registers[secondNibble];
        break;
      case 0xE:
        //todo shift
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
  case 0xD000: // display DXYN

    uint16_t x_coord = registers[secondNibble] % 64;
    uint16_t y_coord = registers[thirdNibble] % 32;

    registers[15] = 0;
    uint8_t height = fourthNibble;

    for (int r = 0; r < height; r++) {
      uint8_t spritePixel = ram[idxRegister + r];

      for (int c = 0; c < 8; c++) {
        int shift_x = x_coord + c;
        int shift_y = y_coord + r;
        bool isSpriteBitOn = (spritePixel & (0x80 >> c));
        if (isSpriteBitOn) {
          if (display[shift_y][shift_x]) {
            registers[15] = 1;
          }
          display[shift_y][shift_x] ^= true;

          redrawScreen();
        }
      }
    }
    return;
  }
}
