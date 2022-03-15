

#include <SDL2/SDL.h>


#define BLOCK_SIZE 4096000

static Uint8 *audio_buf = NULL;
static Uint8 *audio_pos = NULL;
static Uint8 *audio_end = NULL;
static size_t audio_buf_len = 0;


void read_audio_data(void *udata, Uint8 *stream, int len) 
{
    if (audio_buf_len == 0)
    {
        printf("audio_buf_len\n");
        return;
    }

    SDL_memset(stream, 0, len);

    len = (len < audio_buf_len) ? len : audio_buf_len;
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

    audio_pos += len;
    audio_buf_len -= len;
    
}

int main(int argc, char const *argv[])
{

    FILE *audio_fp = NULL;
    const char *path = "./top10.pcm";

    SDL_AudioSpec spec;

    int ret = 0;

    if (SDL_Init(SDL_INIT_AUDIO))
    {
        return -1;
    }

    audio_fp = fopen(path, "r");
    if (!audio_fp)
    {
        goto __FAIL;
    }
    

    audio_buf = (Uint8 *)malloc(BLOCK_SIZE);
    if (!audio_buf) {
        goto __FAIL;
    }
    spec.freq = 44100;
    spec.channels = 2;
    spec.format = AUDIO_S16SYS;
    spec.callback = read_audio_data;
    spec.userdata = NULL;
    if (SDL_OpenAudio(&spec, NULL)) {
        goto __FAIL;
    }

    SDL_PauseAudio(0);

    do
    {
        
        audio_buf_len = fread(audio_buf, 1, BLOCK_SIZE, audio_fp);
        audio_pos = audio_buf;
        audio_end = audio_buf + audio_buf_len;
        while (audio_pos < audio_end)
        {
            SDL_Delay(1);
        }
        
    } while (audio_buf_len != 0);
    
    SDL_CloseAudio();


__FAIL:

    if(audio_buf)
        free(audio_buf);

    
    if (audio_fp)
        fclose(audio_fp);

    SDL_Quit();
    return ret;
}
