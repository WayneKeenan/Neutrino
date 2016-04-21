#Neutrino

Neutrino is a 2D shoot-em-up, originally concieved by Wayne Keenan, Nigel Rudgley and Gareth Noyce. It is also the name
of the framework/2D engine that will be used for this and future games from Triple Eh.

##Dependencies

Neutrino requires the following code to build and run:

- The GLM math library
- IMGUI, immediate mode GUI 
- Libconfig, C Library to parse config files. 
- GLEW, GL extension library.
- PhysFS, pack file manager

External dependencies can be brought into the project (via git clone) by running:

`./external_dependencies/SetupDeps.sh`

IMGUI & GLM are header only libraries so require no additional build steps.  

The external dependencies folder can be emptied by running:

`./external_dependencies/RemoveDeps.sh`

Libconfig, GLEW and PhysFS are package dependencies that should be installed via your package manager prior to running CMake generation. 

##Building

To build the game and engine:

- `cd build/`
- `./GenCmake.sh`
- `./MakePackfile.sh`
- `./BuildAndRun.sh`

This will leave the Neutrino executable in the build folder. To clean the build folder (remove executable and all cmake/make related files) run the following from the build folder:

`./CleanAll.sh` 

To make a release build, run:

- `cd build/`
- `./Release_GenCmake.sh`
- `./MakePackfile.sh`
- `./BuildAndRun.sh`


##Documentation

More to come.
