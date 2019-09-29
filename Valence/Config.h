#pragma once

const unsigned int UNIVERSE_SIZE = 32;

//master debug control.
const bool DEBUG = false;

//debugging options
const bool PRINT_ATOM_INIT = true;
const bool INCLUDE_EMPTY_INIT = false; // also prints empty atoms on init
const bool PRINT_BOND_CALCULATION = true;
const bool PRINT_UNIVERSE_ON_UPDATE = true;
const bool PRINT_MOVEMENT_CALCULATION = true;
const bool WAIT_ON_UPDATE = true;


//rendering options
const bool ELECTRON_SPIN = true;