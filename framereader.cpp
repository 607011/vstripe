extern "C" {
#include <avcodec.h>
#include <avformat.h>
}
#include <stdio.h>


bool GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx,
    int videoStream, AVFrame *pFrame)
{
    static AVPacket packet;
    static int      bytesRemaining=0;
    static uint8_t  *rawData;
    static bool     fFirstTime=true;
    int             bytesDecoded;
    int             frameFinished;

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if(fFirstTime)
    {
        fFirstTime=false;
        packet.data=NULL;
    }

    // Decode packets until we have decoded a complete frame
    while(true)
    {
        // Work on the current packet until we have decoded all of it
        while(bytesRemaining > 0)
        {
            // Decode the next chunk of data
            bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame,
                &frameFinished, rawData, bytesRemaining);

            // Was there an error?
            if(bytesDecoded < 0)
            {
                fprintf(stderr, "Error while decoding frame\n");
                return false;
            }

            bytesRemaining-=bytesDecoded;
            rawData+=bytesDecoded;

            // Did we finish the current frame? Then we can return
            if(frameFinished)
                return true;
        }

        // Read the next packet, skipping all packets that aren't for this
        // stream
        do
        {
            // Free old packet
            if(packet.data!=NULL)
                av_free_packet(&packet);

            // Read new packet
            if(av_read_packet(pFormatCtx, &packet)<0)
                goto loop_exit;
        } while(packet.stream_index!=videoStream);

        bytesRemaining=packet.size;
        rawData=packet.data;
    }

loop_exit:

    // Decode the rest of the last frame
    bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished,
        rawData, bytesRemaining);

    // Free last packet
    if(packet.data!=NULL)
        av_free_packet(&packet);

    return frameFinished!=0;
}
