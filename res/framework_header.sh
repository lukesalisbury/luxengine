#!/bin/sh


#rm -r ../resources/Frameworks/SDL_gfx.framework/Headers/SDL

mkdir -p ../resources/Frameworks/Headers/SDL/
cp -v ../resources/Frameworks/SDL_gfx.framework/Versions/A/Headers/* ../resources/Frameworks/Headers/SDL/
cp -v ../resources/Frameworks/SDL.framework/Versions/A/Headers/* ../resources/Frameworks/Headers/SDL/
cp -v ../resources/Frameworks/SDL_mixer.framework/Versions/A/Headers/* ../resources/Frameworks/Headers/SDL/
cp -v ../resources/Frameworks/SDL_image.framework/Versions/A/Headers/* ../resources/Frameworks/Headers/SDL/
