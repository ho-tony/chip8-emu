#ifndef GPU_H
#define GPU_H

#include <SDL2/SDL_render.h>
#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>

class GPU{
public:
    static const int SCREEN_WIDTH = 640;
    static const int SCREEN_HEIGHT = 320;
    GPU();

    void redrawScreen();
    void destroyGPU();
    bool display[32][64];
private:
    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    
};



#endif