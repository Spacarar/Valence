#pragma once

#include<iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <chrono>

#include "Universe.h"

typedef std::chrono::steady_clock::time_point time_point;
typedef std::chrono::milliseconds millis;

class GameEngine {
	int UPS;
	size_t totalFrames;
	size_t totalUpdates;
	time_point startTime;
	
	int screenWidth;
	int screenHeight;

	bool isRunning;

	Universe* universe;

	SDL_Thread* updateThread, * renderThread;
	SDL_Window* window;
	SDL_Renderer* ren;

	time_point lastUpdate, lastRender;
	millis currentTime;
	void initPreSDL();
	void initSDL();
	void initPostSDL();

	static int updateGame(void* self);
	bool updateRequired();
	void update();

	static int renderGame(void* self);
	bool renderRequired();
	void render();

	void handleEvent(SDL_Event e);

public:
	GameEngine();
	void run();
	void quit();
};