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

/*
* Base Structure of the universe
*
* Simple replication of a real world atom.
* Based around the idea of 8 valence electrons being possible
* The goal is to  have atoms uniquely and interestingly interact with one another based on their
* protons, neutrons and electrons, which determing other factors such as the valence shell, 
* the inner and outer force. All of these coefficients should be taken into account in the 
* universe interaction
*/
class Atom {
protected:
	int x, y;
	int protons;
	int neutrons;
	int electrons;
	int vElectrons;
	bool valence[8];
	double outerForce[8];
	unsigned short int pixelSize;
	bool setOnPass[8];

	void setOuterPressure(double value, OFP position);
	void setOuterPressure(double value[8]);

	void syncPressureAt(double value, OFP position);

public:
	Atom();
	
	/* Atom(protons) will assign all protons, neutrons, and electrons to that value
	*
	* @param protons +charge, 1 weight center of an atom
	* @param neutron ~charge 1.125 weight center of an atom
	* @param electron -charge 0.01 weight outer shell of an atom
	* @param x position on a grid for display
	* @param y position on grid for display
	* @param pixelSize for calculating display offset for (x, y)
	*/
	Atom(int protons, int neutrons = -1, int electrons = -1, int x = 0, int y = 0, unsigned short int pixelSize = 8);
	
	/* No protons/neutrons/electrons
	*/
	bool isEmpty();

	/* Set Protons/Neutrons/Electrons = 0
	*/
	void setEmpty();

	/* Copy only Protons/Neutrons/Electrons
	*/
	void setValue(Atom* atom);

	/* Render to the screen
	*
	* @param renderOffset spin on the electrons for display
	*/
	void draw(SDL_Renderer* ren, int renderOffset);

	/* Called before all the calculations and movement in the update function
	* currently does nothing. likely to change into something more useful
	*/
	void update();

	/* Difference in protons and electrons
	* +/- charge of an atom
	*/
	int charge();

	/* Mass of an atom*
	* creates instability in the atom
	* causes greater magnitude in most calculations
	*/
	double weight();

	//measure between neutrons & protons
	double neutronCharge();

	//stability of electrons
	double radialPressure();

	//stability of the nucleus
	double nucleoidPressure();

	//tertiary calculations
	double totalPressure();

	double measureOuterPressure(Atom* e);
	
	double outerForceAt(OFP position);

	//universe first runs setForceFor. turning setOnPass to true as it goes along
	void setForceFor(Atom* e, int x1, int y1, int x2, int y2);

	/* Outer force calculated to left-/right+ of an atom 
	*/
	double horizontalForce();

	/* Simple horizontal force -1/0/1
	* used for movement on the grid
	* see extra notes
	*/
	int dx();

	/* Outer force calculated to top-/bottom+ of an atom
	*/
	double verticalForce();
	
	/* Simple vertical force -1/0/1
 	* used for movement on the grid
	* see extra notes
	*/
	int dy();

	double topLeftForce();
	double topForce();
	double topRightForce();
	double rightForce();
	double botLeftForce();
	double botForce();
	double botRightForce();
	double leftForce();
	
	/* Add together forces from neighboring atoms
	*
	* this distinguishes the force between atoms in space
	* @param oa neighbors positioned in OFP order
	*
	*/
	void syncPressureWithNeighbors(Atom* oa[8]);
};


/*
	Extra notes

	Initial tests show most interactions in the system I have come up with only cause atoms
	to shift back and forth repeatedly or maintain their position.

	Adjusting force calculations here as well as the update function within Universe
	will be key in creating an interesting ruleset.


	Currently the universe will perform these actions on an update
	- atom -> update
	- atom -> updateOuterPressure
	- atom -> syncAtomPressureGrid
	- Universe -> moveAtoms
	
	Currently moveAtoms also only picks empty grid spaces to replace with
	the greatest force incoming to that grid space, so creating a more interesting
	movement method could easily make the atom response more meaningful.

	In future I would like to implement more features regarding the following
	- Atoms with too great a neucleoid-pressure/weight ratio will decay neutrons in the following ways
	  * "Fragment" a few neutrons leave the atom, and are to be absorbed by neighboring atoms. (lightweight decay)
	  * "Split" two equally sized atoms are created from atoms of great size and moderately high pressure
	  * "Explode" Many small parts are created from an atom of greate size and great pressure
    - Atoms with far too many electrons can decay a stray electron to interact with neighbors
	- Atoms respect multiple bonds better
	  * Currently the force calculation is very 1-1
	  * A secondary force calculation after the sync may be useful to determine what the overall force field looks like
	  * Atoms currently form too many bonds with neighbors
	    - a 7-1 valence electron bond should not attract multiple 1-7 neighbors
		- in general an atom should want 8 total valence elctrons on ALL sides
	- Atoms will respect impacting force better
		* if an atom is pushed in a direction with great force, it should interact with increased force to surrounding atoms
		* this could be a way to break strong bonds
		* atoms should be able to collide and explode if force is very great
*/