/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "mokoi_game.h"
#include "elix_string.hpp"
#include "display.h"

//TODO: Fix Memory leaks

#define AMX_PARENT 1162690895


std::map<const char *, AMX *> pawn_entities; /* Entity Cache */
uint8_t pawn_save_state_mode = LUX_SAVE_DATA_TYPE;

inline bool magiccheck(AMX_HEADER * hdr)
{
	return ( hdr->magic == AMX_MAGIC );
}



/**
 * @brief Lux_PawnCache_SetSaveMode
 * @param mode
 */
void Lux_PawnCache_SetSaveMode(uint8_t mode)
{
	pawn_save_state_mode = mode;
}


/**
 * @brief Lux_PawnEntity_StrError
 * @param errnum
 * @return Error message
 */
char * Lux_PawnEntity_StrError(unsigned int errnum)
{
	static char *messages[] = {
		/* AMX_ERR_NONE		*/ (char*)"(none)",
		/* AMX_ERR_EXIT		*/ (char*)"Forced exit",
		/* AMX_ERR_ASSERT	*/ (char*)"Assertion failed",
		/* AMX_ERR_STACKERR	*/ (char*)"Stack/heap collision (insufficient stack size)",
		/* AMX_ERR_BOUNDS	*/ (char*)"Array index out of bounds",
		/* AMX_ERR_MEMACCESS	*/ (char*)"Invalid memory access",
		/* AMX_ERR_INVINSTR	*/ (char*)"Invalid instruction",
		/* AMX_ERR_STACKLOW	*/ (char*)"Stack underflow",
		/* AMX_ERR_HEAPLOW	*/ (char*)"Heap underflow",
		/* AMX_ERR_CALLBACK	*/ (char*)"No (valid) native function callback",
		/* AMX_ERR_NATIVE	*/ (char*)"Native function failed",
		/* AMX_ERR_DIVIDE	*/ (char*)"Divide by zero",
		/* AMX_ERR_SLEEP	*/ (char*)"(sleep mode)",
		/* 13				*/ (char*)"(reserved)",
		/* 14				*/ (char*)"(reserved)",
		/* 15				*/ (char*)"(reserved)",
		/* AMX_ERR_MEMORY	*/ (char*)"Out of memory",
		/* AMX_ERR_FORMAT	*/ (char*)"Invalid/unsupported P-code file format",
		/* AMX_ERR_VERSION	*/ (char*)"File is for a newer version of the AMX",
		/* AMX_ERR_NOTFOUND	*/ (char*)"File or function is not found",
		/* AMX_ERR_INDEX	*/ (char*)"Invalid index parameter (bad entry point)",
		/* AMX_ERR_DEBUG	*/ (char*)"Debugger cannot run",
		/* AMX_ERR_INIT		*/ (char*)"AMX not initialized (or doubly initialized)",
		/* AMX_ERR_USERDATA	*/ (char*)"Unable to set user data field (table full)",
		/* AMX_ERR_INIT_JIT	*/ (char*)"Cannot initialize the JIT",
		/* AMX_ERR_PARAMS	*/ (char*)"Parameter error",
	};
	if ( errnum < 0 || errnum >= sizeof(messages) / sizeof(messages[0]) )
		return (char*)"(unknown)";
	return messages[errnum];
}

/**
 * @brief Deprecated Function for pawn if a function is removed or not implemented
 * @param amx
 * @param params
 * @return AMX_ERR_NOTFOUND
 */
cell pawnDeprecatedFunction(AMX *amx, const cell *params)
{
	return AMX_ERR_NOTFOUND;
}

/**
 * @brief Monitor Entity to see if runs to long.
 * @param amx
 * @return
 */
int Lux_PawnEntity_Monitor(AMX * amx)
{
	if ( ( amx->flags & AMX_FLAG_WATCH ) == AMX_FLAG_WATCH )
	{
		uint32_t t = lux::core->GetTime();
		Entity * p = Lux_PawnEntity_GetParent(amx);
		if ( t > p->starting_run_time + 120 )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Script '" << p->_base << "' running to long (" << (amx->function_name ? amx->function_name : "main")  << "). Force Sleep" << std::endl;
			p->sleeping = true;
			return AMX_ERR_SLEEP;
		}
	}
	return AMX_ERR_NONE;
}

/**
 * @brief Lux_PawnEntity_Register
 * @param amx
 * @return
 */
int32_t Lux_PawnEntity_Register( AMX * amx )
{
	amx_SetDebugHook(amx, Lux_PawnEntity_Monitor);

	extern const AMX_NATIVE_INFO core_Natives[];
	extern const AMX_NATIVE_INFO string_Natives[];
	extern const AMX_NATIVE_INFO float_Natives[];
	extern const AMX_NATIVE_INFO fixed_Natives[];
	extern const AMX_NATIVE_INFO Graphics_Natives[];
	extern const AMX_NATIVE_INFO Functions_Natives[];
	extern const AMX_NATIVE_INFO Entity_Natives[];
	extern const AMX_NATIVE_INFO Player_Natives[];
	extern const AMX_NATIVE_INFO Network_Natives[];
	extern const AMX_NATIVE_INFO Online_Natives[];
	extern const AMX_NATIVE_INFO Time_Natives[];
	extern const AMX_NATIVE_INFO console_Natives[];
	extern const AMX_NATIVE_INFO Maps_Natives[];
	extern const AMX_NATIVE_INFO System_Natives[];
	extern const AMX_NATIVE_INFO Game_Natives[];
	extern const AMX_NATIVE_INFO Input_Natives[];

	amx_Register(amx, Maps_Natives, -1);
	amx_Register(amx, Entity_Natives, -1);
	amx_Register(amx, float_Natives, -1);
	amx_Register(amx, string_Natives, -1);
	amx_Register(amx, fixed_Natives, -1);
	amx_Register(amx, Graphics_Natives, -1);
	amx_Register(amx, Functions_Natives, -1);
	amx_Register(amx, Player_Natives, -1);
	amx_Register(amx, Network_Natives, -1);
	amx_Register(amx, Online_Natives, -1);
	amx_Register(amx, Time_Natives, -1);
	amx_Register(amx, console_Natives, -1);
	amx_Register(amx, System_Natives, -1);
	amx_Register(amx, Game_Natives, -1);
	amx_Register(amx, Input_Natives, -1);

	return amx_Register(amx, core_Natives, -1);
}

/**
 * @brief Lux_PawnEntity_LoadFile
 * @param entity_name
 * @return
 */
AMX * Lux_PawnEntity_LoadFile( const char * entity_name )
{
	AMX * temp_amx = NULL;
	std::string file_name = "./c/scripts/";
	file_name.append(entity_name);
	#if PLATFORMBITS == 64
	file_name.append(".amx64");
	#else
	file_name.append(".amx");
	#endif

	//bool successful = false;
	uint8_t * temp_block = NULL;

	if ( lux::game_data->HasFile( file_name ) )
	{
		uint32_t bytes_read = 0;
		uint8_t * mem_block = NULL;
		int32_t result = 0;

		AMX_HEADER hdr;


		bytes_read = lux::game_data->GetFile( file_name, &temp_block, false );
		if ( bytes_read > sizeof(hdr) )
		{
			memcpy( &hdr, temp_block, sizeof(hdr) );
		}
		else
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << "Error " << entity_name << ": " << Lux_PawnEntity_StrError(AMX_ERR_FORMAT) << std::endl;
			goto function_exit;
		}

		amx_Align16(&hdr.magic);
		amx_Align32((uint32_t *)&hdr.size);
		amx_Align32((uint32_t *)&hdr.stp);

		if ( hdr.magic != AMX_MAGIC )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << "Error " << entity_name << ": " << Lux_PawnEntity_StrError(AMX_ERR_FORMAT) << std::endl;
			goto function_exit;
		}

		temp_amx = new AMX;
		mem_block = new uint8_t[hdr.stp]; // amx->base

		memcpy( mem_block, temp_block, hdr.size );
		memset( temp_amx, 0, sizeof(AMX) );

		result = amx_Init(temp_amx, mem_block);

		if ( result )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Error " << entity_name << ": " << Lux_PawnEntity_StrError(result) << std::endl;
		}

		result = Lux_PawnEntity_Register( temp_amx );
		if ( result  )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Error " << entity_name << ": " << Lux_PawnEntity_StrError(result) << std::endl;
		}
		else
		{
			//check for overwrite
			//pawn_entities[entity_name] = temp_amx;
			//successful = true;



			goto function_exit;
		}

		delete temp_amx;
		delete[] mem_block;

		temp_amx = NULL;
	}
	else
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | Lux_PawnEntity_LoadFile Error " << entity_name << ": File not found" << std::endl;
	}


	function_exit:

	NULLIFY_ARRAY( temp_block );

	return temp_amx;
}

/** Lux_PawnCache_Init
*
* \param
*/
void Lux_PawnCache_Init()
{

}

/** Lux_PawnCache_Cleanup
*
* \param
*/
void Lux_PawnCache_Cleanup()
{
	/*
	if ( pawn_entities.size() )
	{
		while ( pawn_entities.begin() != pawn_entities.end() )
		{
			AMX * amx = (AMX *)pawn_entities.begin()->second;
			amx_SetDebugHook(amx, NULL);
			if( amx->base != NULL )
			{
				amx_Cleanup( amx );
				//delete [] amx->base; //Crashes
				amx->base = NULL;
			}
			delete amx;

			pawn_entities.erase( pawn_entities.begin() );
		}
	}
	*/
}

/**
 * @brief Lux_PawnEntity_GetBaseAMX
 * @param entity_base
 * @return
 */
AMX * Lux_PawnEntity_GetBaseAMX( const char * entity_base )
{
	AMX * base_amx = NULL;

	std::map<const char *, AMX *>::iterator iter = pawn_entities.find(entity_base);

	if ( iter != pawn_entities.end() )
	{
		base_amx = iter->second;
	}
	else
	{
		base_amx = Lux_PawnEntity_LoadFile(entity_base);
		if( base_amx != NULL )
		{
			pawn_entities[entity_base] = base_amx;
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Entity not found '" << entity_base << "'" << std::endl;
		}

	}

	return base_amx;
}

/**
 * @brief Creates New Entity from a Pawn Script
 * @param entity_id
 * @param entity_base
 * @param entity
 * @return
 */
mem_pointer Lux_PawnEntity_Init( const char * entity_id, const char * entity_base, Entity * entity )
{
	AMX * entity_data = NULL;

#if PAWNCLONABLE
	long datasize = 0, stackheap = 0;
	AMX * base_amx = NULL;

	uint8_t * memory_block = NULL;
	int32_t result = 0;

	base_amx = Lux_PawnEntity_GetBaseAMX( entity_base );

	if ( base_amx != NULL )
	{
		result = amx_MemInfo( base_amx, NULL, &datasize, &stackheap);
		if ( result )
		{
			lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | AMX Clone Failed: "  << Lux_PawnEntity_StrError(result) << "." << std::endl;
			MessagePush( (char*)"%s: [Clone Error] %s", entity->_base.c_str(), Lux_PawnEntity_StrError(result) );
		}
		else
		{
			entity_data = new AMX;
			memory_block = new uint8_t[datasize + stackheap];

			memset(entity_data, 0, sizeof(AMX));
			memset(memory_block, 0, datasize + stackheap);


			int32_t result = amx_Clone(entity_data, base_amx, memory_block);
			if ( result )
			{
				lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | AMX Clone Failed: "  << Lux_PawnEntity_StrError(result) << "." << std::endl;
				MessagePush( (char*)"%s: [Clone Error] %s", entity->_base.c_str(), Lux_PawnEntity_StrError(result) );

				NULLIFY(entity_data);
				NULLIFY_ARRAY(memory_block);
			}
			else
			{
				entity_data->parent = entity;
				cell * internal_ident = NULL;
				if ( amx_FindPubVar( entity_data, "internal_ident", &internal_ident) == AMX_ERR_NONE )
				{
					*internal_ident = entity->hashid;
				}
			}
		}
	}
#else
	entity_data = Lux_PawnEntity_LoadFile( entity_base );

	if ( entity_data != NULL )
	{
		entity_data->parent = entity;

		cell * internal_ident = NULL;
		if ( amx_FindPubVar( entity_data, "internal_ident", &internal_ident) == AMX_ERR_NONE )
		{
			*internal_ident = entity->hashid;
		}
	}

#endif
	return static_cast<mem_pointer>(entity_data);
}

/**
 * @brief Lux_PawnEntity_QuickReload
 * @param entity_data
 */
void Lux_PawnEntity_QuickReload(Entity * entity)
{
	std::cout << "Lux_PawnEntity_QuickReload" << std::endl;
	if ( entity )
	{
		AMX * new_amx = NULL;
		AMX * amx = static_cast<AMX*>(entity->_data);
		uint8_t * old_data;
		long int new_header_area_size = 1, old_header_area_size = 0;

		amx_MemInfo(amx, NULL, &old_header_area_size, NULL);

		if ( new_header_area_size > 0 )
		{
			AMX_HEADER * hdr = (AMX_HEADER *)amx->base;
			uint8_t * data = (amx->data != NULL) ? amx->data : amx->base+(int)hdr->dat;

			old_data = new uint8_t[old_header_area_size];
			memcpy(old_data, data, old_header_area_size);


			new_amx = Lux_PawnEntity_LoadFile( entity->_base.c_str() );

			if ( new_amx != NULL )
			{

				amx_MemInfo(amx, NULL, &new_header_area_size, NULL);
				std::cout << entity->_base << " " << old_header_area_size << "-" << new_header_area_size << std::endl;
				if (old_header_area_size == new_header_area_size)
				{
					hdr = (AMX_HEADER *)new_amx->base;
					data = (new_amx->data != NULL) ? new_amx->data : new_amx->base+(int)hdr->dat;
					new_amx->parent = entity;

					memcpy(data, old_data, new_header_area_size);

					entity->_data = static_cast<mem_pointer>(new_amx);
					Lux_PawnEntity_Destroy(amx);
				}
				else
				{
					Lux_PawnEntity_Destroy(new_amx);
				}

			}
		}
	}
}

/**
 * @brief Removes Entity data
 * @param entity_data
 */
void Lux_PawnEntity_Destroy(mem_pointer entity_data)
{
	if ( entity_data )
	{
		AMX * entity = static_cast<AMX*>(entity_data);

		amx_SetDebugHook(entity, NULL);
		if ( entity->data != NULL )
		{
			NULLIFY_ARRAY(entity->data);
		}
//		if ( entity->base != NULL )
//		{
//			NULLIFY_ARRAY(entity->base);
//			memset(entity, 0, sizeof(AMX));
//		}
		delete entity;
	}
}

/**
 * @brief Lux_PawnEntity_GetParent
 * @param entity_data
 * @return AMX's Parent Entity
 */
Entity * Lux_PawnEntity_GetParent(AMX * entity_data)
{
	if ( !entity_data )
	{
		return NULL;
	}
	return static_cast<Entity*>(entity_data->parent);
}

/** Lux_PawnEntity_Load
*
* \param entity_data -
*/
// TODO
void Lux_PawnEntity_Restore(elix::File * current_save_file, mem_pointer entity_data)
{
	AMX * amx = (AMX*)entity_data;
	uint32_t saved_data_size = 0;

	if ( pawn_save_state_mode == LUX_SAVE_HIBERNATE_TYPE )
	{
		uint32_t unsigned_header_area_size = 0;
		long int header_area_size = 1;

		saved_data_size = current_save_file->ReadUint32WithLabel( "Entity Header Size", true );

		amx_MemInfo(amx, NULL, &header_area_size, NULL);
		unsigned_header_area_size = (uint32_t)header_area_size;

		if ( unsigned_header_area_size == 0 )
		{

		}
		else if ( saved_data_size == unsigned_header_area_size )
		{
			//memset(amx->data, 44, saved_data_size);
			current_save_file->ReadWithLabel( "Entity Header Data", (data_pointer)amx->data, sizeof(uint8_t), saved_data_size);
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Lux_PawnEntity_Restore: Error restoring data. Skipping" << std::endl;
			current_save_file->Seek( current_save_file->Tell() + (int32_t)saved_data_size );
		}
	}
	else if ( pawn_save_state_mode == LUX_SAVE_DATA_TYPE )
	{
		uint32_t count = 0;

		saved_data_size = current_save_file->ReadUint32WithLabel("Entity Data Size",  true );

		if ( saved_data_size )
		{
			for ( ; count < saved_data_size; count++ )
			{
				std::string variable_name = current_save_file->ReadStringWithLabel("Public Variable Name" );
				int32_t variable_value = (int32_t)current_save_file->ReadUint32WithLabel("Public variable", true );
				Lux_PawnEntity_PublicVariable(amx, variable_name, &variable_value );

			}
		}
	}
}

/** Lux_PawnEntity_Save
*
* \param entity_data - Pointer to AMX
*/
// TODO
void Lux_PawnEntity_Save(elix::File * current_save_file, mem_pointer entity_data)
{
	AMX * amx = (AMX*)entity_data;

	if ( pawn_save_state_mode == LUX_SAVE_HIBERNATE_TYPE )
	{
		uint32_t unsigned_header_area_size = 0;
		long int header_area_size = 1;

		amx_MemInfo(amx, NULL, &header_area_size, NULL);

		if ( header_area_size > 0 )
		{
			AMX_HEADER * hdr = (AMX_HEADER *)amx->base;
			uint8_t * data = (amx->data != NULL) ? amx->data : amx->base+(int)hdr->dat;

			unsigned_header_area_size = (uint32_t)header_area_size;

			current_save_file->WriteWithLabel( "Entity Header Size", unsigned_header_area_size );
			current_save_file->WriteWithLabel( "Entity Header Data",  (mem_pointer) data, sizeof(uint8_t), unsigned_header_area_size );
		}
		else
		{
			current_save_file->WriteWithLabel( "Entity Header Size", unsigned_header_area_size );
		}
	}
	else
	{
		cell * variable_value_address;
		uint32_t variable_value;
		int32_t count = 0;
		int32_t number_public_variables = 0;
		char variable_name[sNAMEMAX+1];

		amx_NumPubVars(amx, (int*)&number_public_variables);

		current_save_file->WriteWithLabel("Entity Data Size", (uint32_t)number_public_variables );
		if ( number_public_variables > 0 )
		{
			for ( ; count < number_public_variables; count++ )
			{
				amx_GetPubVar( amx, count, variable_name, &variable_value_address);
				variable_value = *variable_value_address;

				current_save_file->WriteWithLabel("Public Variable Name", variable_name, sizeof(uint8_t), strlen(variable_name) + 1 );
				current_save_file->WriteWithLabel("Public variable", variable_value );
			}
		}

	}
}




/** Lux_PawnEntity_Run
*
* \param entity_data -
*/
bool Lux_PawnEntity_Run(mem_pointer entity_data, bool & scriptcontinue )
{
	if ( !entity_data )
		return false;

	int32_t error = 0;
	AMX * amx = static_cast<AMX *>(entity_data);
	AMX_HEADER * hdr = (AMX_HEADER *)amx->base;
	Entity * p = Lux_PawnEntity_GetParent(amx);

	if ( hdr->magic == AMX_MAGIC )
	{
		p->starting_run_time = lux::core->GetTime();
		amx->flags |= AMX_FLAG_WATCH;
		error = amx_Exec( amx, NULL, (scriptcontinue ? AMX_EXEC_CONT : AMX_EXEC_MAIN));
	}
	else
	{
		error = 13;
	}

	if ( error )
	{
		if ( error == AMX_ERR_SLEEP )
		{
			scriptcontinue = true;
			return true;
		}
		else
		{
			MessagePush( (char*)"%s:%d [ID:%s] %s [%s] ", p->_base.c_str(), 0, p->id.c_str(), Lux_PawnEntity_StrError(error), amx->function_name );

			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Entity:" << p->_base << "[ID:" << p->id << "]";
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Main Runtime Error - " << Lux_PawnEntity_StrError(error) << std::endl;
			return false;
		}
	}
	scriptcontinue = false;
	return true;
}

#if PLATFORMBITS == 64
/**
 * @brief Lux_PawnEntity_PushArrayNative
 * @param entity_data
 * @param array
 * @param size
 * @param stack_mem
 * @return
 */
bool Lux_PawnEntity_PushArrayNative( mem_pointer entity_data, native_type array[], uint32_t size, native_pointer stack_mem)
{
	if ( !entity_data )
		return false;


	#if PAWN_VERSION == 4
	int32_t err = amx_PushArray((AMX*)entity_data, (cell**)&stack_mem, (cell*)array, size);
	#else
	int32_t err = amx_PushArray((AMX*)entity_data, (cell*)stack_mem, NULL, (cell*)array, size);
	#endif
	if ( err )
	{
		lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_ERROR ) <<  "| " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	return true;
}
#endif


/**
 * @brief Lux_PawnEntity_PushArray
 * @param entity_data
 * @param array
 * @param size
 * @param stack_mem
 * @return
 * Note: 64bit pawn, expects int64_t instead of int32_t that engine offers so we need to convert them.
 * It also means that you can't get a array back with this method on 64 bit systems
 */
bool Lux_PawnEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, native_pointer stack_mem)
{
	if ( !entity_data && size == 0 )
		return false;
	native_type * native_array = NULL;

	#if PLATFORMBITS == 64
	native_array = new native_type[size];
	uint32_t count = 0;
	while (count < size)
	{
		native_array[count] = array[count];
	}
	#else
	native_array = (native_type *)array;
	#endif

	int32_t err = amx_PushArray((AMX*)entity_data, (cell**)&stack_mem, (cell*)native_array, size);
	if ( err )
	{
		lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_ERROR ) <<  "| " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	else
	{
#if PLATFORMBITS == 64
		count = 0;
		while (count < size)
		{
			array[count] = (int32_t)native_array[count];
		}
		delete[] native_array;
#endif
	}
	return true;
}

/** Lux_PawnEntity_PushString
*
*/
bool Lux_PawnEntity_PushString(mem_pointer entity_data, std::string str, native_pointer stack_mem)
{
	if ( !entity_data )
		return false;

	int32_t err = amx_PushString((AMX*)entity_data, (cell**)&stack_mem, str.c_str(), 0, 0);
	if ( err )
	{
		lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_ERROR ) <<  "| " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	return true;
}

/** Lux_PawnEntity_Push
*
*/
bool Lux_PawnEntity_Push( mem_pointer entity_data, int32_t number)
{
	if ( !entity_data )
		return false;
	cell native = (cell)number;
	int32_t err = amx_Push((AMX*)entity_data, native );

	if ( err )
	{
		lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_ERROR ) <<  "| " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	return true;
}


/**
 * @brief Calls a public function from the entity
 * @param entity_data
 * @param function
 * @param memstack
 * @return
 */
int32_t Lux_PawnEntity_Call( mem_pointer entity_data, char * function, native_pointer memstack )
{
	if ( !entity_data || !function )
		return false;
	int32_t index = -1;
	int32_t error;
	cell return_type = -1;

	AMX * amx = static_cast<AMX *>(entity_data);
	AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
	Entity * p = Lux_PawnEntity_GetParent(amx);

	if ( hdr->magic == AMX_MAGIC )
	{
		error = amx_FindPublic(amx, function, (int*)&index);
	}
	else
	{
		error = 13;
	}

	if ( error )
	{
		if ( function[0] != '@' )
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Entity:" << p->_base << " - '" << function << "' not found." << std::endl;
		return_type = -2;
	}
	else
	{
		amx->flags &= ~AMX_FLAG_WATCH;
		error = amx_Exec(amx, &return_type, index);
		if ( error )
		{
			MessagePush( (char*)"%s:%d [ID:%s] %s [%s] ", p->_base.c_str(), 0, p->id.c_str(), Lux_PawnEntity_StrError(error), amx->function_name );

			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Entity:" << p->_base << "[ID:" << p->id << "] ";
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Public Runtime Error  [" << function << "]" << Lux_PawnEntity_StrError(error) << std::endl;
		}

	}

	if ( memstack )
	{
		#ifdef HAVE_X86_64
		uintptr_t q = (uintptr_t)memstack;
		if ( amx->hea > q)
			amx->hea = q;
		#else
		amx_Release( amx, (cell*)memstack );
		#endif
	}
	return (int32_t)return_type;
}

/** Lux_PawnEntity_HasString
* Checks to see if string contains something.
* \param amx - Pointer to current pawn script
* \param str_param - Script parameter to string
*/
uint32_t Lux_PawnEntity_HasString(AMX * amx, cell str_param)
{
	cell * amx_cstr;
	int amx_length;
	amx_cstr = amx_Address(amx, str_param);
	amx_StrLen(amx_cstr, &amx_length);
	return amx_length;
}

/** Lux_PawnEntity_GetEntity
* Checks to see if string contains something.
* \param amx - Pointer to current pawn script
* \param text_param - parameter to Text
* \param length - length of
* \return Entity object or NULL
*/
Entity * Lux_PawnEntity_GetEntity(AMX *amx, cell text_param )
{
	Entity * wanted = NULL;
	uint32_t hash = (uint32_t)text_param;

	if ( hash )
	{
		wanted = lux::entities->GetEntity(hash);
	}
	else
	{
		wanted = Lux_PawnEntity_GetParent(amx);
	}

	return wanted;
}


/** Lux_PawnEntity_GetEntityHash
* Checks to see if string contains something.
* \param amx - Pointer to current pawn script
* \return Entity Hash
*/
uint32_t Lux_PawnEntity_GetEntityHash(AMX *amx)
{
	uint32_t hash = 0;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);

	if ( wanted )
	{
		hash = wanted->hashid;
	}


	return hash;
}


/** Lux_PawnEntity_SetString
* Checks to see if string contains something.
* \param dest - Pointer to current pawn script
* \param source - parameter to Text
* \param size - length of
* \return error code
*/
size_t Lux_PawnEntity_SetString(cell * dest, const char * source, size_t dest_size )
{
	size_t length, copy_length;

	length = strlen(source);
	copy_length = std::min( length, dest_size );

	// Packed Strings
	memset( dest, 0, dest_size );
	memcpy( dest, source, copy_length );

	#if BYTE_ORDER==LITTLE_ENDIAN
	dest_size /= sizeof(cell);
	for ( size_t count = 0; count < dest_size; count++ )
		amx_SwapCell( (ucell *)&dest[count] );

	//for (i=(int)(len/sizeof(cell)); i>=0; i--)
	//	amx_SwapCell((ucell *)&dest[i]);
	#endif

	// Unpacked Strings
	//for (size_t i=0; i<length; i++)
	//	dest[i]=(cell)source[i];

	return copy_length;
}

/**
 * @brief Checks to see if string contains something.
 * @param amx - Pointer to current pawn script
 * @param param - parameter to Text
 * @return
 */
std::string Lux_PawnEntity_GetString(AMX * amx, cell param)
{
	std::string str = "";
	cell * cstr = amx_Address( amx, param );

	if (cstr)
	{
		if ( (ucell)*cstr > UNPACKEDMAX )
		{
			/* source string is packed */
			uint32_t i = sizeof(ucell)-1;
			ucell c = 0;
			uint8_t char8 = amx_get_next_packed( cstr, c, i );
			while ( char8 != '\0')
			{
				if ( char8 < 128 )
				{
					str.append( 1, char8 );
				}
				else if ( char8 < 194 )
				{
					return str; // Error
				}
				else if ( char8 < 224 )
				{
					str.append( 1, char8 );
					str.append( 1, amx_get_next_packed( cstr, c, i ) );
				}
				else if ( char8 < 240 )
				{
					str.append( 1, char8 );
					str.append( 1, amx_get_next_packed( cstr, c, i ) );
					str.append( 1, amx_get_next_packed( cstr, c, i ) );
				}
				else if ( char8 < 245 )
				{
					str.append( 1, char8 );
					str.append( 1, amx_get_next_packed( cstr, c, i ) );
					str.append( 1, amx_get_next_packed( cstr, c, i ) );
				}
				else
				{
					return str; // Error
				}

				char8 = amx_get_next_packed( cstr, c, i );
			}
		}
		else
		{
			while ( *cstr != 0 )
			{
				str.append( 1, (char)*cstr++ );
			}
		}
	}
	return str;
}

/** Lux_PawnEntity_PublicVariable
* Checks to see if string contains something.
* \param dest - Pointer to current pawn script
* \param source - parameter to Text
* \param size - length of
* \return value
*/
int32_t Lux_PawnEntity_PublicVariable(AMX * amx, std::string varname, int32_t * new_value )
{
	int32_t ret = 0;
	cell * cptr = NULL;
	#if PAWN_VERSION == 4

	if ( amx_FindPubVar( amx, varname.c_str(), &cptr) == AMX_ERR_NONE )
	{
		if ( new_value )
		{
			*cptr = *new_value;
			ret = 1;
		}
		else
		{
			ret = *cptr;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
	#else
	cell address;
	if ( amx_FindPubVar( amx, varname.c_str(), &address) == AMX_ERR_NONE )
	{
		cptr = amx_Address( amx, address );
		if ( new_value )
		{
			*cptr = *new_value;
			ret = 1;
		}
		else
		{
			ret = *cptr;
		}
	}
	else
	{
		ret = 0;
	}
	#endif

	return ret;


}

