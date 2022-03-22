
#include <stdio.h>

#include <SDL2/SDL.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

int main(int argc, char const *argv[])
{
    
    const char *filepath;

    AVFormatContext *fmt = NULL;
    int video_index;
    int ret = 0;

    AVCodec *decoder = NULL;
    AVCodecContext *c = NULL;

    int v_width, v_height;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Rect rect;

    AVPacket pkt;
    AVFrame *frame = NULL;

    struct SwsContext *sws = NULL;
    int got_picture_ptr;

    if (argc < 2)
    {
        return -1;
    }

    filepath = argv[1];
    

    if ((ret = avformat_open_input(&fmt, filepath, NULL, NULL)) < 0) 
    {
        printf("open input error: %s\n", av_err2str(ret));
        return -1;
    }
    printf("open input\n");
    if ((ret = avformat_find_stream_info(fmt, NULL)) < 0) 
    {
        printf("find stream info error: %s", av_err2str(ret));
        goto __EXIT;
    }
    printf("find stream\n");

    av_dump_format(fmt, -1, filepath, 0);

    if ((video_index = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0)) < 0)
    {
        printf("can't find video index\n");
        goto __EXIT;
    }
    printf("find video index: %d\n", video_index);
    

    decoder = avcodec_find_decoder(fmt->streams[video_index]->codecpar->codec_id);
    if (!decoder)
    {
        printf("failed find decoder by codec_id \n");
        goto __EXIT;
    }
    printf("success find decoder\n");


    c = avcodec_alloc_context3(decoder);
    if (!c)
    {
        printf("failed alloc c");
        goto __EXIT;
    }
    printf ("success alloc c");

    if ((ret = avcodec_parameters_to_context(c, fmt->streams[video_index]->codecpar))< 0)
        goto __EXIT;
    printf("set parameters\n");

    if ((ret = avcodec_open2(c, decoder, NULL)) < 0 )
        goto __EXIT;
    printf("open decoder\n");

    v_width = c->width;
    v_height = c->height;


    sws = sws_getContext(v_width, v_height, c->pix_fmt, v_width, v_height, AV_PIX_FMT_YUV420P,SWS_BILINEAR, NULL, NULL,NULL);
    if (!sws)
    {
        printf("get sws error\n");
        goto __EXIT;
    }
    
    frame = av_frame_alloc();
    if (!frame)
        goto __EXIT;


//   AVPicture *pict = (AVPicture*)malloc(sizeof(AVPicture));
//   avpicture_alloc(pict, 
// 		  AV_PIX_FMT_YUV420P, 
// 		  c->width, 
// 		  c->height);
    
    AVFrame *pict = av_frame_alloc();
    av_image_alloc(pict->data, pict->linesize, v_width, v_height, AV_PIX_FMT_YUV420P, 1);
    


    SDL_Init(SDL_INIT_AUDIO);

    window = SDL_CreateWindow("video player", 0, 0, 320, 180, SDL_WINDOW_METAL);
    if (!window)    
        goto __EXIT;
    printf("window\n");
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
        goto __EXIT;
printf("renderer\n");
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, v_width, v_height);
    if (!texture)
        goto __EXIT;
printf("texture\n");
    

    av_init_packet(&pkt);

    while (av_read_frame(fmt, &pkt) >= 0)
    {
        if (pkt.stream_index == video_index)
        {
            printf("read video stream\n");
            avcodec_decode_video2(c, frame, &got_picture_ptr, &pkt);
            printf("got pic: %d\n", got_picture_ptr);

            if (got_picture_ptr)
            {


                 // Convert the image into YUV format that SDL uses
                // int height = sws_scale(sws, (uint8_t const * const *)frame->data,
                //         frame->linesize, 0, c->height,
                //         pict->data, pict->linesize);
                // printf("height: %d\n", height);

                // SDL_UpdateYUVTexture(texture, NULL, 
                //         pict->data[0], pict->linesize[0],
                //         pict->data[1], pict->linesize[1],
                //         pict->data[2], pict->linesize[2]);

                int height = sws_scale(sws, (const uint8_t *const *)frame->data, frame->linesize,0,v_height, pict->data, pict->linesize);
                printf("height: %d\n", height);
                SDL_UpdateTexture(texture, NULL, pict->data[0], v_width);


                rect.x = 0;
                rect.y = 0;
                rect.w = 320;
                rect.h = 180;

                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_RenderPresent(renderer);
            }
            
        }
        
        av_packet_unref(&pkt);

        SDL_Event event;
        SDL_PollEvent(&event);
        switch(event.type) {
        case SDL_QUIT:
            goto __EXIT;
        break;
            default:
        break;
        }
    }
    

__EXIT:
    avformat_close_input(&fmt);

    if (frame)
    {
        av_frame_free(&frame);
    }

    if (pict)
        av_frame_free(&pict);
    

    if (c)
        avcodec_free_context(&c);

    if (window)
        SDL_DestroyWindow(window);
    
    if (renderer)
        SDL_DestroyRenderer(renderer);
    
    if (texture)
        SDL_DestroyTexture(texture);

    SDL_Quit();

    return 0;
}
