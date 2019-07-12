Lux Engine
=======================
Lux Engine is a object based cross-platform game system. It uses a high level language to control each the of game entities.

Next Branch Goals
----------------------
- Replace Sprite and Font texture handling.
- Fix OpenGL/ES2 rendering 
- Move more editing function from the editor into the Lux Engine.
- Switch and 3ds ports


Requirements
----------------------
- SDL 2.0.3 *
- SDL_Mixer 2.0.0 * 
- OpenGL or OpenGL/ES

(*) Depending on Platform requirements

Included Source Code
----------------------
- Pawn
- TinyXML
- ENet
- Box2d

How to Build
----------------------
On fresh checkout, you may need to run 'make' to create configuration files required by the ninja build system..
Once that has been done, you just need to run 'ninja'.


