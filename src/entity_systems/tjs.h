/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _TINYJXENTITY_H_
	#define _TINYJXENTITY_H_
	#include "../stdheader.h"
	#include "../entity.h"


	/* Init */
	bool Lux_TinyJSEntity_Init(std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity);
	void Lux_TinyJSEntity_Destroy(void * entity_data);

	/* Loading/Saving */
	void Lux_TinyJSEntity_Restore(void * entity_data);
	void Lux_TinyJSEntity_Save(void * entity_data);

	/* Functions */
	bool Lux_TinyJSEntity_Run(void * entity_data, bool & scriptcontinue);
	bool Lux_TinyJSEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, mem_pointer memstack);
	bool Lux_TinyJSEntity_PushString( mem_pointer entity_data, std::string str, mem_pointer memstack);
	bool Lux_TinyJSEntity_Push( mem_pointer entity_data, int32_t size);
	int32_t Lux_TinyJSEntity_Call(mem_pointer entity_data, char * function, mem_pointer memstack);

#endif
