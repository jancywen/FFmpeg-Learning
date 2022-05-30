#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <unistd.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

int main(int argc, char const *argv[])
{
    
    AVFormatContext *fmt = NULL;
    AVCodec *codec = NULL;
    AVCodecContext *ctx = NULL;
    int ret = 0;

    const char * in_filename = NULL, *out_filename = NULL;
    int audio_index;

    AVPacket pkt;


    struct SwrContext *swr = NULL;


    AVIOContext *avioctx = NULL;

    if (argc < 3)
    {
        return -1;
    }

   
    in_filename = argv[1];
    out_filename = argv[2];


    if ((ret = avformat_open_input(&fmt, in_filename, NULL, NULL))< 0 ) {
        printf("can't open input file: %s\n", in_filename);
        return -1;
    }

    if ((ret = avformat_find_stream_info(fmt, NULL)) < 0) {
        printf("find stream info error\n");
        goto end;
    }

    av_dump_format(fmt,0, in_filename, 0);

    audio_index = av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_index < 0)
    {
        printf("can't find audio stream index\n");
        goto end;
    }

    printf("find audio stream index: %d \n", audio_index);
    

    
    
    ctx = avcodec_alloc_context3(NULL);
    if (!ctx)
    {
        printf("alloc codec ctx error\n");
        goto end;
    }
    
    ret = avcodec_parameters_to_context(ctx, fmt->streams[audio_index]->codecpar);
    if (ret < 0) {
        printf("set codec ctx parameters error\n");
        goto end;
    }

    codec = avcodec_find_decoder(fmt->streams[audio_index]->codecpar->codec_id);
    if (!codec)
    {
        printf("find decoder error\n");
        goto end;
    }

    

    uint64_t in_layout = av_get_default_channel_layout(ctx->channels);
    
    int out_channels = 2;
    uint64_t out_layout = av_get_default_channel_layout(out_channels);
    uint64_t out_sample_rate = 44100;


    swr = swr_alloc();
    if (swr)
    {
        swr_alloc_set_opts(swr,
                            out_layout, 
                            AV_SAMPLE_FMT_S16, 
                            out_sample_rate, 
                            in_layout,
                            ctx->sample_fmt, 
                            ctx->sample_rate, 
                            0, NULL);
    }
    swr_init(swr);

    if ((ret = avcodec_open2(ctx, codec, NULL)) < 0) {
        printf("open codec error\n");
        goto end;
    }

    ret = avio_open2(&avioctx, out_filename,AVIO_FLAG_WRITE, NULL, NULL); 
    if (ret < 0) {
        printf("open avio error\n");
        goto end;
    }

    static AVFrame frame;
    // static uint8_t audio_buf[MAX_AUDIO_FRAME_SIZE *  2];
    uint8_t *out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);

    //Out Buffer Size
    int out_buffer_size = av_samples_get_buffer_size(NULL,
                                                     out_channels,
                                                     ctx->frame_size,
                                                     AV_SAMPLE_FMT_S16,
                                                     1);

    while (1)
    {
        ret = av_read_frame(fmt, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            usleep(100);
            continue;
        }else if (ret < 0) {
            break;
        }

        printf("pkt stream index: %d\n", pkt.stream_index);

        if (pkt.stream_index != audio_index) {
            av_packet_unref(&pkt);
            continue;
        }

        int got_frame;
        int len;
        len = avcodec_decode_audio4(ctx, &frame, &got_frame, &pkt);
        printf("len: %d; pkt data size: %d\n, get frame: %d\n", len, pkt.size, got_frame);

        if (len < 0) {
            printf("decode error\n");
            break;
        }

        if (got_frame) {
            swr_convert(swr, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame.data, frame.nb_samples);
            printf("convert\n");
            avio_write(avioctx, out_buffer, out_buffer_size);
            printf("sizeof(out_buffer): %ld\n", sizeof(out_buffer));
            printf("out buffer size: %d\n", out_buffer_size);
            printf("write\n");
        }
        
        
        
    }
    
 end:   
    avformat_close_input(&fmt);

    avio_close(avioctx);

    avcodec_free_context(&ctx);

    return ret;
}
