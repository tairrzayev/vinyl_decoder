#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "vinyl.h"

bool vinyl_init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Failed to initialize SDL: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf( "Failed to initialize SDL_image: %s\n", IMG_GetError() );
            success = false;
            SDL_Quit();
        }
    }
    return success;
}


void vinyl_exit()
{
    IMG_Quit();
    SDL_Quit();
}
