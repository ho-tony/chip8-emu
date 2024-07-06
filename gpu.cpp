#include "gpu.h"
#include <iostream>
#include "SDL2/SDL.h"

GPU::GPU() {

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

void GPU::redrawScreen() {
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

void GPU::destroyGPU() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

