#ifndef _MYSDL_H_
#define _MYSDL_H_

#include <iostream>
#include <string>

#define SDL_MAIN_HANDLED

extern "C" {
#include "SDL.h"
#include "SDL_main.h"
}

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2test.lib")

//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
//Break
#define BREAK_EVENT  (SDL_USEREVENT + 2)
class SDLPlayer
{
public:
	SDLPlayer();
	int initPlayer(void* winID = NULL);
	void setHeight(int height);
	void setWidth(int width);
	void setPlayerTitle(std::string title);
	int playYUV(uint8_t* buffer, SDL_Rect sdlRect, int delayTime = 40);
	int playYUV(uint8_t* buffer, int delayTime = 40);
	void pause();
	void start();
	void quit();

	static int refreshThread(void* opaque);
	~SDLPlayer();

private:
	int windowH;
	int windowW;
	int height;
	int width;
	std::string title;
	//SDL WINDOW
	SDL_Window* window;
	SDL_Texture* sdlTexture;
	SDL_Renderer* sdlRanderer;
	//ÊÂ¼þ
	SDL_Event event;

	int threadExit;
	int threadPause;
	int delayTime;

};


#endif