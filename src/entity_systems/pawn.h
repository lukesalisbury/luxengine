/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _PAWNENTITY_H_
#define _PAWNENTITY_H_
#include "../stdheader.h"
#include "../entity.h"

int32_t Lux_PawnEntity_PublicVariable(AMX * amx, std::string varname, int32_t * new_value );

/* Init */
bool Lux_PawnEntity_Init(std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity);
void Lux_PawnEntity_Destroy(void * entity_data);

/* Parent Entity */
Entity * Lux_PawnEntity_GetParent(mem_pointer entity_data);

/* Loading/Saving */
void Lux_PawnEntity_Restore(elix::File * current_save_file, void * entity_data);
void Lux_PawnEntity_Save(elix::File * current_save_file, void * entity_data);

/* Functions */
bool Lux_PawnEntity_Run(void * entity_data, bool & scriptcontinue);
bool Lux_PawnEntity_PushArray(mem_pointer entity_data, int32_t  array[], uint32_t size, native_pointer memstack);
bool Lux_PawnEntity_PushString( mem_pointer entity_data, std::string str, native_pointer memstack);
bool Lux_PawnEntity_Push(mem_pointer entity_data, int32_t value);
int32_t Lux_PawnEntity_Call(mem_pointer entity_data, char * function, native_pointer memstack);

#ifdef __x86_64__
bool Lux_PawnEntity_PushArrayNative( mem_pointer entity_data, native_type array[], uint32_t size, mem_pointer memstack);
#endif

#endif
