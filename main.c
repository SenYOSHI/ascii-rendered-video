/*
    helpful references :
    https://ffmpeg.org/doxygen/trunk/examples.html (start with this)
    https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html
    https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html
    https://dranger.com/ffmpeg/tutorial01.html
    demux_decode.c
    general flow :
        MP4 file
            ↓
        AVFormatContext
            ↓
        AVPacket
            ↓
        AVCodecContext
            ↓
        AVFrame
            ↓
        your pixel processing
    
    core loop :
        Read packet
            ↓
        Send packet to decoder
            ↓
        Receive decoded frame(s)
            ↓
        Edit pixels
*/

#include <stdio.h>
#include <stdlib.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
