#pragma once

#include<string>
#include<iostream>
#include<cstdio>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/error.h"
#include "libavutil/avutil.h"
#include "libavutil/dict.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}

#include<memory>
#include <thread>
#include <chrono>

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

class NewFFmpeg
{
public:
	NewFFmpeg();
	NewFFmpeg(std::string path);
	void SetUrl(std::string url) { m_filePath = url; }
	int Open(int dstW, int dstH);
	AVPacket* NewFFmpeg::ReadPacket();
	int NewFFmpeg::SendPacket();
	AVFrame* NewFFmpeg::ReciveFrame();
	int Clear();
	int Close();
	~NewFFmpeg();

//private:
public:
	std::string m_filePath{ "" };
	int m_videoIndex{ 0 };
	int m_audioIndex{ 0 };
	int m_CurrentIndex{ 0 };

	int m_outFormat{ AV_SAMPLE_FMT_S16 };

	bool m_isStop{ false };

	AVFormatContext* m_pFormatCtx{ nullptr };
	AVCodecContext* m_pCodercCtxVideo{ nullptr };
	AVCodecContext* m_pCodercCtxAudio{ nullptr };
	SwrContext* m_pSwrCxt{ nullptr };
	const AVCodec* m_pCodercVideo{ nullptr };
	const AVCodec* m_pCodercAudio{ nullptr };
	AVFrame* m_pFrame{ nullptr };
	AVFrame* m_pFrameYUV{ nullptr };
	AVPacket* m_pPacket{ nullptr };
	struct SwsContext* m_pImgConvertCtx{ nullptr };
};
