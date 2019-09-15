#pragma once

#include <SDL.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <vector>

const int RENDER_POSITION[8] = { 0, 1, 2, 4, 7, 6, 5, 3};

class Element {
protected:
	int x, y;
	int dx, dy;
	int protons;
	int neutrons;
	int electrons;
	int valence[8];
	unsigned short int renderOffset;
	unsigned short int pixelSize;

public:
	Element() {
		this->protons = this->electrons = this->neutrons = 0;
		this->pixelSize = this->renderOffset = 0;
		this->x = this->y = 0;
		this->dx = this->dy = 0;
		for (int i = 0; i < 8; i++) {
			valence[i] = false;
		}
	}
	Element(int protons, int neutrons = -1, int electrons = -1, int x = 0, int y = 0, unsigned short int pixelSize = 8) {
		this->protons = protons;
		this->electrons = electrons == -1 ? protons : electrons;
		this->neutrons = neutrons == -1 ? protons : neutrons;
		this->pixelSize = pixelSize;
		this->renderOffset = 0;
		this->x = x;
		this->y = y;
		this->dx = this->dy = 0;

		int vElectrons = electrons % 8;
		if (electrons != 0 && vElectrons == 0) {
			vElectrons = 8;
		}
		int oElectrons = 8 - vElectrons;
		int startingPosition = rand() % 8;
		int valenceRatio = 1, unsetRatio = 1;

		for (int i = 0; i < 8; i++) {
			if (valenceRatio * oElectrons < unsetRatio * vElectrons) {
				valenceRatio++;
				this->valence[(i + startingPosition) % 8] = true;
			}
			else {
				unsetRatio++;
				this->valence[(i + startingPosition) % 8] = false;
			}
		}
	}

	void draw(SDL_Renderer* ren) {
		if (!this->protons && !this->electrons && !this->neutrons) {
			return;
		}
		static SDL_Rect drawRect;
		drawRect.w = drawRect.h = this->pixelSize;
		drawRect.x = this->x + this->pixelSize;
		drawRect.y = this->y + this->pixelSize;
		//FIXME PROTON COLOR
		const int red[8] = {0, 0, 0, 55, 55, 0, 240, 255};
		const int green[8] = {200, 155, 100, 200, 122, 25, 25, 25};
		const int blue[8] = {255, 255, 255, 200, 42, 122, 55, 0};
		SDL_SetRenderDrawColor(ren, red[protons % 8], green[protons % 8], blue[protons % 8], 255);
		SDL_RenderFillRect(ren, &drawRect);

		const int xReorder[8] = { 0, 1, 2, 2, 2, 1, 0, 0 };
		const int yReorder[8] = { 0, 0, 0, 1, 2, 2, 2, 1 };
		for (unsigned short int i = 0; i < 8; i++) {
			if (this->valence[(i + this->renderOffset) % 8]) {
				SDL_SetRenderDrawColor(ren, 250, 255, 255, 255);
			}
			else {
				SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			}
			drawRect.x = this->x + xReorder[i] * this->pixelSize;
			drawRect.y = this->y + yReorder[i] * this->pixelSize;
			SDL_RenderFillRect(ren, &drawRect);
		}
		std::cout << std::endl << std::endl;
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	}

	void valencePosition() {
		std::cout << "Valence at: ";
		for (int i = 0; i < 8; i++) {
			if (valence[i]) {
				std::cout << i << " ";
			}
		}
		std::cout << std::endl;
	}

	void update() {
		if (!this->protons && !this->electrons && !this->neutrons) {
			return;
		}
		this->renderOffset = (this->renderOffset + 1) % 8;
	}

	//first level fundemental calculations
	int charge() {
		return this->protons - this->electrons;
	}
	double weight() {
		return this->protons + this->neutrons * 1.125 + this->electrons * 0.01;
	}

	double neutronCharge() {
		return (this->protons - (this->neutrons * 0.8)) * sqrt(this->weight() / 2.0);
	}

	//second level fundemental calculations
	double radialPressure() {
		const double chargeCoeff = abs(3 * this->charge());
		const double valenceCoeff = std::min(this->electrons % 8, 8 - this->electrons % 8) * 12.0;
		const double weightCoeff = this->weight() * 0.4;
		return chargeCoeff + valenceCoeff + weightCoeff;
	}
	double nucleoidPressure() {
		double neutronCoeff = 3.0 * abs(this->neutronCharge());
		const double sizeCoeff = sqrt(this->weight() / 2.0);
		return neutronCoeff * sizeCoeff;
	}
	double inertia() {
		return this->weight();
	}

	//tertiary calculations
	double totalPressure() {
		return this->radialPressure() + this->nucleoidPressure();
	}
};