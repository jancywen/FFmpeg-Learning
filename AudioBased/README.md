### 音频基础


三元组:
 * 采样率 sample rate
 * 通道 channels
 * 采样大小 位深

帧率：
stream nb_samples
aac: 1024
mp3: 1152

frame_duration = nb_samples * 1000 / sample_rate  ms

AAC一帧的播放时间是= 1024*1000/44100= 22.32ms (单位为ms)    22ms
MP3一帧的播放时间是= 1152*1000/44100= 26.122msms (单位为ms)     26ms

### format context
* 打开文件创建上下文
avformat_open_input
* 读取流信息
avformat_find_stream_info
* 打印信息
av_dump_format
* 寻找流的索引
av_find_best_stream

* 释放资源
avformat_close_input

### codec context
* 解码器上下文
avcodec_alloc_context3
* copy 参数
avcodec_parameters_to_context
* 解码器
avcodec_find_decoder
* 打开解码器
avcodec_open2
* 解码
avcodec_decode_audio4
* 释放资源
avcodec_free_context

### swresample
* 初始化上下文 设置参数
swr_alloc
swr_alloc_set_opts
swr_init
* 重采样
swr_convert

* 获取 channel layout
av_get_default_channel_layout

### avio
* 打开文件 上下文
avio_open2
* 写入
avio_write
* 释放资源
avio_close

编译：
```
clang -g -o resample resample.c `pkg-config --libs libavformat, libavcodec, libavutil, libswresample`
```