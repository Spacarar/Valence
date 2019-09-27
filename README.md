# Valence
Atomic Cellular Automata

Currently set up known to compile SDL2 for x64bit windows systems.
Requires linking the SDL2, SDL2_ttf, SDL2_mixer, SDL2_image libraries for your system.
Untested but likely could run on other systems as no platform specific code should be implemented.

# Summary

Built upon the ideas from Conways Game of Life and my general knowledge of chemistry, valence is a simplified cellular automata
of the universe composed from a grid of atoms. These atoms like regular atoms have protons neutrons and electrons.
In this game, the atoms have a valence shell that is balanced always at 8 valence electons. Valence electrons is derived from
the "outer shell" of the atom that is exposed, saying that other electrons are held within from the pull of the protons.
Most calculations of an atom take the protons/neutrons/electrons into account creating coefficients for charge, weight, radialPressure, and nucleoidPressure which are then used by the universe to generate forces between atoms.

The Universe is where the interaction happens. The atoms forces are all calculated 1-1 with each neighbor during an update,
then these neighboring forces are synced up so an x/y movement can be agreed upon between atoms.
Atoms that do well filling eachothers valence electrons to exactly 8 with the nearby neighbors should be attracted pulling eachother together.
Atoms that don't fit or are too unstable should push eachother away, and impact their neighbors causing disruption in the universe.

The general idea is to create a simplified space where simplified atoms will have unique interactions where which they 
bond and break apart according to their unique number of protons/neutrons/electrons within the atom and their unique neighbors.

# Where to start in the code

Refer to https://github.com/Spacarar/Valence/blob/master/Valence/Atom.h as this is the building block of the universe
and it has some notes about what is supposed to be implemented in upcoming builds.

The size of the universe is currently defined within https://github.com/Spacarar/Valence/blob/master/Valence/GameEngine.cpp

The constructor of universe is currently defining the exact layout of the universe. it is currently random.

The universe update function defines how the atoms interact with one another.
