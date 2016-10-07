#!/bin/bash
if [ ! -d ./glm ]; then
	echo "GLM not found, cloning: "
	git clone https://github.com/g-truc/glm.git ./glm
fi

#if [ ! -d ./cmake-modules ]; then
#	git clone https://github.com/tcbrindle/sdl2-cmake-scripts.git ./cmake-modules
#	echo "Copying FindLIBCONFIG.cmake to cmake modules"
#	cp ./tdi-cmake-modules/FindLibConfig.cmake ./cmake-modules/FindLIBCONFIG.cmake
#fi

if [ ! -d ./imgui ]; then
	echo "IMGUI not found, cloning: "
	git clone https://github.com/ocornut/imgui.git ./imgui
fi


if [ ! -d ./glew ]; then 
	echo "Glew not found, cloning: "
	git clone https://github.com/nigels-com/glew.git ./glew
	echo "For Glew to be installed do the following:"
	echo " cd ./glew/auto"
	echo " make"
	echo " cd .."
	echo " make && sudo make install"
fi


