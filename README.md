#Neutrino

Neutrino is a 2D shoot-em-up, originally concieved by Wayne Keenan, Nigel Rudgley and Gareth Noyce. It is also the name
of the framework/2D engine that will be used for this and future games from Triple Eh.

##Dependencies

Neutrino requires the following code to build and run:

- The GLM math library
- IMGUI, immediate mode GUI 
- FindSDL2 cmake modules
- GLEW, GL Extension manager

With the exception of GLEW, external dependencies can be brought into the project (via git clone) by running:

`./external_dependencies/SetupDeps.sh`

IMGUI & GLM are header only libraries so require no additional build steps.  GLEW should be installed via package manager, or windows binary. 

The external dependencies folder can be emptied by running:

`./external_dependencies/RemoveDeps.sh`

##Building

To build the game and engine:

- cd build/
- ./GenCmake.sh
- make

This will leave the Neutrino executable in the build folder. To clean the build folder (remove executable and all cmake/make related files) run the following from the build folder:

`./CleanAll.sh` 


##Documentation

More to come.