#include "QtSDLPlay.h"
#include "MyFFmpeg.h"

QtSDLPlay::QtSDLPlay(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void QtSDLPlay::closeEvent(QCloseEvent* event)
{
	this->m_newFFmpeg.m_isStop = true;
	qDebug() << "Close Windows";
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void QtSDLPlay::videoWidgetSetPos()
{
	int mainX = this->x();
	int mainY = this->y();
	int mainW = this->width();
	int mainH = this->height();

	int videoWideoW = mainW*2/3;
	int videoWideoH = mainH*2/3;

	int videoWideoX = (mainW - videoWideoW) / 2;
	int videoWideoY = mainX - ui.openFile->x() - ui.openFile->height() + 20;

	ui.videoWidget->resize(videoWideoW, videoWideoH);
	ui.videoWidget->move(videoWideoX, videoWideoY);

	m_rect.x = 0;
	m_rect.y = 0;
	m_rect.w = videoWideoW;
	m_rect.h = videoWideoH;

}

void QtSDLPlay::openAudioSDL()
{
	SDL_CloseAudio();

	m_sdlAudioSpec.freq = m_newFFmpeg.m_pCodercCtxAudio->sample_rate;
	//m_sdlAudioSpec.format = AUDIO_S16SYS;
	m_sdlAudioSpec.format = AUDIO_S16;
	//m_sdlAudioSpec.format = AUDIO_S16LSB;
	m_sdlAudioSpec.channels = m_newFFmpeg.m_pCodercCtxAudio->channels;
	m_sdlAudioSpec.silence = 0;
	m_sdlAudioSpec.samples = m_newFFmpeg.m_pCodercCtxAudio->frame_size;
	m_sdlAudioSpec.callback = ReadAudioData;
	m_sdlAudioSpec.userdata = this;

	int ret = SDL_OpenAudio(&m_sdlAudioSpec, nullptr);
	if (ret < 0)
	{
		std::cout << "SDL_OpenAudio ERR!" << std::endl;
		return;
	}

	SDL_PauseAudio(0);
}

uint8_t* pPCM[AV_NUM_DATA_POINTERS] = {new uint8_t[1024 * 1024 * 2], 0, 0, 0, 0, 0, 0, 0};
std::atomic<unsigned int> audioLen = 0;
unsigned char* audioChunk = NULL;
unsigned char* audioPos = NULL;

void ReadAudioData(void* udata, Uint8* stream, int len)
{
	SDL_memset(stream, 0, len);

	QtSDLPlay* pThis = (QtSDLPlay*)udata;
	//std::unique_lock<std::mutex> lck(pThis->m_pcmLenMtx);
	if (audioLen == 0)
	{
		//pThis->m_PCMLenCdt.notify_all();
		return;
	}
	// 填充数据
	//len = (len > audioLen ? audioLen : len);
	if (len > audioLen)
	{
		len = audioLen;
	}

	SDL_MixAudio(stream, audioPos, len, SDL_MIX_MAXVOLUME);
	audioPos += len;
	audioLen -= len;

	//pThis->m_PCMLenCdt.notify_all();
}

void QtSDLPlay::showSDLVideoWidget()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
	{
		std::cout << "SDL_Init Error:[" << SDL_GetError() << "]" << std::endl;
		return;
	}

	void* pVideoWidgetID = (void*)(ui.videoWidget->winId());	// 主窗口ID

	m_pSDLWin = SDL_CreateWindowFrom(pVideoWidgetID);
	if (m_pSDLWin == nullptr)
	{
		std::cout << "SDL_CreateWindow Error:[" << SDL_GetError() << "]" << std::endl;
		SDL_Quit();
		return;
	}

	m_pSDLRen = SDL_CreateRenderer(m_pSDLWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//SDL_Renderer* pRen = SDL_CreateRenderer(pWin, -1, 0);
	if (m_pSDLRen == nullptr)
	{
		SDL_DestroyWindow(m_pSDLWin);
		SDL_Quit();
		std::cout << "SDL_CreateRenderer Error:[" << SDL_GetError() << "]" << std::endl;
		return;
	}

#if 0
	std::string imagePath = "R.bmp";
	SDL_Surface* pBmp = SDL_LoadBMP(imagePath.c_str());
	if (pBmp == nullptr)
	{
		SDL_DestroyRenderer(pRen);
		SDL_DestroyWindow(pWin);
		SDL_Quit();
		std::cout << "SDL_LoadBMP Error:[" << SDL_GetError() << "]" << std::endl;
		return;
	}
#endif

	//SDL_ShowWindow(pWin); // 当 w 不执行 show 时必须设置SDL_ShowWindow,但是Qt界面的信息初始化不完整，例如x,y等
	Uint32 pixformat = SDL_PIXELFORMAT_IYUV;
	m_pSDLTxt = SDL_CreateTexture(m_pSDLRen, pixformat, SDL_TEXTUREACCESS_STREAMING, ui.videoWidget->width(), ui.videoWidget->height());
	if (m_pSDLTxt == nullptr)
	{
		SDL_DestroyRenderer(m_pSDLRen);
		SDL_DestroyWindow(m_pSDLWin);
		SDL_Quit();
		std::cout << "SDL_CreateTextureFromSurface Error:[" << SDL_GetError() << "]" << std::endl;
		return;
	}

	//std::cout << "videoWidget [" << ui.videoWidget->width() << "x" << ui.videoWidget->height() << "]" << std::endl;

#if 0
	//SDL_FreeSurface(pBmp);
	SDL_SetRenderDrawColor(m_pSDLRen, 0, 0, 0, 10);
	SDL_RenderClear(m_pSDLRen);
	SDL_RenderCopy(m_pSDLRen, m_pSDLTxt, NULL, NULL);
	SDL_RenderPresent(m_pSDLRen);
#endif
	//SDL_Delay(1000);
}

void QtSDLPlay::destroySDLVideoWidget()
{
	if (m_pSDLTxt)
	{
		SDL_DestroyTexture(m_pSDLTxt);
		m_pSDLTxt = nullptr;
	}
	if (m_pSDLRen)
	{
		SDL_DestroyRenderer(m_pSDLRen);
		m_pSDLRen = nullptr;
	}
	if (m_pSDLWin)
	{
		SDL_DestroyWindow(m_pSDLWin);
		m_pSDLWin = nullptr;
	}
	SDL_Quit();
}

void QtSDLPlay::SelectLocalFile()
{
    qDebug() << "SelectLocalFile Event!!";
    m_fileUrl = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择播放的文件"), QString::fromLocal8Bit("D://安防物联管理平台录像下载//20210622"));
    qDebug() << "Select File:" << m_fileUrl;

    //MyFFmpeg m_ffmpeg;
	//m_ffmpeg.initFFmpeg(m_fileUrl.toStdString());
	ui.filePath->setText(m_fileUrl);
}

void QtSDLPlay::OpenFile()
{
    qDebug() << "OpenFile Event!!";
	qDebug() << "GetFile URL:" << ui.filePath->text();

	m_newFFmpeg.SetUrl(ui.filePath->text().toStdString());
	m_newFFmpeg.Open(ui.videoWidget->width(), ui.videoWidget->height());

	int threadRet = 0;
	if (m_pAVThread1)
	{
		SDL_WaitThread(m_pAVThread1, &threadRet);
		m_pAVThread1 = nullptr;
	}

	if (m_pAVThread2)
	{
		SDL_WaitThread(m_pAVThread2, &threadRet);
		m_pAVThread2 = nullptr;
	}

	if (m_pAVThread3)
	{
		SDL_WaitThread(m_pAVThread3, &threadRet);
		m_pAVThread3 = nullptr;
	}

	while (m_YUVList.size() > 0)
	{
		AVFrame* pAVFrame = m_YUVList.front();
		if (pAVFrame != nullptr)
		{
			av_frame_free(&pAVFrame);
		}
		m_YUVList.pop_front();
	}

	while (m_PCMList.size() > 0)
	{
		AVFrame* pAVFrame = m_PCMList.front();
		if (pAVFrame != nullptr)
		{
			av_frame_free(&pAVFrame);
		}
		m_PCMList.pop_front();
	}

	m_newFFmpeg.m_isStop = false;

	openAudioSDL();

	m_pAVThread1 = SDL_CreateThread(RunDecodeCallBack, nullptr, this);
	m_pAVThread2 = SDL_CreateThread(RunPresentCallBack, nullptr, this);
	m_pAVThread3 = SDL_CreateThread(RunAudioCallBack, nullptr, this);
	//m_pEventThread4 = SDL_CreateThread(RunCheckEvent, nullptr, this);
}

#if 0
int RunCheckEvent(void* arg)
{
	QtSDLPlay* pThis = (QtSDLPlay*)arg;

	do {
		if (pThis->m_newFFmpeg.m_isStop)
			break;

		SDL_Event ev;
		SDL_WaitEvent(&ev);

		switch (ev.type)
		{
		case SDL_QUIT:
			SDL_Log("Event type is %d", ev.type);
			pThis->m_newFFmpeg.m_isStop = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		default:
			SDL_Log("Event type is %d", ev.type);
			break;
		}

	} while (true);

	return 0;
}
#endif

int RunDecodeCallBack(void* arg)
{
	QtSDLPlay* pThis = (QtSDLPlay*)arg;
	//int readSpeed = int(pThis->m_newFFmpeg.m_pCodercCtxVideo->framerate.num / pThis->m_newFFmpeg.m_pCodercCtxVideo->framerate.den);
	int readSpeed = 10;
	std::cout << "FrameRate:" << readSpeed << std::endl;
	while (true)
	{
		// 停止
		if (pThis->m_newFFmpeg.m_isStop == true)
		{
			pThis->m_PCMcdt.notify_all();
			pThis->m_YUVcdt.notify_all();
			return 0;
		}

		// 接受Packet
		if (pThis->m_newFFmpeg.ReadPacket() == nullptr)
			break;

		// 发送Packet
		if (pThis->m_newFFmpeg.SendPacket() != 0)
		{
			///
		}

		do {
			if (pThis->m_newFFmpeg.ReciveFrame() != nullptr)
			{		
				if (pThis->m_newFFmpeg.m_CurrentIndex == pThis->m_newFFmpeg.m_videoIndex)
				{
					//向队列放视频数据
					pThis->m_newFFmpeg.m_pFrameYUV = av_frame_alloc();
					int ret = av_image_alloc(pThis->m_newFFmpeg.m_pFrameYUV->data, \
						pThis->m_newFFmpeg.m_pFrameYUV->linesize, pThis->m_rect.w, pThis->m_rect.h, AV_PIX_FMT_YUV420P, 1);
					//std::cout << "av_image_alloc ret size:[" << ret << "]" << std::endl;

					// convert image
					if (pThis->m_newFFmpeg.m_pImgConvertCtx)
					{
						sws_scale(pThis->m_newFFmpeg.m_pImgConvertCtx, pThis->m_newFFmpeg.m_pFrame->data, pThis->m_newFFmpeg.m_pFrame->linesize, \
							0, pThis->m_newFFmpeg.m_pCodercCtxVideo->height, \
							pThis->m_newFFmpeg.m_pFrameYUV->data, pThis->m_newFFmpeg.m_pFrameYUV->linesize);

						av_frame_free(&(pThis->m_newFFmpeg.m_pFrame));

						std::unique_lock<std::mutex> lck(pThis->m_yuvMtx);
						pThis->m_YUVList.push_back(pThis->m_newFFmpeg.m_pFrameYUV);
						pThis->m_YUVcdt.notify_all();
					}
				}
				else if (pThis->m_newFFmpeg.m_CurrentIndex == pThis->m_newFFmpeg.m_audioIndex)
				{
					//pThis->m_pcmMtx.lock();
					std::unique_lock<std::mutex> lck(pThis->m_pcmMtx);
					pThis->m_PCMList.push_back(pThis->m_newFFmpeg.m_pFrame);
					pThis->m_PCMcdt.notify_all();
					//pThis->m_pcmMtx.unlock();
				}
			}
			else
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(readSpeed));
		} while (true);
	
	}

	do {
		if (pThis->m_newFFmpeg.ReciveFrame() != nullptr)
		{
			if (pThis->m_newFFmpeg.m_CurrentIndex == pThis->m_newFFmpeg.m_videoIndex)
			{
				//向队列放视频数据
				pThis->m_newFFmpeg.m_pFrameYUV = av_frame_alloc();
				int ret = av_image_alloc(pThis->m_newFFmpeg.m_pFrameYUV->data, \
					pThis->m_newFFmpeg.m_pFrameYUV->linesize, pThis->m_rect.w, pThis->m_rect.h, AV_PIX_FMT_YUV420P, 1);

				// convert image
				if (pThis->m_newFFmpeg.m_pImgConvertCtx)
				{
					sws_scale(pThis->m_newFFmpeg.m_pImgConvertCtx, pThis->m_newFFmpeg.m_pFrame->data, pThis->m_newFFmpeg.m_pFrame->linesize, \
						0, pThis->m_newFFmpeg.m_pCodercCtxVideo->height, \
						pThis->m_newFFmpeg.m_pFrameYUV->data, pThis->m_newFFmpeg.m_pFrameYUV->linesize);

					av_frame_free(&(pThis->m_newFFmpeg.m_pFrame));

					pThis->m_yuvMtx.lock();
					pThis->m_YUVList.push_back(pThis->m_newFFmpeg.m_pFrameYUV);
					pThis->m_yuvMtx.unlock();
				}
			}
			else if (pThis->m_newFFmpeg.m_CurrentIndex == pThis->m_newFFmpeg.m_audioIndex)
			{
				pThis->m_pcmMtx.lock();
				pThis->m_PCMList.push_back(pThis->m_newFFmpeg.m_pFrame);
				pThis->m_pcmMtx.unlock();
			}
		}
		else
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while (true);

	//pThis->m_newFFmpeg.Clear();
	//pThis->m_newFFmpeg.Close();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	pThis->m_newFFmpeg.m_isStop = true;
	pThis->m_PCMcdt.notify_all();
	pThis->m_YUVcdt.notify_all();
	
	return 0;
}

int RunPresentCallBack(void* arg)
{
	QtSDLPlay* pThis = (QtSDLPlay*)arg;
	// 向队列取数据并显示
	while (true)
	{
		std::unique_lock<std::mutex> lck(pThis->m_yuvMtx);
		while (pThis->m_YUVList.size() == 0)
		{
			if (pThis->m_newFFmpeg.m_isStop)
				return 0;
			pThis->m_YUVcdt.wait(lck);
		}

		if (pThis->m_newFFmpeg.m_isStop)
			return 0;


		AVFrame* pFrameYUV = pThis->m_YUVList.front();

		if (pFrameYUV)
		{
			SDL_UpdateYUVTexture(pThis->m_pSDLTxt, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0], \
				pFrameYUV->data[1], pFrameYUV->linesize[1], pFrameYUV->data[2], pFrameYUV->linesize[2]);
			SDL_RenderClear(pThis->m_pSDLRen);
			//std::cout << ">>>>>>>>>[" << pFrameYUV->width << "x" << pFrameYUV->height << "] picType[" << pFrameYUV->pict_type << "] fromat["\
				<< pFrameYUV->format << "]" << std::endl;
			SDL_RenderCopy(pThis->m_pSDLRen, pThis->m_pSDLTxt, NULL, &(pThis->m_rect));
			SDL_RenderPresent(pThis->m_pSDLRen);
		}

		av_freep(&pFrameYUV[0]);
		av_frame_free(&pFrameYUV);
		pThis->m_YUVList.pop_front();
		//pThis->m_yuvMtx.unlock();
	}

	return 0;
}

int RunAudioCallBack(void* arg)
{
	QtSDLPlay* pThis = (QtSDLPlay*)arg;
	// 向队列取数据并显示
	while (true)
	{
		while (audioLen > 0 && !(pThis->m_newFFmpeg.m_isStop))
		{
			SDL_Delay(2);
			//pThis->m_PCMLenCdt.wait(lck);
		}

		std::unique_lock<std::mutex> lck(pThis->m_pcmMtx);
		while (pThis->m_PCMList.size() == 0)
		{
			if (pThis->m_newFFmpeg.m_isStop)
				return 0;
			pThis->m_PCMcdt.wait(lck);
		}

		if (pThis->m_newFFmpeg.m_isStop)
			return 0;

		AVFrame* pFramePCM = pThis->m_PCMList.front();

		if (pFramePCM)
		{
			int out_buffer_size = av_samples_get_buffer_size(NULL, \
				pFramePCM->channels, pThis->m_newFFmpeg.m_pCodercCtxAudio->frame_size, AV_SAMPLE_FMT_S16, 1);
			//int ret = swr_convert(pThis->m_newFFmpeg.m_pSwrCxt, \
				(uint8_t**)pPCM, pFramePCM->nb_samples, \
				(const uint8_t**)pFramePCM->data, pFramePCM->nb_samples);
			//memset(pPCM[0], 0x00, 1024 * 1024 * 2);
			int ret = swr_convert(pThis->m_newFFmpeg.m_pSwrCxt, \
				(uint8_t**)pPCM, out_buffer_size, \
				(const uint8_t**)pFramePCM->data, pFramePCM->nb_samples);


			// 写文件
#if 0
			const char* outFile = "D:/安防物联管理平台录像下载/20210622/test.pcm";
			const char* outFile1 = "D:/安防物联管理平台录像下载/20210622/test1.pcm";
			FILE* pFile = fopen(outFile, "ab+");
			FILE* pFile1 = fopen(outFile1, "ab+");
			size_t wRet = fwrite((const void*)(pPCM[0]), 1, out_buffer_size, pFile);
			wRet = fwrite((const void*)(pFramePCM->data[0]), 1, pFramePCM->linesize[0], pFile1);
			wRet = fwrite((const void*)(pFramePCM->data[1]), 1, pFramePCM->linesize[0], pFile1);
			std::cout << "Write :" << wRet << std::endl;
			std::cout << "Audio Info: channel[" << pFramePCM->channels << "] format[" << pFramePCM->format \
				<< "] nb_samples[" << pFramePCM->nb_samples << "] sampleRate[" << pFramePCM->sample_rate << "]" << std::endl;
			fclose(pFile);
			fclose(pFile1);

			static int audioFrameCount = 0;
			++audioFrameCount;
			std::cout << "Audio Frame Count:" << audioFrameCount << std::endl;
#endif
			// 写文件

			audioChunk = (unsigned char*)(pPCM[0]);
			audioPos = audioChunk;
			audioLen = out_buffer_size;//av_samples_get_buffer_size(NULL, pFramePCM->channels, pFramePCM->nb_samples, (enum AVSampleFormat)(pThis->m_newFFmpeg.m_outFormat), 1);
		}

		av_frame_free(&pFramePCM);
		pThis->m_PCMList.pop_front();
		//pThis->m_pcmMtx.unlock();
	}

	return 0;
}
