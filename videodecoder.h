/*
THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __VIDEODECODER_H
#define __VIDEODECODER_H

#include <QObject>
#include <QIODevice>
#include <QFile>
#include <QImage>

#include "ffmpeg.h"

class VideoDecoder : public QObject
{
    Q_OBJECT

public:
    explicit VideoDecoder(void);
    explicit VideoDecoder(QString file);
    ~VideoDecoder();
    bool openFile(const char* file);
    void close();
    bool getFrame(QImage& img, int* effectiveframenumber = 0, int* effectiveframetime = 0, int* desiredframenumber = 0, int* desiredframetime = 0);
    bool seekNextFrame(int skip = 1);
    bool seekMs(int ts);
    bool seekFrame(int64_t frame);
    int getVideoLengthMs();
    bool isOk() const;
    QSize frameSize(void) const;
    int frameNumber(void) const  { return LastFrameNumber; }
    int ms(void) const { return Ms; }

protected:
    // Basic FFmpeg stuff
    ffmpeg::AVFormatContext* pFormatCtx;
    int videoStream;
    ffmpeg::AVCodecContext* pCodecCtx;
    ffmpeg::AVCodec* pCodec;
    ffmpeg::AVFrame* pFrame;
    ffmpeg::AVFrame* pFrameRGB;
    ffmpeg::AVPacket packet;
    ffmpeg::SwsContext* img_convert_ctx;
    uint8_t* buffer;
    int numBytes;

    // State infos for the wrapper
    bool ok;
    QImage LastFrame;
    int LastFrameTime, LastLastFrameTime, LastLastFrameNumber, LastFrameNumber;
    int DesiredFrameTime, DesiredFrameNumber;
    bool LastFrameOk; // Set upon start or after a seek we don't have a frame yet
    int Ms;

    // Initialization functions
    virtual bool initCodec();
    virtual void InitVars();

    // Helpers
    virtual void dumpFormat(ffmpeg::AVFormatContext *ic, int index, const char *url, int is_output);
    virtual void saveFramePPM(ffmpeg::AVFrame *pFrame, int width, int height, int iFrame);

    // Seek
    virtual bool decodeSeekFrame(int after);

};

#endif // __VIDEODECODER_H
