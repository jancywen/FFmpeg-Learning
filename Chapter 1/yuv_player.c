#include <SDL2/SDL.h>


#define BLOCK_SIZE 1200000

#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define END_EVENT (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_timer(void *data)
{

    while (!thread_exit)
    {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    
    thread_exit = 0;
    SDL_Event event;
    event.type = END_EVENT;
    SDL_PushEvent(&event);

    return 0;
}



int main(int argc, char const *argv[])
{
    
    FILE *videofile = NULL;
    char *filepath = "/Users/wangwenjie/Jancy/material/video_328.yuv";

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    SDL_Thread *thread = NULL;
    SDL_Event event;
    SDL_Rect rect;

    int w_w = 320, w_h = 180;
    const int v_w = 328, v_h = 216;
    const int video_frame_len = v_w * v_h *1.5;

    Uint8 video_buf[BLOCK_SIZE];
    Uint8 *video_pos, *video_end;
    int video_buf_len, blank_spacing_len;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("龙门石窟", 0, 0, 320, 180, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (!window)
        goto end;
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
        goto end;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, v_w, v_h);
    if (!texture)
        goto end;

    
    thread = SDL_CreateThread(thread_timer, NULL, NULL);
    
    videofile = fopen(filepath, "r");
    if (!videofile)
        goto end;

    video_buf_len = fread(video_buf, 1, BLOCK_SIZE, videofile);
    if (video_buf_len <= 0)
        goto end;
    
    video_pos = video_buf;
    video_end = video_buf + video_buf_len;
    blank_spacing_len = BLOCK_SIZE - video_buf_len;

    do
    {
        SDL_WaitEvent(&event);
        if (event.type == REFRESH_EVENT)
        {
            
            if ((video_pos + video_frame_len) > video_end)
            {
                
            }
            

            SDL_UpdateTexture(texture, NULL, video_pos, v_w);

            rect.x = 0;
            rect.y = 0;
            rect.w = w_w;
            rect.h = w_h;

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_RenderPresent(renderer);

            video_pos += video_frame_len;

        }else if (event.type == SDL_WINDOWEVENT)
        {
            SDL_GetWindowSize(window, &w_w, &w_h);
        }else if (event.type == SDL_QUIT)
        {
            
        }else if (event.type == END_EVENT) {
            thread_exit = 1;
        }
        
        
        
    } while (1);
    


end:

    if (videofile)
        fclose(videofile);

    if (texture)
        SDL_DestroyTexture(texture);
    

    if(renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);
    
    
    SDL_Quit();


    return 0;
}
