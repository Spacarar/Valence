#include "Universe.h"

Universe::Universe() {
	universeSize = 0;
	space = nullptr;
	this->outerSpace = nullptr;
}

Universe::Universe(int size, int pixelSize) {
	this->universeSize = size;
	this->space = new Atom * *[size];
	this->outerSpace = new Atom * *[size];
	for (int y = 0; y < size; y++) {
		this->space[y] = new Atom * [size];
		this->outerSpace[y] = new Atom * [size];
		for (int x = 0; x < size; x++) {
			int pne = 0;
			if (floor(rand() % 5) == 0) {
				pne = rand() % 9;
			}
			this->space[y][x] = new Atom(pne, pne, pne, x * pixelSize * 3, y * pixelSize * 3, pixelSize);
			this->outerSpace[y][x] = new Atom(pne, pne, pne, x * pixelSize * 3, y * pixelSize * 3, pixelSize);
			if (pne) {
				std::cout << "pne(" << pne << ")  pressure: " << space[y][x]->radialPressure() << ", " << space[y][x]->nucleoidPressure() << ")\n";
			}
		}
	}
}

int Universe::safeN(int n) {
	if (n < 0) {
		return this->universeSize - (abs(n) % this->universeSize);
	}
	else {
		return n % this->universeSize;
	}
}

void Universe::getNeighborsFor(int y, int x, Atom* neighbors[8]) {
	neighbors[F_TOPL] = this->space[safeN(y - 1)][safeN(x - 1)];
	neighbors[F_TOP] = this->space[safeN(y - 1)][x];
	neighbors[F_TOPR] = this->space[safeN(y - 1)][safeN(x + 1)];
	neighbors[F_RIGHT] = this->space[y][safeN(x + 1)];
	neighbors[F_BOTR] = this->space[safeN(y + 1)][safeN(x + 1)];
	neighbors[F_BOT] = this->space[safeN(y + 1)][x];
	neighbors[F_BOTL] = this->space[safeN(y + 1)][safeN(x - 1)];
	neighbors[F_LEFT] = this->space[y][safeN(x - 1)];
}

void Universe::updateAtomOuterPressure(int y, int x) {
	for (int offX = -1; offX <= 1; offX++) {
		for (int offY = -1; offY <= 1; offY++) {
			if (offY || offX) { //not self
				int thisX = safeN(x + offX);
				int thisY = safeN(y + offY);
				this->space[y][x]->setForceFor(this->space[thisY][thisX], x, y, x + offX, y + offY);
			}
		}
	}
}

void Universe::syncAtomPressureGrid(int y, int x) {
	Atom* neighbors[8];
	this->getNeighborsFor(y, x, neighbors);
	this->space[y][x]->syncPressureWithNeighbors(neighbors);
}

Atom* Universe::strongestNeighboringForce(int y, int x) {
	Atom* strongest = nullptr;
	double strongestForce = 0.0;
	Atom* neighbors[8];
	this->getNeighborsFor(y, x, neighbors);
	if (neighbors[F_TOPL]->botRightForce() > strongestForce) {
		strongest = neighbors[F_TOPL];
	}
	if (neighbors[F_TOP]->botForce() > strongestForce) {
		strongest = neighbors[F_TOP];
	}
	if (neighbors[F_TOPR]->botLeftForce() > strongestForce) {
		strongest = neighbors[F_TOPR];
	}
	if (neighbors[F_RIGHT]->leftForce() > strongestForce) {
		strongest = neighbors[F_RIGHT];
	}
	if (neighbors[F_BOTR]->topLeftForce() > strongestForce) {
		strongest = neighbors[F_BOTR];
	}
	if (neighbors[F_BOT]->topForce() > strongestForce) {
		strongest = neighbors[F_BOT];
	}
	if (neighbors[F_BOTL]->topRightForce() > strongestForce) {
		strongest = neighbors[F_BOTL];
	}
	if (neighbors[F_LEFT]->rightForce() > strongestForce) {
		strongest = neighbors[F_LEFT];
	}
	return strongest;
}

void Universe::moveAtoms(int y, int x) {
	if (this->space[y][x]->isEmpty()) {
		return;
	}
	//check which direction the force is telling the atom to move in
	int checkX = safeN(x + this->space[y][x]->dx());
	int checkY = safeN(y + this->space[y][x]->dy());
	/*std::cout << "Move atoms calculated for: (" << x << ", " << y << ")";
	std::cout << " dx:" << this->space[y][x]->dx() << "  dy:" << this->space[y][x]->dy() << std::endl;

	std::cout << "against (" << checkX << ", " << checkY << ")";
	std::cout << " dx:" << this->space[checkY][checkX]->dx() << "  dy:" << this->space[checkY][checkX]->dy() << std::endl;*/

	//if there is an atom here we cannot move into that position
	if (!this->space[checkY][checkX]->isEmpty()) {
		this->outerSpace[y][x]->setValue(this->space[y][x]);
		return;
	}
	//this point in code represents an atom with force moving it in the direction of an empty space
	//check the empty space's neighboring cells for the atom that has the greatest applied force in that direction
	//if you are the greatest force swap with the empty space.
	if (this->space[y][x] == this->strongestNeighboringForce(checkY, checkX)) {
		//std::cout << "PASS" << std::endl;
		this->outerSpace[checkY][checkX]->setValue(this->space[y][x]);
		this->outerSpace[y][x]->setValue(this->space[checkY][checkX]);
	}
	else {
		//std::cout << "FAIL" << std::endl;
		this->outerSpace[y][x]->setValue(this->space[y][x]);
	}
}

void Universe::update() {
	for (int y = 0; y < universeSize; y++) {
		for (int x = 0; x < universeSize; x++) {
			this->space[y][x]->update();
			this->outerSpace[y][x]->setEmpty();
			this->updateAtomOuterPressure(y, x);
		}
	}
	for (int y = 0; y < universeSize; y++) {
		for (int x = 0; x < universeSize; x++) {
			this->syncAtomPressureGrid(y, x);
		}
	}
	//this->printUniverse();
	//std::cout << std::endl;
	for (int y = 0; y < universeSize; y++) {
		for (int x = 0; x < universeSize; x++) {
			this->moveAtoms(y, x);
		}
	}
	//std::cout << "One Update" << std::endl;
	//std::cin.get();
	std::swap(this->space, this->outerSpace);
}

void Universe::printUniverse() {
	using namespace std;
	cout << fixed << showpoint << setprecision(1);
	for (int y = 0; y < universeSize; y++) {
		for (int yLevel = 0; yLevel < 3; yLevel++) {
			for (int x = 0; x < universeSize; x++) {
				if (yLevel == 0) {
					cout << setw(6) << this->space[y][x]->outerForceAt(F_TOPL) << "|";
					cout << setw(6) << this->space[y][x]->outerForceAt(F_TOP) << "|";
					cout << setw(6) << this->space[y][x]->outerForceAt(F_TOPR) << "|";
				}
				else if (yLevel == 1) {
					cout << setw(6) << this->space[y][x]->outerForceAt(F_LEFT) << "|";
					cout << setw(6) << "X" << "|";
					cout << setw(6) << this->space[y][x]->outerForceAt(F_RIGHT) << "|";
				}
				else {
					cout << setw(6) << this->space[y][x]->outerForceAt(F_BOTL) << "|";
					cout << setw(6) << this->space[y][x]->outerForceAt(F_BOT) << "|";
					cout << setw(6) << this->space[y][x]->outerForceAt(F_BOTR) << "|";
				}
			}
			cout << endl;
		}
		cout << endl;
	}
}

void Universe::draw(SDL_Renderer* ren) {
	static int drawCount = 0;
	for (int y = 0; y < universeSize; y++) {
		for (int x = 0; x < universeSize; x++) {
			this->space[y][x]->draw(ren, drawCount);
		}
	}
	//drawCount++;
}

void Universe::handleEvent(SDL_Event e, SDL_Point m) {
	return;
}
