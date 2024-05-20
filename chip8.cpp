#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <vector>
/* #include <stack> */
#include <SDL2/SDL.h>
#include <fstream>

uint16_t registers[16];
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
/* std::stack<uint16_t> data_stk; //todo: stackoverflow handle	 */
/*  */
/* uint8_t delayTimer = 0; //delays clock when above 0  */
/* uint8_t soundTimer = 0; //beeps when above 0  */
/*  */
/* //todo add key commands */
/* bool running = true; */
int programCounter = 0x200;
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
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
  // bool quit = false;
  // SDL_Event e;
  // while (!quit) {
  //   while (SDL_PollEvent(&e) != 0) {
  //     if (e.type == SDL_QUIT) {
  //       quit = true;
  //     }
  //   }
  //   SDL_RenderClear(renderer);
  //   SDL_RenderCopy(renderer, texture, NULL, NULL);
  //   SDL_RenderPresent(renderer);
  // }

  // SDL_DestroyTexture(texture);
  // SDL_DestroyRenderer(renderer);
  // SDL_DestroyWindow(window);
  // SDL_Quit();

  // timing should be 700 cycles per second
  bool running = true;
  SDL_Event e;
  programCounter = 0x200; 
  while (running) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false; 
      }
    }
    std::cout << "program counter: " << programCounter << std::endl;
    uint8_t firstHalfInstruct = ram[programCounter];
    uint8_t secondHalfInstruct = ram[programCounter+1];
    uint16_t instruction = static_cast<uint16_t>(firstHalfInstruct << 8);
  
    instruction = instruction | secondHalfInstruct;
    std::cout << "program counter before decoding " << std::dec << programCounter << std::endl;
    decode(instruction);
    programCounter += 2;
    SDL_Delay(3000);
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
  window =
      SDL_CreateWindow("Potato Chip8", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  ;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  // add error handling for window and renderer
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);
  Uint32 *pixels = nullptr;
  int pitch = 0;
  SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

  for (int y = 0; y < 480; y++) {
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
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);
  Uint32 *pixels = nullptr;
  int pitch = 0;
  SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      bool isPixelTurnedOn = display[y / 10][x / 10];
      Uint32 color = 0;

      if (isPixelTurnedOn) {
        color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255,
                            255, 255);
      } else {
        color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 0,
                            255);
      }
      pixels[y * (pitch / 4) + x] = color;
    }
  }
  SDL_UnlockTexture(texture);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void decode(uint16_t instruction) {
  std::cout << "program counter inside instruction " << std::dec << programCounter << std::endl;
  std::cout << "decoding " << std::hex << instruction << std::endl;
  uint16_t lastThreeNibbles = instruction & 0x0FFF;
  uint8_t secondNibble = 0x0F00 & instruction >> 8;
  uint8_t thirdNibble = 0x00F0 & instruction >> 4;
  uint8_t fourthNibble = 0x000F & instruction;
  uint8_t lastTwoNibbles = 0x00FF & instruction;

  switch (instruction & 0xF000) {
  case 0x0000:
    std::cout << "clearing screen" << std::endl;
    if ((0x0FFF & instruction) == 0x00E0) {
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          display[i][j] = false;
        }
      }
    }
    return;

  case 0x1000: // jump to memory address
    std::cout << "jumping through instruction " << std::hex << instruction << std::endl;
    programCounter = lastThreeNibbles;
    return;
  case 0x6000: // 6XNN sets register X to the number NN
    registers[secondNibble] = lastTwoNibbles;
    return;
  case 0x7000: // 7XNN adds register X by NN
    registers[secondNibble] += lastTwoNibbles;
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
        int shift_x = x_coord + r;
        int shift_y = y_coord + c;
        if (shift_x > 63)
          break;
        if (shift_y > 31)
          break;
        if (spritePixel && display[shift_x][shift_y]) {
          display[shift_x][shift_y] = false;
          registers[15] = 1;
        } else if (spritePixel && !display[shift_x][shift_y]) {
          display[shift_x][shift_y] = true;
        }
      }
    }
    redrawScreen();
    return;
  }
}
