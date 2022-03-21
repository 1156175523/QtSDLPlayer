#include "MySDL.h"

SDLPlayer::SDLPlayer() :
	height(420),
	width(640),
	title("SDL player"),
	window(nullptr),
	sdlTexture(nullptr),
	sdlRanderer(nullptr),
	threadExit(0),
	threadPause(0),
	delayTime(40)
{
}

int SDLPlayer::initPlayer(void* winID)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	windowW = this->width;
	windowH = this->height;
	if (winID != NULL) {
		window = SDL_CreateWindowFrom(winID);

	}
	else {
		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			windowW, windowH, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	}

	if (window == nullptr) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}
	sdlRanderer = SDL_CreateRenderer(window, -1, 0);
	SDL_ShowWindow(window);
	Uint32 pixformat = 0;
	pixformat = SDL_PIXELFORMAT_IYUV;
	sdlTexture = SDL_CreateTexture(sdlRanderer, pixformat,
		SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
	//创建新线程
	SDL_CreateThread(SDLPlayer::refreshThread, NULL, this);
	return 1;
}

void SDLPlayer::setHeight(int _height)
{
	height = _height;
}

void SDLPlayer::setWidth(int _width)
{
	this->width = _width;
}

void SDLPlayer::setPlayerTitle(std::string _title)
{
	title = _title;
}

int SDLPlayer::playYUV(uint8_t* buffer, SDL_Rect sdlRect, int delayTime)
{
	SDL_WaitEvent(&event);
	if (event.type == REFRESH_EVENT) {
		SDL_UpdateTexture(sdlTexture, NULL, buffer, this->width);
		SDL_RenderClear(sdlRanderer);
		SDL_RenderCopy(sdlRanderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRanderer);
		//Delay 40ms
		this->delayTime = delayTime;
	}
	else if (event.type == SDL_QUIT) {
		this->threadExit = 1;
	}
	else if (event.type == BREAK_EVENT) {
		return -1;
	}
	else if (event.type == SDL_WINDOWEVENT) {
		//If Resize
		SDL_GetWindowSize(window, &windowW, &windowH);
	}
	return 0;

}

int SDLPlayer::playYUV(uint8_t* buffer, int delayTime)
{

	SDL_WaitEvent(&event);
	if (event.type == REFRESH_EVENT) {
		SDL_Rect sdlRect;
		SDL_UpdateTexture(sdlTexture, NULL, buffer, this->width);
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = windowW;
		sdlRect.h = windowH;

		SDL_RenderClear(sdlRanderer);
		SDL_RenderCopy(sdlRanderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRanderer);
		//Delay 40ms

		this->delayTime = delayTime;
	}
	else if (event.type == SDL_QUIT) {
		this->threadExit = 1;
	}
	else if (event.type == SDL_WINDOWEVENT) {
		//If Resize
		SDL_GetWindowSize(window, &windowW, &windowH);
	}
	else if (event.type == BREAK_EVENT) {
		return -1;
	}
	return 0;
}

void SDLPlayer::pause()
{
	if (this->threadPause == 1) {
		this->threadPause = 0;
	}
	else {
		this->threadPause = 1;
	}

}

void SDLPlayer::start()
{
	this->threadPause = 0;

}

void SDLPlayer::quit()
{
	this->threadExit = 1;
}


int SDLPlayer::refreshThread(void* opaque)
{
	SDLPlayer* sdl = (SDLPlayer*)opaque;
	while (!sdl->threadExit)
	{
		if (!sdl->threadPause) {
			SDL_Event _event;
			_event.type = REFRESH_EVENT;
			SDL_PushEvent(&_event);
			SDL_Delay(sdl->delayTime);
		}
	}
	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);
	return 0;

}

SDLPlayer::~SDLPlayer()
{
	SDL_DestroyWindow(window);
	SDL_Quit();

}
