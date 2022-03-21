#pragma once

#include <QtWidgets/QWidget>
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QCloseEvent>
#include "ui_QtSDLPlay.h"
#include "MyFFmpeg.h"

#include <mutex>
#include <list>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <atomic>

extern "C" {
#include "SDL.h"
}

class QtSDLPlay : public QWidget
{
    Q_OBJECT

public:
    QtSDLPlay(QWidget *parent = Q_NULLPTR);

    QString m_fileUrl{ "" };
    // 设置videoWidget
    void openAudioSDL();
    void showSDLVideoWidget();      // videoWidget 显示SDL
    void destroySDLVideoWidget();   // videoWidget 销毁
    void videoWidgetSetPos();       // videoWidget 设置位置

public:
    SDL_Window* m_pSDLWin{ nullptr };
    SDL_Renderer* m_pSDLRen{ nullptr };
    SDL_Texture* m_pSDLTxt{ nullptr };

    // 播放音频
    SDL_AudioSpec m_sdlAudioSpec;
    
    SDL_Rect m_rect = { 0,0,0,0 };

    NewFFmpeg m_newFFmpeg;
    std::list<AVFrame*> m_YUVList; // 视频数据
    std::list<AVFrame*> m_PCMList; // 音频数据

    std::mutex m_yuvMtx;
    std::condition_variable m_YUVcdt;
    std::mutex m_pcmMtx;
    std::condition_variable m_PCMcdt;
    std::mutex m_pcmLenMtx;
    std::condition_variable m_PCMLenCdt;

public slots:
    void SelectLocalFile();
    void OpenFile();

private:
    Ui::QtSDLPlayClass ui;


    SDL_Thread* m_pAVThread1{ nullptr };
    SDL_Thread* m_pAVThread2{ nullptr };
    SDL_Thread* m_pAVThread3{ nullptr };
    SDL_Thread* m_pEventThread4{ nullptr };

protected:
    void closeEvent(QCloseEvent* event);
};

int RunDecodeCallBack(void* arg);
int RunPresentCallBack(void* arg);
int RunAudioCallBack(void* arg);
void ReadAudioData(void* udata, Uint8* stream, int len);
//int RunCheckEvent(void* arg);