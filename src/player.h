/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "luxengine.h"
#include "entity.h"
#include "object_effect.h"
#include "lux_sprite.h"

#define PLAYER_CONFIRM 16
#define PLAYER_CANCEL 17
#define PLAYER_POINTER 18

typedef enum {
	LOCAL,
	CPU,
	REMOTE
} Player_Control;

typedef struct Player_Button {
		InputDevice device;
		uint32_t device_number;
		int32_t sym;
		void set( InputDevice d, uint32_t n, int32_t s ) { device = d; device_number = n; sym = s; }
} Player_Button;

typedef struct Player_Axis {
		InputDevice device;
		uint32_t device_number;
		int32_t sym[6]; //
		void set( InputDevice d, uint32_t n, int32_t xn, int32_t xp, int32_t yn, int32_t yp, int32_t zn, int32_t zp ) {
			device = d; device_number = n; sym[0] = xn; sym[1] = xp; sym[2] = yn; sym[3] = yp; sym[4] = zn; sym[5] = zp;
		}
} Player_Axis;

typedef struct Player_Pointer {
		InputDevice device;
		uint32_t device_number;
		int32_t sym[4];
		void set( InputDevice d, uint32_t n, int32_t x, int32_t y ) { device = d; device_number = n; sym[0] = x; sym[2] = y; }
} Player_Pointer;

class Player
{
	public:
		Player(uint32_t id, uint8_t control = LOCAL);
		~Player();
	protected:
		uint8_t _control;
		uint16_t _id;
		uint16_t net_id;
		uint32_t timer;

		int16_t _button[20];
		Player_Button _buttonConfig[20];
		int16_t _controller[6]; /* X,Y,Z,A,B,C */
		Player_Axis _controllerConfig[2];
		int16_t _pointer[3]; /* x, y, active */
		Player_Pointer _pointerConfig;

		/* Entity */
		Entity * _entity;
		std::string _name;


	public:
		void Loop();

		/* Input */
		int16_t GetControllerAxis(uint8_t axis);
		int16_t GetButton(uint8_t key);
		int16_t GetPointer(uint8_t axis);
		void SetControllerAxis(uint8_t axis, int16_t value);
		void SetButton(uint8_t key, int16_t value);
		void SetPointer(uint8_t axis, int16_t value);

		void SetControls(uint8_t preset);
		void SetControls(std::string controller_name );

		LuxSprite * GetInputSprite( int8_t axis, int8_t key, int8_t pointer );
		MapObject * GetPointerObject();

		ObjectEffect PlayerColour;

		/* Entity */
		Entity * GetEntity();
		void SetEntity(Entity * entity);
		void SetEntityPostion(fixed x, fixed y, fixed z, uint32_t mapid);
		void SetName(std::string name);
		std::string GetName( );

		/* Network */
		void Message( int32_t * data, uint32_t size );

	protected:
		void ParsePointer( Player_Pointer * pointer, std::string config );
		void ParseButton( Player_Button * button, std::string config );
		void ParseAxis( Player_Axis * axis, std::string config );

		bool SetupController( std::string name );

		void CachePointerValues();
		void CacheAxisValues( uint8_t n );
		void CacheButtonValues( uint8_t n );

};
#endif
