/*
THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __VIDEODECODER_H_
#define __VIDEODECODER_H_

#include <QObject>
#include <QIODevice>
#include <QFile>
#include <QImage>

#include "abstractvideodecoder.h"

#include "ffmpeg.h"



class VideoDecoder : public IAbstractVideoDecoder
{
    Q_OBJECT

public:
    explicit VideoDecoder(void);
    explicit VideoDecoder(QString file);
    ~VideoDecoder();
    int getVideoLengthMs(void);
    int getDefaultSkip(void) const { return mDefaultSkip; }
    inline bool isOk(void) const { return mOk; }
    QSize frameSize(void) const;
    ffmpeg::AVFormatContext* formatCtx(void) const { return mFormatCtx; }
    ffmpeg::AVCodecContext* codecCtx(void) const { return mCodecCtx; }
    ffmpeg::AVCodec* codec(void) const { return mCodec; }
    bool decodeOk(void) const { return mFormatCtx != NULL && mCodecCtx != NULL && mCodec != NULL; }

    virtual bool open(const char* file);
    virtual bool open(int deviceId);
    virtual void close(void);
    virtual bool getFrame(QImage& img, int* effectiveframenumber = 0, int* effectiveframetime = 0, int* desiredframenumber = 0, int* desiredframetime = 0);
    virtual bool seekNextFrame(int skip = 0);
    virtual bool seekMs(int);
    virtual bool seekFrame(qint64);

    virtual QString codecInfo(void) const;
    virtual const QString typeName(void) const { return "VideoFile"; }


private: // variables
    // Basic FFmpeg stuff
    int mVideoStream;
    ffmpeg::AVFormatContext* mFormatCtx;
    ffmpeg::AVCodecContext* mCodecCtx;
    ffmpeg::AVCodec* mCodec;
    ffmpeg::AVFrame* mFrame;
    ffmpeg::AVFrame* mFrameRGB;
    ffmpeg::AVPacket mPacket;
    ffmpeg::SwsContext* mImgConvCtx;
    quint8* mBuf;
    int mByteCount;

    // State infos for the wrapper
    bool mOk;
    QImage mLastFrame;
    int mLastFrameTime, mLastLastFrameTime, mLastLastFrameNumber, mLastFrameNumber;
    int mDesiredFrameTime, mDesiredFrameNumber;
    bool mLastFrameOk; // Set upon start or after a seek we don't have a frame yet

private: // methods
    bool initCodec();
    void initVars();

    bool decodeSeekFrame(int after);

};

#endif // __VIDEODECODER_H_
