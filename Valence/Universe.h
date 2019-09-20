 #pragma once

#include "Atom.h"
#include <iomanip>
class Universe {
	int universeSize;
	Atom*** space;
	int safeN(int n) {
		if (n < 0) {
			return this->universeSize - (abs(n) % this->universeSize);
		}
		else {
			return n % this->universeSize;
		}
	}
public:
	Universe() {
		universeSize = 0;
		space = nullptr;
	}
	Universe(int size, int pixelSize = 8) {
		this->universeSize = size;
		this->space = new Atom ** [size];
		for (int y = 0; y < size; y++) {
			this->space[y] = new Atom * [size];
			for (int x = 0; x < size; x++) {
				int pne = floor(rand() % 9);
				if (x % 3 == 0 && y % 2 == 0 || x % 2 == 0 && y % 3 == 0) {
					pne = 0;
				}
				this->space[y][x] = new Atom(pne, pne, pne, x * pixelSize * 3, y * pixelSize * 3, pixelSize);
				if (pne) {
					std::cout << "pne(" << pne << ")  pressure: " << space[y][x]->radialPressure() << ", " << space[y][x]->nucleoidPressure() << ")\n\n";
				}
			}
		}
	}

	void updateAtomOuterPressure(int y, int x) {
		for (int offX = -1; offX <= 1; offX++) {
			for (int offY = -1; offY <= 1; offY++) {
				if (offY || offX) { //not self
					int thisX = safeN(x + offX);
					int thisY = safeN(y + offY);
					this->space[y][x]->setForceFor(this->space[thisY][thisX], x, y, thisX, thisY);
				}
			}
		}
	}
	void syncAtomPressureGrid(int y, int x) {
		static Atom* neighbors[8];
		neighbors[F_TOPL] = this->space[safeN(y - 1)][safeN(x - 1)];
		neighbors[F_TOP] = this->space[safeN(y - 1)][x];
		neighbors[F_TOPR] = this->space[safeN(y - 1)][safeN(x + 1)];
		neighbors[F_RIGHT] = this->space[y][safeN(x + 1)];
		neighbors[F_BOTR] = this->space[safeN(y + 1)][safeN(x + 1)];
		neighbors[F_BOT] = this->space[safeN(y + 1)][x];
		neighbors[F_BOTL] = this->space[safeN(y + 1)][safeN(x - 1)];
		neighbors[F_LEFT] = this->space[y][safeN(x - 1)];
		this->space[y][x]->syncPressureWithNeighbors(neighbors);
	}

	void update() {
		for (int y = 0; y < universeSize; y++) {
			for (int x = 0; x < universeSize; x++) {
				this->space[y][x]->update();
				this->updateAtomOuterPressure(y, x);
			}
		}
		for (int y = 0; y < universeSize; y++) {
			for (int x = 0; x < universeSize; x++) {
				this->syncAtomPressureGrid(y, x);
			}
		}
		this->printUniverse();
		std::cout << std::endl << "Waiting for confirmation..." << std::endl;
		std::cin.get();
		std::cin.get();
	}

	void printUniverse() {
		using namespace std;
		cout << fixed << showpoint << setprecision(1);
		for (int y = 0; y < universeSize; y++) {
			for(int yLevel = 0; yLevel < 3; yLevel++) {
				for (int x = 0; x < universeSize; x++) {
					if (yLevel == 0) {
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_TOPL) << "|";
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_TOP) << "|";
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_TOPR) << "|";
					}
					else if (yLevel == 1) {
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_LEFT) << "|";
						cout << setw(6) << "X|";
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_RIGHT) << "|";
					}
					else {
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_BOTL) << "|";
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_BOT) << "|";
						cout << setw(6) <<  this->space[y][x]->outerForceAt(F_BOTR) << "|";
					}
				}
				cout << endl;
			}
			cout << endl;
		}
	}

	void draw(SDL_Renderer* ren) {
		for (int y = 0; y < universeSize; y++) {
			for (int x = 0; x < universeSize; x++) {
				this->space[y][x]->draw(ren);
			}
		}
	}

	void handleEvent(SDL_Event e, SDL_Point m) {
		return;
	}
};