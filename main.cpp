#include <iostream>
#include <SDL3/SDL.h>

#include "cpu.hpp"

//Key pressed
enum Key
{
    ONE, TWO, THR, FOU,
    Q, W, E, R,
    A, S, D, F,
    Z, X, C, V
};

int main()
{

    SDL_Init(SDL_INIT_VIDEO);

    // Size of each pixel in a 64x32 grid
    const int pixel_size = 20;

    SDL_Window* window = SDL_CreateWindow("blank", 64 * pixel_size, 32 * pixel_size, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    
    SDL_Event e;
    bool run = true;
    while(run)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                run = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
