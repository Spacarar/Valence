#include <iostream>
#include "GameEngine.h"

int main(int argc, char** argv) {
	std::cout << "Welcome to valence, this program does nothing thanks" << std::endl;
	GameEngine* valence = new GameEngine();
	valence->run();
	std::cin.get();
	return 0;
}