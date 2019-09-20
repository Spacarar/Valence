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
typedef enum OFP {F_TOPL, F_TOP, F_TOPR, F_RIGHT, F_BOTR, F_BOT, F_BOTL, F_LEFT, F_NONE} OFP; //Outer force position
static OFP getOFP(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	if (dx == 0) {
		if (dy == 0) {
			return F_NONE;
		}
		else if (dy > 0) {
			return F_BOT;
		}
		else {
			return F_TOP;
		}
	}
	else if (dx > 0) {
		if (dy == 0) {
			return F_RIGHT;
		}
		else if (dy > 0) {
			return F_BOTR;
		}
		else {
			return F_TOPR;
		}
	}
	else {
		if (dy == 0) {
			return F_LEFT;
		}
		else if (dy > 0) {
			return F_BOTL;
		}
		else {
			return F_TOPL;
		}
	}
}
static OFP getOFP(SDL_Point xy1, SDL_Point xy2) {
	return getOFP(xy1.x, xy1.y, xy2.x, xy2.y);
}
static OFP OOFP(OFP pos = F_TOPL) {
	if (pos == F_NONE) {
		return F_NONE;
	}
	return static_cast<OFP>((static_cast<int>(pos) + 4) % 8);
}
static OFP OOFP(int pos = 0) {
	return OOFP(static_cast<OFP>(pos));
}

class Atom {
protected:
	int x, y;
	int dx, dy;
	int protons;
	int neutrons;
	int electrons;
	int vElectrons;
	bool valence[8];
	double outerForce[8];
	unsigned short int renderOffset;
	unsigned short int pixelSize;
	bool setOnPass[8];

	void setOuterPressure(double value, OFP position) {
		this->outerForce[position] = value;
		this->setOnPass[position] = true;
	}
	void setOuterPressure(double value[8]) {
		for (int i = 0; i < 8; i++) {
			this->outerForce[i] = value[i];
			this->setOnPass[i] = true;
		}
	}

	void syncPressureAt(double value, OFP position) {
		this->outerForce[position] = value;
		this->setOnPass[position] = false;
	}

public:
	Atom() {
		this->protons = this->electrons = this->neutrons = this->vElectrons = 0;
		this->pixelSize = this->renderOffset = 0;
		this->x = this->y = 0;
		this->dx = this->dy = 0;
		for (int i = 0; i < 8; i++) {
			valence[i] = false;
			outerForce[i] = 0.0;
			this->setOnPass[i] = false;
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
		this->vElectrons = electrons % 8;
		if (electrons != 0 && vElectrons == 0) {
			this->vElectrons = 8;
		}
		int oElectrons = 8 - vElectrons;
		int startingPosition = rand() % 8;
		int valenceRatio = 1, unsetRatio = 1;

		for (int i = 0; i < 8; i++) {
			this->setOnPass[i] = false;
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
		this->electrons = e.electrons;
		this->vElectrons = e.vElectrons;
		this->protons = e.protons;
		this->neutrons = e.neutrons;
		this->renderOffset = e.renderOffset;
		this->pixelSize = e.pixelSize;
		for (int i = 0; i < 8; i++) {
			this->valence[i] = e.valence[i];
			this->outerForce[i] = e.outerForce[i];
			this->setOnPass[i] = e.setOnPass[i];
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

	double measureOuterPressure(Atom* e) {
		//positive push
		//negative pull
		if (e == nullptr || this->isEmpty()) {
			return 0;
		}
		int netCharge = this->charge() + e->charge();
		int valenceDiff = abs(this->vElectrons - e->vElectrons);
		int valenceIonic = this->vElectrons + e->vElectrons;
		double valenceCovalent = this->vElectrons * 2.0 / 3.0 + e->vElectrons * 2.0 / 3.0;
		double ionicChance = valenceDiff * (1.0 / 6.0) * 100.0 - abs(valenceIonic - 8.0)  * 12.0 - abs(netCharge) * 12.0;
		double covalentChance = 100.0 - valenceDiff * 12.0 - abs(valenceCovalent - 8.0) * 12.0 - abs(netCharge) * 12.0;
		if (ionicChance < 50 && covalentChance < 50) {
			//add in the abs(ionicChance) to push away the same elements from each other if they don't naturally bond
			return abs(this->radialPressure() - e->radialPressure() + abs(ionicChance));
		}
		else  if (ionicChance > covalentChance) {
			return abs(this->radialPressure() - e->radialPressure() - abs(ionicChance / 10)) * -1;
		}
		else {
			return abs(this->radialPressure() - e->radialPressure() - abs(covalentChance / 10)) * -1;
		}
	}
	
	//universe first runs setForceFor. turning setOnPass to true as it goes along
	void setForceFor(Atom* e, int x1, int y1, int x2, int y2) {
		if (this->isEmpty() || e->isEmpty()) {
			return;
		}
		OFP myPos = getOFP(x1, y1, x2, y2);
		OFP otherPos = getOFP(x2, y2, x1, y1);
		if (this->setOnPass[myPos]) { //we saw this side from a neighbor within this update
			return;
		}
		double p = measureOuterPressure(e);
		this->setOuterPressure(p, getOFP(x1, y1, x2, y2));
		e->setOuterPressure(p, getOFP(x2, y2, x1, y1));
	}
	
	
	void syncPressureWithNeighbors(Atom* oa[8]) { //OFP Positioned neighbors
		//setOnPass is true here from setForceFor.
		//so only update the spaces that have not been set to false yet
		if (this->setOnPass[F_TOPL]) {
			double tlForce = this->outerForce[F_TOPL] + oa[F_TOPL]->outerForce[F_BOTR] + oa[F_TOP]->outerForce[F_BOTL] + oa[F_LEFT]->outerForce[F_TOPR];
			this->syncPressureAt(tlForce, F_TOPL);
			oa[F_TOPL]->syncPressureAt(tlForce, F_BOTR);
			oa[F_TOP]->syncPressureAt(tlForce, F_BOTL);
			oa[F_LEFT]->syncPressureAt(tlForce, F_TOPR);
		}
		if (this->setOnPass[F_TOPR]) {
			double trForce = this->outerForce[F_TOPR] + oa[F_TOPR]->outerForce[F_BOTL] + oa[F_TOP]->outerForce[F_BOTR] + oa[F_RIGHT]->outerForce[F_TOPL];
			this->syncPressureAt(trForce, F_TOPR);
			oa[F_TOPR]->syncPressureAt(trForce, F_BOTL);
			oa[F_TOP]->syncPressureAt(trForce, F_BOTR);
			oa[F_RIGHT]->syncPressureAt(trForce, F_TOPL);
		}
		if (this->setOnPass[F_BOTL]) {
			double blForce = this->outerForce[F_BOTL] + oa[F_BOTL]->outerForce[F_TOPR] + oa[F_BOT]->outerForce[F_TOPL] + oa[F_LEFT]->outerForce[F_BOTR];
			this->syncPressureAt(blForce, F_BOTL);
			oa[F_BOTL]->syncPressureAt(blForce, F_TOPR);
			oa[F_BOT]->syncPressureAt(blForce, F_TOPL);
			oa[F_LEFT]->syncPressureAt(blForce, F_BOTR);
		}
		if (this->setOnPass[F_BOTR]) {
			double brForce = this->outerForce[F_BOTR] + oa[F_BOTR]->outerForce[F_TOPL] + oa[F_BOT]->outerForce[F_TOPR] + oa[F_RIGHT]->outerForce[F_BOTL];
			this->syncPressureAt(brForce, F_BOTR);
			oa[F_BOTR]->syncPressureAt(brForce, F_TOPL);
			oa[F_BOT]->syncPressureAt(brForce, F_TOPR);
			oa[F_LEFT]->syncPressureAt(brForce, F_BOTL);
		}
		if (this->setOnPass[F_TOP]) {
			double topForce = this->outerForce[F_TOP] + oa[F_TOP]->outerForce[F_BOT];
			this->syncPressureAt(topForce, F_TOP);
			oa[F_TOP]->syncPressureAt(topForce, F_BOT);
		}
		if (this->setOnPass[F_BOT]) {
			double botForce = this->outerForce[F_BOT] + oa[F_BOT]->outerForce[F_TOP];
			this->syncPressureAt(botForce, F_BOT);
			oa[F_BOT]->syncPressureAt(botForce, F_TOP);
		}
		if (this->setOnPass[F_LEFT]) {
			double leftForce = this->outerForce[F_LEFT] + oa[F_LEFT]->outerForce[F_RIGHT];
			this->syncPressureAt(leftForce, F_LEFT);
			oa[F_LEFT]->syncPressureAt(leftForce, F_LEFT);
		}
		if (this->setOnPass[F_RIGHT]) {
			double rightForce = this->outerForce[F_RIGHT] + oa[F_RIGHT]->outerForce[F_LEFT];
			this->syncPressureAt(rightForce, F_RIGHT);
			oa[F_RIGHT]->syncPressureAt(rightForce, F_LEFT);
		}
	}
};
