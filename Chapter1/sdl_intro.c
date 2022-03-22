
#include <SDL2/SDL.h>


int main(int argc, char const *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    SDL_Event event;

    SDL_Rect rect;
    rect.x = rand() % 640;
    rect.y = rand() %480;
    rect.w = 64;
    rect.h = 48;

    int w_width = 640, w_height = 480;

    SDL_Init(SDL_INIT_VIDEO);


    window = SDL_CreateWindow("title", 0, 0, w_width, w_height, SDL_WINDOW_METAL);
    if (!window)
        goto __EXIT;
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
        goto __EXIT;

    SDL_SetRenderDrawColor(renderer, 160, 160, 160,255);
    SDL_RenderClear(renderer);

    // SDL_RenderDrawRect(renderer, &rect);
    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_RenderFillRect(renderer, &rect);
    // SDL_RenderPresent(renderer);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, w_width, w_height);
    if (!texture)
        goto __EXIT;

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255,0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);

    

    do
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
        {
            break;
        }
        
    } while (1);
    

__EXIT:

    if (texture)
        SDL_DestroyTexture(texture);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window) 
        SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
