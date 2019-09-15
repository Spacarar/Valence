 #pragma once

#include "Element.h"
typedef unsigned short int usi;

class Universe {
	usi universeSize;
	Element*** space;

public:
	Universe() {
		universeSize = 0;
		space = nullptr;
	}
	Universe(usi size, usi pixelSize = 8) {
		this->universeSize = size;
		this->space = new Element ** [size];
		for (usi y = 0; y < size; y++) {
			this->space[y] = new Element * [size];
			for (usi x = 0; x < size; x++) {
				int testProtons = (x+y)%8;
				int testElectrons = (x+y)%8;
				int testNeutrons = (x + y) % 8;
				this->space[y][x] = new Element(testProtons, testNeutrons, testElectrons, x * pixelSize * 3, y*pixelSize * 3);
			}
		}
	}

	void update() {
		for (usi y = 0; y < universeSize; y++) {
			for (usi x = 0; x < universeSize; x++) {
				this->space[y][x]->update();
			}
		}
	}

	void draw(SDL_Renderer* ren) {
		for (usi y = 0; y < universeSize; y++) {
			for (usi x = 0; x < universeSize; x++) {
				this->space[y][x]->draw(ren);
			}
		}
	}

	void handleEvent(SDL_Event e, SDL_Point m) {
		return;
	}
};