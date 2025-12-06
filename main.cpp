#include <iostream>
#include <SDL3/SDL.h>

#include "cpu.hpp"

//Key pressed
enum Key
{
    K1, K2, K3, K4,
    KQ, KW, KE, KR,
    KA, KS, KD, KF,
    KZ, KX, KC, KV
};

CPU myCPU;

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    // Size of each pixel in a 64x32 grid
    const int pixel_size = 20;

    SDL_Window* window = SDL_CreateWindow("blank", 64 * pixel_size, 32 * pixel_size, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    
    SDL_Event e;
    bool run = true;

    const bool* kb = SDL_GetKeyboardState(NULL);

    myCPU.initialize();
    myCPU.load("");
    while(run)
    {
        myCPU.emulateCycle();

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                run = false;
            }
        }

        for(int i=0;i<16;i++)
        {
            myCPU.setKey(i, false);
        }

        if(kb[SDL_SCANCODE_1]) myCPU.setKey(Key::K1, true); 
        if(kb[SDL_SCANCODE_2]) myCPU.setKey(Key::K2, true);
        if(kb[SDL_SCANCODE_3]) myCPU.setKey(Key::K3, true);
        if(kb[SDL_SCANCODE_4]) myCPU.setKey(Key::K4, true);
        if(kb[SDL_SCANCODE_Q]) myCPU.setKey(Key::KQ, true);
        if(kb[SDL_SCANCODE_W]) myCPU.setKey(Key::KW, true);
        if(kb[SDL_SCANCODE_E]) myCPU.setKey(Key::KE, true);
        if(kb[SDL_SCANCODE_R]) myCPU.setKey(Key::KR, true);
        if(kb[SDL_SCANCODE_A]) myCPU.setKey(Key::KA, true);
        if(kb[SDL_SCANCODE_S]) myCPU.setKey(Key::KS, true);
        if(kb[SDL_SCANCODE_D]) myCPU.setKey(Key::KD, true);
        if(kb[SDL_SCANCODE_F]) myCPU.setKey(Key::KF, true);
        if(kb[SDL_SCANCODE_Z]) myCPU.setKey(Key::KZ, true);
        if(kb[SDL_SCANCODE_X]) myCPU.setKey(Key::KX, true);
        if(kb[SDL_SCANCODE_C]) myCPU.setKey(Key::KC, true);
        if(kb[SDL_SCANCODE_V]) myCPU.setKey(Key::KV, true);

        // Draw the emulated graphics
        if(myCPU.draw_flag)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderClear(renderer);

            for(int y_line = 0;y_line < 32;y_line++)
            {
                for(int x_line = 0;x_line < 64;x_line++)
                {
                    SDL_FRect pixel = {x_line * pixel_size, y_line * pixel_size, pixel_size, pixel_size};
                    if(myCPU.getGfx()[(y_line * 64) + x_line])
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    }
                    else if(!myCPU.getGfx()[(y_line * 64) + x_line])
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }

            myCPU.draw_flag = false;
        }

        

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
