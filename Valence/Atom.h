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
typedef enum OFP {F_TOPL, F_TOP, F_TOPR, F_RIGHT, F_BOTR, F_BOT, F_BOTL, F_LEFT} OFP; //Outer force position

class Atom {
protected:
	int x, y;
	int dx, dy;
	int protons;
	int neutrons;
	int electrons;
	int valence[8];
	double outerForce[8];
	unsigned short int renderOffset;
	unsigned short int pixelSize;



public:
	Atom() {
		this->protons = this->electrons = this->neutrons = 0;
		this->pixelSize = this->renderOffset = 0;
		this->x = this->y = 0;
		this->dx = this->dy = 0;
		for (int i = 0; i < 8; i++) {
			valence[i] = false;
			outerForce[i] = 0.0;
		}
	}
	Atom(int protons, int neutrons = -1, int electrons = -1, int x = 0, int y = 0, unsigned short int pixelSize = 8) {
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
			outerForce[i] = 0.0;
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
	Atom(const Atom& e) {
		this->x = e.x;
		this->y = e.y;
		this->dx = e.dx;
		this->dy = e.dy;
		this->protons = e.protons;
		this->neutrons = e.neutrons;
		this->renderOffset = e.renderOffset;
		this->pixelSize = e.pixelSize;
		for (int i = 0; i < 8; i++) {
			this->valence[i] = e.valence[i];
			this->outerForce[i] = e.outerForce[i];
		}
	}
	bool isEmpty() {
		return (!this->protons && !this->electrons && !this->neutrons);
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
		const int red[8] =   {  75,   0,   0,  55, 122, 255, 240, 200};
		const int green[8] = {255, 155, 50, 200, 188, 122, 100,  0};
		const int blue[8] =  {255, 255, 220, 105,  42,  42, 155,  25};

		if (this->protons) {
			SDL_SetRenderDrawColor(ren, red[protons % 8], green[protons % 8], blue[protons % 8], 255);
			SDL_RenderFillRect(ren, &drawRect);
		}
		else if (this->neutrons) { //no protons only neutrons
			SDL_SetRenderDrawColor(ren, 122, 122, 122, 255);
			SDL_RenderFillRect(ren, &drawRect);
		}
		

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
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	}

	void update() {
		x += dx * 3 * pixelSize;
		y += dy * 3 * pixelSize;
		if (this->isEmpty()) {
			return;
		}
		this->renderOffset = (this->renderOffset + 1) % 8;
	}

	//measure of valence shell filling
	int charge() {
		return this->protons - this->electrons;
	}

	double weight() {
		return this->protons + this->neutrons * 1.125 + this->electrons * 0.01;
	}

	//measure between neutrons & protons
	double neutronCharge() {
		return (this->protons - (this->neutrons * 0.8)) * sqrt(this->weight() / 2.0);
	}

	//stability of electrons
	double radialPressure() {
		const double chargeCoeff = abs(3 * this->charge());
		const double valenceCoeff = std::min(this->electrons % 8, 8 - this->electrons % 8) * 12.0;
		const double weightCoeff = this->weight() * 0.6;
		return chargeCoeff + valenceCoeff + weightCoeff;
	}

	//stability of the nucleus
	double nucleoidPressure() {
		double neutronCoeff = 3.0 * abs(this->neutronCharge());
		const double sizeCoeff = sqrt(this->weight() / 2.0);
		return neutronCoeff * sizeCoeff;
	}

	//tertiary calculations
	double totalPressure() {
		return this->radialPressure() + this->nucleoidPressure();
	}

	double measureOuterPressure(const Atom* e) {
		if (e == nullptr || this->isEmpty()) {
			return 0;
		}
		//FIXME FORCE CALCULATION BETWEEN ATOMS
		return 0.0
	}
	
	void setOuterPressure(double value, OFP, position) {
		this->outerForce[position] = value;
	}
	void setOuterPressure(double value[8]) {
		for(int i = 0; i < 8; i++) {
			this->outerForce[i] = value[i];
		}
	}
};
