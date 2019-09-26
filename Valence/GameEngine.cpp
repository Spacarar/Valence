#include "GameEngine.h"
#include <thread>

const unsigned int UNIVERSE_SIZE = 9;

GameEngine::GameEngine() {
	initPreSDL();
	initSDL();
	initPostSDL();
}

void GameEngine::initPreSDL() {
	srand(time(NULL));
	UPS = 2;
	totalFrames = 0;
	totalUpdates = 0;
	startTime = std::chrono::steady_clock::now();
}
void GameEngine::initSDL() {
	screenWidth = 1280;
	screenHeight = 720;

	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(-1);
	}
	window = SDL_CreateWindow("Valence", 50, 50, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		printf("Could not create window!");
	}
	//FIXME GRAPHICS SETTINGS
	//SDL_SetWindowFullscreen(gameWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
	}
	//Initialize SDL_mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		printf("SDL2_mixer init error\n CLOSING...");
		SDL_Quit();
	}
	if (TTF_Init() == -1) {
		printf("SDL2_ttf init error\n CLOSING...");
		SDL_Quit();
	}
}
void GameEngine::initPostSDL() {
	universe = new Universe(UNIVERSE_SIZE);
	isRunning = true;
}

int GameEngine::updateGame(void* self) {
	GameEngine* g = (GameEngine*)self;
	while (g->isRunning) {
		if (g->updateRequired()) {
			g->update();
		}
	}
	return 0;
}
bool GameEngine::updateRequired() {
	using namespace std::chrono_literals;
	std::chrono::milliseconds duration;
	duration = std::chrono::milliseconds(1000 / UPS) - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUpdate);
	if (duration < 1ms) {
		lastUpdate = std::chrono::steady_clock::now();
		return true;
	}
	else {
		std::this_thread::sleep_for(duration);
		return false;
	}
}

void GameEngine::update() {
	totalUpdates++;
	universe->update();
}

int GameEngine::renderGame(void* self) {
	GameEngine* g = (GameEngine*)self;
	while (g->isRunning) {
		if (g->renderRequired()) {
			g->render();
		}
	}
	return 0;
}

bool GameEngine::renderRequired() {
	return true;
}
void GameEngine::render() {
	totalFrames++;
	SDL_RenderClear(ren);
	universe->draw(ren);
	SDL_RenderPresent(ren);
}

void GameEngine::handleEvent(SDL_Event e) {
	static SDL_Point mousePoint;
	mousePoint.x = e.motion.x;
	mousePoint.y = e.motion.y;
	universe->handleEvent(e, mousePoint);
}

void GameEngine::run() {
	int updateResult, drawResult;
	//update things while this is going on
	updateThread = SDL_CreateThread(updateGame, "Update", this);
	//oh yeah and draw them too
	renderThread = SDL_CreateThread(renderGame, "Render", this);
	while (isRunning) {
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent)) {
			//if the window was closed, close the game
			if (sdlEvent.type == SDL_WINDOWEVENT) {
				if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
					isRunning = false;
				}
			}
			//any other event needs to be handled
			handleEvent(sdlEvent);
		}
	}
	//give me some info before you're done
	std::cout << "FRAMES: " << totalFrames << " FPS: " << (totalFrames * 1000) / ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count())) << std::endl;
	std::cout << "Total Updates: " << totalUpdates << std::endl;
	SDL_WaitThread(updateThread, &updateResult);
	SDL_WaitThread(renderThread, &drawResult);
	quit();
}

void GameEngine::quit() {
	isRunning = false;
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(window);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}