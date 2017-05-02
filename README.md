# Neutrino

Neutrino is a 2D shoot-em-up, originally concieved by Wayne Keenan, Nigel Rudgley and Gareth Noyce. It is also the name
of the framework/2D engine that will be used for this and future games from Triple Eh.

## Dependencies

Neutrino requires the following code to build and run:

- SDL2, SDLImage, SDLMixer
- Box2D
- The GLM math library
- Libconfig, C Library to parse config files. 
- GLEW, GL extension library.
- PhysFS, pack file manager

External dependencies can be brought into the project (via git clone) by running:

`./external_dependencies/SetupDeps.sh`

GLM is a header only library so requires no additional build steps. 

Although the above script clones the latest IMGUI, it's already been manually copied into the Neutrino source folder. You probably don't need to update this to the newly downloaded version unless you have build errors. 

The latest version of GLEW will be cloned from git, so will require building and installing. The script will echo the build steps after cloning. Or checkout the Readme in the repro.  

On linux systems ensure you're running the latest version of Mesa. Neutrino only requires an OpenGL3.0 context but old Mesa is old. For Intel chipsets you can download the Ubuntu upgrade kit from: https://01.org/linuxgraphics/downloads

The external dependencies folder can be emptied by running:

`./external_dependencies/RemoveDeps.sh`

SDL2, SDLImage, SDLMixer, Libconfig and PhysFS are package dependencies that should be installed via your package manager prior to running CMake generation. 

Box2D can be cloned from GitHub. It includes a CMAKE file for compilation on Linux. See the README included in the Box2D Repo. At present this is not included in the SetupDeps.sh script (It's on my TODO).

## Building

To build the game and engine:

- `cd build/`
- `./GenCmake.sh`
- `./MakePackfile.sh`
- `./BuildAndRun.sh`

Or, alternatively:

- `cd build/`
- `./FullBiuld.sh`

This will leave the Neutrino executable in the build/bin folder. To clean the build folder (remove executable and all cmake/make related files) run the following from the build folder:

`./CleanAll.sh` 

To make a release build, run:

- `cd build/`
- `./Release_GenCmake.sh`
- `./MakePackfile.sh`
- `./BuildAndRun.sh`

## Windows

After manually installing the dependencies (you may have to compile Glew, and you'll definitely need to compile Box2D) you'll need to use the CMake GUI to locate the libraries and add include paths. Make sure
'advanced' is ticked and populate the filepaths as required. You should make sure /DDEBUG is added to the solution properties in order to 
compile and also flag the correct DPI Awareness setting in: "Project->Neutrino Properties->Manifest->Input & Output". "Per monitor DPI Awareness" will probably do what you expect. 

On first run Visual Studio will most likely have ALL_BUILD set as the start-up project. Right click on "Neutrino" and choose "Set as StartUp project" and you'll be able to run and debug from within VS.

## Status And licensing

TBD

## Documentation

### Tilemap editor

You can activate the tilemap editor by pressing F3 in Debug builds. 


## Key Commands

In DEBUG builds, the framework has the following shortcuts:
- F1: Toggle performance panel
- F3: Enter tile map editor (Current WIP)
- F: Toggle Fly cam when the Performance Panel / Debug Overlay is visible
