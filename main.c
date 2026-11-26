/*
    helpful references :
    https://ffmpeg.org/doxygen/trunk/examples.html (start with this)
    https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html
    https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html
    https://dranger.com/ffmpeg/tutorial01.html
    demux_decode.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define cellheight 20
#define cellwidth 10

AVFormatContext *fmt_ctx = NULL;
AVCodecContext *video_dec_ctx = NULL;
const AVCodec *dec = NULL;
AVStream *st = NULL;
AVFrame *frame = NULL;
AVPacket *pkt = NULL;
enum AVPixelFormat pix_fmt;
enum AVMediaType type;
int video_stream_idx = -1;
int width, height, stream_idx;


int main(int argc, char **argv)
{
    int res = 0;
    if (argc < 2)
    {
        printf("give a video to play\n");
        return 1;
    }
    char *filename = argv[1];
    res = avformat_open_input(&fmt_ctx, filename, NULL, NULL);
    if (res < 0)
    {
        printf("error opening the file");
        return 2;
    }
    res = avformat_find_stream_info(fmt_ctx, NULL);
    if (res < 0)
    {
        printf("error finding stream info\n");
        return 3;
    }
    res = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (res < 0)
    {
        printf("couldn't find %s stream in input file\n", av_get_media_type_string(type));
        return 4;
    }
    video_stream_idx = res;
    st = fmt_ctx -> streams[stream_idx];
    dec = avcodec_find_decoder(st -> codecpar -> codec_id);
    video_dec_ctx = avcodec_alloc_context3(dec);
    res = avcodec_parameters_to_context(video_dec_ctx, st -> codecpar);
    if (res < 0)
    {
        printf("couldn't copy codec parameters to decoder context\n");
        return 5;
    }
    res = avcodec_open2(video_dec_ctx, dec, NULL);
    if (res < 0)
    {
        printf("failed to open codec\n");
        return 6;
    }
    width = video_dec_ctx -> width;
    height = video_dec_ctx -> height;
    pix_fmt = video_dec_ctx -> pix_fmt;
    frame = av_frame_alloc();
    pkt = av_packet_alloc();
    if (!pkt)
    {
        printf("couldn't allocate packet\n");
        return 7;
    }
    printf("pkt -> stream_index = %d\n", pkt -> stream_index);
    printf("video_stream_idx = %d\n", video_stream_idx);
    int c = 0;
    int twidth = width / cellwidth;
    int theight = height / cellheight;
    char map[14] = {'.', ',', ':', ';', '-', '=', '+', 'x', 'X', '8', '0', 'S', '#', '@'};
    while (res = av_read_frame(fmt_ctx, pkt) >= 0)
    {
        if (pkt -> stream_index == video_stream_idx)
        {
            avcodec_send_packet(video_dec_ctx, pkt);
            // ascii rendering inside this if statement
            if (avcodec_receive_frame(video_dec_ctx, frame) >= 0)
            {
                int linesize = frame -> linesize[0];
                // brightness values of a frame (per pixel)
                for (int i = 0; i < theight; i++)
                {
                    int x0 = i * height / theight;
                    int x1 = (i + 1) * height / theight;
                    char row[twidth + 2];
                    for (int j = 0; j < twidth; j++)
                    {
                        int y0 = j  * width / twidth;
                        int y1 = (j + 1) * width / twidth;
                        long sum = 0;
                        int count = 0;
                        //printf("makes it here\n");
                        for (int x = x0; x < x1; x++)
                        {
                            for (int y = y0; y < y1; y++)
                            {
                                if (x >= height || y >= width)
                                {
                                    printf("frame bounds disrespected\n");
                                    printf("x = %d, y = %d\n", x, y);
                                    exit(1);
                                }
                                sum += frame -> data[0][x * linesize + y];
                                count++;
                            }
                        }
                        //printf("count = %d\n", count);
                        sum = sum / count;
                        row[j] = map[(sum * 13 + 127) / 255];
                    }
                    row[twidth] = '\n';
                    row[twidth + 1] = '\0';
                    printf("%s\n", row);
                    fputs(row, stdout);
                }
                break;
            }
        }
    }
    av_packet_unref(pkt);
}