 #pragma once

#include "Atom.h"
#include <iomanip>

/*
* Defines the laws of the universe
*
* Manages a grid of atoms determining rules for how they interact.
* space is the main universe for display, outerspace is used for creating the n+1 grid.
* space & outerspace pointers are swapped at the end of an update to complete the atoms calculated interactions.
*/
class Universe {
	int universeSize;
	Atom*** space;
	Atom*** outerSpace;
	
	/* Creates grid wrapping effect for exceeding array bounds
	*/
	int safeN(int n);
public:
	Universe();

	/* @param size is number of atoms
	* @param pixelSize display size of 1 unit (electron/nucleus) of the grid
	*/
	Universe(int size, int pixelSize = 8);

	/* Measures forces between each neighboring atom
	* as it goes through the grid. it will set "setOnPass" = true
	* for the positions that were measured from neighboring calculations
	* this saves calculations and will be used to do the same in reverse for the sync
	*/
	void updateAtomOuterPressure(int y, int x);

	/* Takes all measurements of outer force made individually
	* and adds them together for neighboring cells.
	* for example the left<->right or top<->bottom force are added together
	* all 4 members of the corner positions are added together
	* this will determine the overall forces appllied to the atoms
	*/
	void syncAtomPressureGrid(int y, int x);

	/* Uses the forces calculated to take action and make a movement plan for the next grid
	* This function is critical for interesting changes to occur
	* Changing the way this function works will highly affect the interactions
	*/
	void moveAtoms(int y, int x);

	/* Called before other update functions
	* currently does nothing but could be used to set initial values
	* or moved to the end of the update cycle to make more calculations
	*/
	void update();


	/* Prints Atoms as X's showing their measured force on all sides
	 The size of this grid will be 3N X 3N due to showing neighboring outer force cells
	*/
	void printUniverse();

	void draw(SDL_Renderer* ren);

	void handleEvent(SDL_Event e, SDL_Point m);
};