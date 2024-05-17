#include <iostream>
#include <cstdint> 
#include <vector> 
#include <stdint.h>
/* #include <stack> */
#include <fstream>
#include <SDL2/SDL.h> 

uint16_t registers[16]; 
uint16_t idxRegister = 0; //used to point at locations in memory
std::vector<uint8_t> ram(4096, 0); //initializes ram	
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
int programCounter = 512; 
void readAndStoreRom();
void initializeFont();

int main() {
    std::cout << " hello \n"; 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Potato Chip8",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);;
    SDL_Surface* screenSurface = NULL;
    if( window != NULL ) {
        std::cout <<  "Window created!\n";
         screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
            
            //Update the surface
            SDL_UpdateWindowSurface( window );

            //Hack to get window to stay up
            SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }

    }
    initializeFont();
    readAndStoreRom(); 
    while (true) {
    }

	//timing should be 700 cycles per second 
	/* while (running) { */
	/* 	 uint8_t firstHalfInstruct = ram[programCounter]; */ 
	/* 	 uint8_t secondHalfInstruct = ram[programCounter]; */ 
	/* 	* uint16_t instruction = static_cast<uint16_t>(firstHalfInstruct << 8); */ 
	/* 	 instruction = instruction | secondHalfInstruct; 	 */ 
	/* 	 programCounter+=2;	 */ 
	/*  */
	/* } */
	

	return 0;

}

void initializeFont() {
   for (int i = 0; i < 80; i++) {
        ram[i + 50] = font[i];
   }
}

void readAndStoreRom () {
    std::string file_name = "2-ibm-logo.ch8";
        std::ifstream file(file_name, std::ios::binary);
        unsigned char byte;
        int i = 0x200; //512 in decimal
        while (file.read(reinterpret_cast<char*>(&byte), 1)) {
             ram[i++] = byte;
            // Print each byte as a two-digit hexadecimal number
              std::cout << std::hex << std::setfill('0') << std::setw(2) << (static_cast<int>(ram[i-1])) << ' '; 

        }
    

}

void decode(uint16_t instruction) {
	uint16_t lastThreeNibbles = instruction & 0x0FFF; 
	uint8_t secondNibble = 0x0F00 & instruction >> 8;
	uint8_t thirdNibble = 0x00F0 & instruction >> 4;
	uint8_t fourthNibble = 0x000F & instruction;
	uint8_t lastTwoNibbles = 0x00FF & instruction;

	switch(instruction & 0xF000) {
		case 0x0000:
			if ((0x0FFF & instruction) == 0x00E0) {
				for (int i = 0; i < 32; i++) {
                    for (int j = 0; j < 64; j++) {
                        display[i][j] = false;
                    }
                }
                
			}
			break; 
			
		case 0x1000: //jump to memory address
			programCounter = lastThreeNibbles; 
			break;
		case 0x6000: //6XNN sets register X to the number NN
			registers[secondNibble] = lastTwoNibbles; 
			break;
		case 0x7000: //7XNN adds register X by NN 
			registers[secondNibble] += lastTwoNibbles;	
			break;
		case 0xA000:
			idxRegister = lastThreeNibbles; 
			break;
		case 0xD000: //display DXYN
			uint16_t x_coord = registers[secondNibble] % 64;
			uint16_t y_coord = registers[thirdNibble] % 32;
			registers[15] = 0;
			uint8_t height = fourthNibble;
			for (int r = 0; r < height; r++) {
				uint8_t spritePixel = ram[idxRegister + r];
				
				for (int c = 0; c < 8 ; c++) {
                    int shift_x = x_coord + r; 
                    if (shift_x > 63) break;
					if ((spritePixel & (0x80 << c)) == 0) {
                        int shift_y = y_coord + c; 
                        if (shift_y > 31) break; 
                       if (display[shift_x][shift_y]) {
                            display[shift_x][shift_y] = false;
                            registers[15] = 1;
                       } else {
                           display[x_coord][y_coord] = true;
                            
                       }
					}
				}
			}
			break;
		
	}

}
