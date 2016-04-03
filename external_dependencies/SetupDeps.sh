#!/bin/bash
if [ ! -d ./glm ]; then
	git clone https://github.com/g-truc/glm.git ./glm
fi

if [ ! -d ./cmake-modules ]; then
	git clone https://github.com/tcbrindle/sdl2-cmake-scripts.git ./cmake-modules
fi

if [ ! -d ./imgui ]; then
	git clone https://github.com/ocornut/imgui.git ./imgui
fi
