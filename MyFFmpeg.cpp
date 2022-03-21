#include "MyFFmpeg.h"

std::string getAVErrStrLog(int avErrNo)
{
	char errbuf[1024] = { 0 };
	av_strerror(avErrNo, errbuf, sizeof(errbuf) - 1);
	std::string retStr = "[";
	retStr += __FILE__;
	retStr += " ";
	retStr += __FUNCTION__;
	retStr += std::to_string(__LINE__);
	retStr += "] ";
	retStr += errbuf;
	return retStr;
}

NewFFmpeg::NewFFmpeg()
{}
NewFFmpeg::NewFFmpeg(std::string path)
{
	m_filePath = path;
}
NewFFmpeg::~NewFFmpeg()
{
}

int NewFFmpeg::Open(int dstW, int dstH)
{
	m_isStop = true;

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	//重启
	Clear();
	Close();

	int avRet = 0;
	if (m_filePath.empty())
	{
		//std::cout << "filePath Err!!" << std::endl;
		return -1;
	}
	//优先选用传入的参数
	std::string pathTemp = m_filePath;

	do {

		if (m_pFormatCtx)
		{
			avformat_free_context(m_pFormatCtx);
			m_pFormatCtx = nullptr;
		}

		AVDictionary* opts = nullptr;
		// -设置rtsp流已tcp协议打开
		av_dict_set(&opts, "rtsp_transport", "tcp", 0);
		// -网络延时时间(访问外网时设置，如果是内网时还好)
		av_dict_set(&opts, "max_delay", "5000000", 0);

		avformat_network_init();
		m_pFormatCtx = avformat_alloc_context();
		//open_input
		if (avformat_open_input(&m_pFormatCtx, pathTemp.c_str(), NULL, &opts) != 0)
		{
			
			return -1;
		}
		//find_stream_infomation
		if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
		{
			return -1;
		}

		//find_decoder
		m_videoIndex = -1;
		m_audioIndex = -1;
		for (int i = 0; i < m_pFormatCtx->nb_streams; ++i)
		{
			if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				m_videoIndex = i;
			}
			if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				m_audioIndex = i;
			}
		}
		if (m_videoIndex != -1)
		{
			m_pCodercVideo = avcodec_find_decoder((enum AVCodecID)(m_pFormatCtx->streams[m_videoIndex]->codecpar->codec_id));
			if (m_pCodercVideo == NULL)
			{
				printf("Codec not found.\n");
				return -1;
			}

			m_pCodercCtxVideo = avcodec_alloc_context3(m_pCodercVideo);
			if (m_pCodercCtxVideo == nullptr)
			{
				printf("Codec not found.\n");
				return -1;
			}
		}
		if (m_audioIndex != -1)
		{
			m_pCodercAudio = avcodec_find_decoder((enum AVCodecID)(m_pFormatCtx->streams[m_audioIndex]->codecpar->codec_id));
			if (m_pCodercAudio == NULL)
			{
				printf("Codec not found.\n");
				return -1;
			}

			m_pCodercCtxAudio = avcodec_alloc_context3(m_pCodercAudio);
			if (m_pCodercCtxAudio == nullptr)
			{
				printf("Codec not found.\n");
				return -1;
			}
			//
		}

		//open2 video
		avRet = avcodec_parameters_to_context(m_pCodercCtxVideo, m_pFormatCtx->streams[m_videoIndex]->codecpar);
		if (avRet < 0)
		{
			printf("%s\n", getAVErrStrLog(avRet).c_str());
			return -1;
		}
		avRet = avcodec_open2(m_pCodercCtxVideo, m_pCodercVideo, NULL);
		if (avRet != 0)
		{
			printf("%s\n", getAVErrStrLog(avRet).c_str());
			return -1;
		}
		//open2 audio
		avRet = avcodec_parameters_to_context(m_pCodercCtxAudio, m_pFormatCtx->streams[m_audioIndex]->codecpar);
		if (avRet < 0)
		{
			printf("%s\n", getAVErrStrLog(avRet).c_str());
			return -1;
		}
		avRet = avcodec_open2(m_pCodercCtxAudio, m_pCodercAudio, NULL);
		if (avRet != 0)
		{
			printf("%s\n", getAVErrStrLog(avRet).c_str());
			return -1;
		}

		//video convert
		//m_pImgConvertCtx = sws_getContext(m_pCodercCtxVideo->width, m_pCodercCtxVideo->height, (enum AVPixelFormat)(m_pCodercCtxVideo->pix_fmt),\
			m_pCodercCtxVideo->width, m_pCodercCtxVideo->height, AV_PIX_FMT_YUV420P, 4, NULL, NULL, NULL);
		m_pImgConvertCtx = sws_getContext(m_pCodercCtxVideo->width, m_pCodercCtxVideo->height, (enum AVPixelFormat)(m_pCodercCtxVideo->pix_fmt),\
			dstW, dstH, AV_PIX_FMT_YUV420P, 4, NULL, NULL, NULL);

		//resample
		m_pSwrCxt = swr_alloc_set_opts(nullptr,	// m_pSwrCxt为 nullptr 会分配空间
			av_get_default_channel_layout(2),	//输出声道格式
			(AVSampleFormat)m_outFormat,		//输出采样格式
			m_pCodercCtxAudio->sample_rate,			//输出采样率
			av_get_default_channel_layout(m_pCodercCtxAudio->channels),	//输入声道格式
			(AVSampleFormat)m_pCodercCtxAudio->sample_fmt,				//输入采样格式
			m_pCodercCtxAudio->sample_rate,			//输入采样率
			0, 0
		);
		swr_init(m_pSwrCxt);

		std::cout << "DEBUG+++++++++++++++++++++++++++++++++++" << std::endl;
		av_dump_format(m_pFormatCtx, m_videoIndex, m_filePath.c_str(), 0);
		av_dump_format(m_pFormatCtx, m_audioIndex, m_filePath.c_str(), 0);
		std::cout << "DEBUG+++++++++++++++++++++++++++++++++++" << std::endl;
	} while (false);
	

	if (avRet == -1)
	{
		Close();
	}

	return 0;
}

AVPacket* NewFFmpeg::ReadPacket()
{
	m_pPacket = av_packet_alloc();

	int avRet = 0;
	char errBuf[1024] = { 0 };
	avRet = av_read_frame(m_pFormatCtx, m_pPacket);
	if (avRet != 0)
	{
		av_strerror(avRet, errBuf, sizeof(errBuf) - 1);
		std::cout << "av_read_frame err:[" << errBuf << std::endl;
		return nullptr;
	}
	return m_pPacket;
}

int NewFFmpeg::SendPacket()
{
	if (m_pPacket == nullptr)
	{
		return -1;
	}

	int avRet = 0;
	char errBuf[1024] = { 0 };
	//视频
	if (m_pPacket->stream_index == m_videoIndex)
	{
		m_CurrentIndex = m_videoIndex;
		avRet = avcodec_send_packet(m_pCodercCtxVideo, m_pPacket);
		if (avRet != 0)
		{
			av_strerror(avRet, errBuf, sizeof(errBuf) - 1);
			std::cout << "avcodec_send_packet err:[" << errBuf << std::endl;
			av_packet_free(&m_pPacket);
			return -1;
		}
	}
	//音频
	else if (m_pPacket->stream_index == m_audioIndex)
	{
		m_CurrentIndex = m_audioIndex;
#if 1
		avRet = avcodec_send_packet(m_pCodercCtxAudio, m_pPacket);
		if (avRet != 0)
		{
			av_strerror(avRet, errBuf, sizeof(errBuf) - 1);
			std::cout << "avcodec_send_packet err:[" << errBuf << std::endl;
			av_packet_free(&m_pPacket);
			return -1;
		}
#endif
	}

	av_packet_free(&m_pPacket);

	return 0;
}

AVFrame* NewFFmpeg::ReciveFrame()
{
	m_pFrame = av_frame_alloc();

	int avRet = 0;
	char errBuf[1024] = {0};
	
	//视频
	if (m_CurrentIndex == m_videoIndex)
	{
		avRet = avcodec_receive_frame(m_pCodercCtxVideo, m_pFrame);
		if (avRet != 0)
		{
			av_strerror(avRet, errBuf, sizeof(errBuf) - 1);
			//std::cout << "avcodec_receive_frame err:[" << errBuf << std::endl;
			av_frame_free(&m_pFrame);
			return nullptr;
		}

		if (m_pFrame)
		{
			//std::cout << "pFrame: pts[" << m_pFrame->pts << "] dts[" << m_pFrame->pkt_dts << "] type[" << (int)(m_pFrame->pict_type) << "] [" << m_pFrame->width << "x"\
				<< m_pFrame->height << "]" << std::endl;
		}
	}
	//音频
	else if(m_CurrentIndex == m_audioIndex)
	{
#if 1
		avRet = avcodec_receive_frame(m_pCodercCtxAudio, m_pFrame);
		if (avRet != 0)
		{
			av_strerror(avRet, errBuf, sizeof(errBuf) - 1);
			//std::cout << "avcodec_receive_frame err:[" << errBuf << std::endl;
			av_frame_free(&m_pFrame);
			return nullptr;
		}
#endif
		//av_frame_free(&m_pFrame);
	}

	return m_pFrame;
}

int NewFFmpeg::Clear()
{
	//清理残余数据
	return 0;
}

int NewFFmpeg::Close()
{
	if (m_pImgConvertCtx)
	{
		sws_freeContext(m_pImgConvertCtx);
		m_pImgConvertCtx = nullptr;
	}
#if 0
	if (m_pFrameYUV)
	{
		av_frame_free(&m_pFrameYUV);
	}
	if (m_pFrame)
	{
		av_frame_free(&m_pFrame);
	}
#endif
	if (m_pCodercCtxVideo)
	{
		avcodec_close(m_pCodercCtxVideo);
		m_pCodercCtxVideo = nullptr;
	}
	if (m_pSwrCxt)
	{
		swr_free(&m_pSwrCxt);
	}
	if (m_pCodercCtxAudio)
	{
		avcodec_close(m_pCodercCtxAudio);
		m_pCodercCtxAudio = nullptr;
	}
	if (m_pFormatCtx)
	{
		avformat_close_input(&m_pFormatCtx);
	}

	return 0;
}
