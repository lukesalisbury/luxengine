/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "entity_manager.h"
#include "mokoi_game.h"
#include "elix_string.h"
/*
#include "../../../ThirdParty/tinyjs/TinyJS.h"

typedef struct {
	CTinyJS s;
	std::string code;
} TinyJS;
*/
/** Lux_TinyJSCache_Init
* 
* \param 
*/
void Lux_TinyJSCache_Init()
{

}

/** Lux_TinyJSCache_Cleanup
* 
* \param 
*/
void Lux_TinyJSCache_Cleanup()
{

}

/** Lux_TinyJSEntity_Init
* Creates New Entity
* \param entity_id - 
* \param entity_base - 
*/
bool Lux_TinyJSEntity_Init(std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity)
{
	/*
	TinyJS * ent = new TinyJS;
	ent->s.addNative("function print(text)", &js_print);
	ent->s.addNative("function dump()", &js_dump);
	ent->string = "print(' hello world');"

	entity_data = ent;
	*/
	return true;
}

/** Lux_TinyJSEntity_Destroy
* Removes Entity data
* \param entity_data - 
*/
void Lux_TinyJSEntity_Destroy(mem_pointer entity_data)
{

}


/** Lux_TinyJSEntity_Restore
* 
* \param entity_data - 
*/
void Lux_TinyJSEntity_Restore(mem_pointer entity_data)
{

}

/** Lux_TinyJSEntity_Save
* 
* \param entity_data - Pointer to AMX
*/
void Lux_TinyJSEntity_Save(mem_pointer entity_data)
{

}

/** Lux_TinyJSEntity_Run
* 
* \param entity_data - 
*/
bool Lux_TinyJSEntity_Run(mem_pointer entity_data, bool & scriptcontinue )
{
	/*
	TinyJS * ent = entity_data;
	
	ent->s.execute(ent->string);
	*/
	return true;
}

/** Lux_TinyJSEntity_PushArray
*
*/
bool Lux_TinyJSEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, mem_pointer stack_mem)
{

	return true;
}

/** Lux_TinyJSEntity_PushString
*
*/
bool Lux_TinyJSEntity_PushString( mem_pointer entity_data, std::string str, mem_pointer stack_mem)
{

	return true;
}

/** Lux_TinyJSEntity_Push
*
*/
bool Lux_TinyJSEntity_Push( mem_pointer entity_data, int32_t number)
{

	return true;
}


/** Lux_TinyJSEntity_Call
* Calls a public function from the entity
* \param entity_data - 
* \param function -
* \param arg_count - 
*/
int32_t Lux_TinyJSEntity_Call( mem_pointer entity_data, char * function, mem_pointer memstack )
{
	return 0;
}

