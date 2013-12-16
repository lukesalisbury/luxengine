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
#include "pawn_helper.h"
#include "mokoi_game.h"
#include "elix_string.hpp"
#include "display.h"
//#include "../amx4/osdefs.h"

std::map<std::string, AMX *> pawn_entities; /* Entity Cache */
uint8_t pawn_save_state_mode = LUX_SAVE_DATA_TYPE;

/** Lux_PawnCache_SetSaveMode
*
* \param mode -
*/
void Lux_PawnCache_SetSaveMode(uint8_t mode)
{
	pawn_save_state_mode = mode;
}


/** Lux_PawnEntity_StrError
*
* \param errnum -
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
/** pawnDeprecatedFunction
*
*
*/
cell AMX_NATIVE_CALL pawnDeprecatedFunction(AMX *amx, const cell *params)
{
	return AMX_ERR_NOTFOUND;
}

/** Lux_PawnEntity_Monitor
*
*/
int AMXAPI Lux_PawnEntity_Monitor(AMX * amx)
{
	Entity * entity;
	uint32_t time = lux::core->GetTime();

	amx_GetUserData(amx, 1162690895, (void**)&entity);
	if ( entity )
	{
		if ( time > entity->starting_run_time + 2000 )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Script '" << entity->_base << "' running to long" << std::endl;
			//return AMX_ERR_EXIT;
		}
	}
	return AMX_ERR_NONE;
}

/** Lux_PawnEntity_LoadFile
*
* \param entity_id -
* \param entity_base -
*/
bool Lux_PawnEntity_LoadFile(std::string entity_name)
{
	#ifdef __x86_64__
	std::string file = "./c/scripts/" + entity_name + ".amx64";
	#else
	std::string file = "./c/scripts/" + entity_name + ".amx";
	#endif

	uint8_t * tempblock = NULL;
	uint8_t * memblock = NULL;

	AMX_HEADER hdr;
	int32_t result = 0;

	if ( lux::game->HasFile( file ) )
	{
		uint32_t read = lux::game->GetFile( file, &tempblock, false );
		if ( read > sizeof(hdr) )
		{
			memcpy( &hdr, tempblock, sizeof(hdr) );
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Lux_PawnEntity_LoadFile Error: " << file << " " << std::endl;
			return false;
		}

		amx_Align16(&hdr.magic);
		amx_Align32((uint32_t *)&hdr.size);
		amx_Align32((uint32_t *)&hdr.stp);

		if ( hdr.magic != AMX_MAGIC )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Lux_PawnEntity_LoadFile Error " << entity_name << ": " << Lux_PawnEntity_StrError(AMX_ERR_FORMAT) << std::endl;
			delete[] tempblock;
			return false;
		}

		memblock = new uint8_t[hdr.stp];
		memcpy( memblock, tempblock, hdr.size );
		delete[] tempblock;

		AMX * temp_amx = new AMX;
		memset(temp_amx, 0, sizeof(AMX) );
		result = amx_Init(temp_amx, memblock);

		if ( result )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Lux_PawnEntity_LoadFile Error " << entity_name << ": " << Lux_PawnEntity_StrError(result) << std::endl;
			delete[] memblock;
			return false;
		}

		//amx_SetDebugHook(temp_amx, Lux_PawnEntity_Monitor);

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

		amx_Register(temp_amx, Maps_Natives, -1);
		amx_Register(temp_amx, Entity_Natives, -1);
		amx_Register(temp_amx, float_Natives, -1);
		amx_Register(temp_amx, string_Natives, -1);
		amx_Register(temp_amx, fixed_Natives, -1);
		amx_Register(temp_amx, Graphics_Natives, -1);
		amx_Register(temp_amx, Functions_Natives, -1);
		amx_Register(temp_amx, Player_Natives, -1);
		amx_Register(temp_amx, Network_Natives, -1);
		amx_Register(temp_amx, Online_Natives, -1);
		amx_Register(temp_amx, Time_Natives, -1);
		amx_Register(temp_amx, console_Natives, -1);

		int error = amx_Register(temp_amx, core_Natives, -1);

		if ( error )
		{
			std::cerr << "Entity:" << entity_name << " - Creation Error " << Lux_PawnEntity_StrError(error) << std::endl;
			return false;
		}
		else
		{
			pawn_entities[entity_name] = temp_amx;
			return true;
		}
	}
	std::cerr << __FILE__ << ":" << __LINE__ << " | Lux_PawnEntity_LoadFile Error " << entity_name << ": File not found" << std::endl;
	return false;
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
	if ( pawn_entities.size() )
	{
		std::map<std::string, AMX *>::iterator p;

		AMX * amx = NULL;

		for( p = pawn_entities.begin(); p != pawn_entities.end(); p++ )
		{
			amx = (*p).second;
			if( amx->base != NULL )
			{
				amx_Cleanup( amx );
				//free( amx->base );
				amx->base = NULL;
			}
			amx_SetDebugHook(amx, NULL);

		}
	}
	pawn_entities.clear();

}

/** Lux_PawnEntity_Init
* Creates New Entity
* \param entity_id -
* \param entity_base -
*/
bool Lux_PawnEntity_Init(std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity)
{
	long datasize = 0, stackheap = 0;
	AMX * base_amx = NULL;

	std::map<std::string, AMX *>::iterator iter = pawn_entities.find(entity_base);

	if ( iter != pawn_entities.end() )
	{
		base_amx = iter->second;
	}
	else
	{
		if( !Lux_PawnEntity_LoadFile(entity_base) )
		{
			std::cerr << __FILE__ << ":" << __LINE__ << " | No AMX found for "  << entity_base << std::endl;
			MessagePush( (char*)"Entity not found '%s'", entity_base.c_str());
			return false;
		}
		base_amx = pawn_entities[entity_base];
	}

	if ( base_amx != NULL )
	{
		mem_pointer memblock = NULL;
		amx_MemInfo(base_amx, NULL, &datasize, &stackheap);
		entity_data = (mem_pointer)new AMX;
		memblock = new mem_pointer[datasize + stackheap];
		memset((AMX *)entity_data, 0, sizeof(AMX));
		memset(memblock, 0, datasize + stackheap);

		int32_t result = amx_Clone((AMX *)entity_data, base_amx, memblock);
		if ( result )
		{
			std::cerr << __FILE__ << ":" << __LINE__ << " | AMX Clone Failed: "  << Lux_PawnEntity_StrError(result) << "." << std::endl;
			MessagePush( (char*)"%s: [Error] %s", entity->_base.c_str(), Lux_PawnEntity_StrError(result) );
			entity_data = NULL;
			return false;
		}
		amx_SetUserData((AMX *)entity_data, 1162690895, (void*)entity);
		amx_SetUserData((AMX *)entity_data, 1162690890, strdup(entity_base.c_str()) );
		return true;
	}
	std::cerr << __FILE__ << ":" << __LINE__ << " | Lux_PawnEntity_Init FAILED" << std::endl;
	return false;
}

/** Lux_PawnEntity_Destroy
* Removes Entity data
* \param entity_data -
*/
void Lux_PawnEntity_Destroy(mem_pointer entity_data)
{
	if ( entity_data )
	{
		char * name = NULL;
		AMX * entity = (AMX*)entity_data;

		amx_SetDebugHook(entity, NULL);
		amx_GetUserData( (AMX*)entity_data, 1162690890, (void**)&name);


		free(name);

		if( entity->base != NULL )
		{
			amx_Cleanup(entity);
			memset(entity,0,sizeof(AMX));
		}
	}
}

/** Lux_PawnEntity_SetParent
*
* \param entity_data -
* \param entity -
*/
void Lux_PawnEntity_SetParent(mem_pointer entity_data, Entity * entity)
{

}

/** Lux_PawnEntity_GetParent
*
* \param entity_data -
* \return AMX's Parent Entity
*/
Entity * Lux_PawnEntity_GetParent(mem_pointer entity_data)
{
	if ( !entity_data )
	{
		return NULL;
	}
	Entity * entity;
	amx_GetUserData((AMX*)entity_data, 1162690895, (void**)&entity);
	return entity;
}

/** Lux_PawnEntity_Load
*
* \param entity_data -
*/
void Lux_PawnEntity_Restore(elix::File * current_save_file, mem_pointer entity_data)
{
	AMX * amx = (AMX*)entity_data;
	uint32_t saved_data_size = 0;

	if ( pawn_save_state_mode == LUX_SAVE_HIBERNATE_TYPE )
	{
		uint32_t unsigned_header_area_size = 0;
		long int header_area_size = 1;

		saved_data_size = current_save_file->Read_uint32WithLabel( "Entity Header Size", true );

		amx_MemInfo(amx, NULL, &header_area_size, NULL);
		unsigned_header_area_size = (uint32_t)header_area_size;

		if ( unsigned_header_area_size == 0 )
		{

		}
		else if ( saved_data_size == unsigned_header_area_size )
		{
			current_save_file->ReadWithLabel( "Entity Header Data", (data_pointer)amx->data, sizeof(uint8_t), saved_data_size);
		}
		else
		{
			std::cerr << "Lux_PawnEntity_Restore: Error restoring data. Skipping" << std::endl;
			current_save_file->Seek( current_save_file->Tell() + (int32_t)saved_data_size );
		}
	}
	else if ( pawn_save_state_mode == LUX_SAVE_DATA_TYPE )
	{
		uint32_t count = 0;

		saved_data_size = current_save_file->Read_uint32WithLabel("Entity Data Size",  true );

		if ( saved_data_size )
		{
			for ( ; count < saved_data_size; count++ )
			{
				std::string variable_name = current_save_file->ReadStringWithLabel("Public Variable Name" );
				int32_t variable_value = (int32_t)current_save_file->Read_uint32WithLabel("Public variable", true );
				Lux_PawnEntity_PublicVariable(amx, variable_name, &variable_value );

			}
		}
	}
}

/** Lux_PawnEntity_Save
*
* \param entity_data - Pointer to AMX
*/
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

#if PAWN_VERSION == 4
				amx_GetPubVar( amx, count, variable_name, &variable_value_address);
#else
				cell * paddr;
				amx_GetPubVar( amx, count, variable_name, &paddr);
				variable_value_address = amx_Address(amx, paddr);
#endif
				variable_value = *variable_value_address;

				current_save_file->WriteWithLabel("Public Variable Name", variable_name, sizeof(uint8_t), strlen(variable_name) + 1 );
				current_save_file->WriteWithLabel("Public variable", variable_value );
			}
		}

	}
}

inline bool magiccheck(AMX_HEADER * hdr)
{
	return ( hdr->magic == AMX_MAGIC );
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
	char * name = NULL;
	Entity * entity = NULL;
	AMX_HEADER * hdr = (AMX_HEADER *)((AMX*)entity_data)->base;

	amx_GetUserData( (AMX*)entity_data, 1162690890, (void**)&name);
	amx_GetUserData((AMX*)entity_data, 1162690895, (void**)&entity);

	if ( magiccheck(hdr) )
	{
		error = amx_Exec((AMX*)entity_data, NULL, (scriptcontinue ? AMX_EXEC_CONT : AMX_EXEC_MAIN));
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
			if ( entity )
			{
				MessagePush( (char*)"%s(%s) [Main] %s", entity->id.c_str(), entity->_base.c_str(), Lux_PawnEntity_StrError(error) );
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, "Entity:" + entity->id + "(" + entity->_base + ") Main Runtime Error " + Lux_PawnEntity_StrError(error) );
			}
			return false;
		}
	}
	scriptcontinue = false;
	return true;
}
#ifdef __x86_64__
/** Lux_PawnEntity_PushArrayNative
*
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
		std::cerr << __FUNCTION__ << ":" << __LINE__ << " | " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	return true;
}
#endif

/** Lux_PawnEntity_PushArray
*
* Note: 64bit pawn, expects int64_t instead of int32_t that engine offers so we need to convert them.
* It also means that you can't get a array back with this method on 64 bit systems
*/
bool Lux_PawnEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, native_pointer stack_mem)
{
	if ( !entity_data && size == 0 )
		return false;
	native_type * native_array = NULL;

	#ifdef __x86_64__
	native_array = new native_type[size];
	uint32_t count = 0;
	while (count < size)
	{
		native_array[count] = array[count];
	}
	#else
	native_array = (native_type *)array;
	#endif

	#if PAWN_VERSION == 4
	int32_t err = amx_PushArray((AMX*)entity_data, (cell**)&stack_mem, (cell*)native_array, size);
	#else
	int32_t err = amx_PushArray((AMX*)entity_data, (cell*)stack_mem, NULL, (cell*)native_array, size);
	#endif
	if ( err )
	{
		std::cerr << __FUNCTION__ << ":" << __LINE__ << " | " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	else
	{
#ifdef __x86_64__
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

	#if PAWN_VERSION == 4
	int32_t err = amx_PushString((AMX*)entity_data, (cell**)&stack_mem, str.c_str(), 0, 0);
	#else
	int32_t err = amx_PushString((AMX*)entity_data, (cell*)stack_mem, NULL, str.c_str(), 0, 0);
	#endif
	if ( err )
	{
		std::cerr << __FUNCTION__ << ":" << __LINE__ << " | " << Lux_PawnEntity_StrError(err) << std::endl;
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
		std::cerr << __FUNCTION__ << ":" << __LINE__ << " | " << Lux_PawnEntity_StrError(err) << std::endl;
		return false;
	}
	return true;
}


/** Lux_PawnEntity_Call
* Calls a public function from the entity
* \param entity_data -
* \param function -
* \param arg_count -
*/
int32_t Lux_PawnEntity_Call( mem_pointer entity_data, char * function, native_pointer memstack )
{
	if ( !entity_data )
		return false;
	int32_t index = -1;
	int32_t error;
	cell return_type = -1;
	Entity * entity;

	AMX_HEADER *hdr = (AMX_HEADER *)((AMX*)entity_data)->base;
	if ( hdr->magic == AMX_MAGIC )
	{
		error = amx_FindPublic((AMX*)entity_data, function, (int*)&index);
	}
	else
	{
		error = 13;
	}
	amx_GetUserData((AMX*)entity_data, 1162690895, (void**)&entity);

	if ( error )
	{
		std::cerr << "Entity:" << entity->_base << " - '" << function << "' not found." << std::endl;
		return_type = -2;
	}
	else
	{
		error = amx_Exec((AMX*)entity_data, &return_type, index);
		if ( error )
		{
			MessagePush( (char*)"%s(%s): [Public] %s", entity->_base.c_str(),function,Lux_PawnEntity_StrError(error) );
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Entity:" << entity->id.c_str() << "(" << entity->_base.c_str() << ") - Public Runtime Error " << Lux_PawnEntity_StrError(error) << std::endl;
		}
	}

	if ( memstack )
	{
		#ifdef HAVE_X86_64
		uintptr_t q = (uintptr_t)memstack;
		if ( ((AMX*)entity_data)->hea > q)
			((AMX*)entity_data)->hea = q;
		#else
			#if PAWN_VERSION == 4
			amx_Release( (AMX*)entity_data, (cell*)memstack );
			#else
			amx_Release( (AMX*)entity_data, (cell)memstack );
			#endif


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
Entity * Lux_PawnEntity_GetEntity(AMX *amx, cell text_param)
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


/** Lux_PawnEntity_SetString
* Checks to see if string contains something.
* \param dest - Pointer to current pawn script
* \param source - parameter to Text
* \param size - length of
* \return error code
*/
uint32_t Lux_PawnEntity_SetString(cell * dest, const char * source, size_t size )
{
	uint32_t err = AMX_ERR_NONE;

	err = amx_SetString(dest, source, 1, 0, size);

	return err;
}

/** Lux_PawnEntity_GetString
* Checks to see if string contains something.
* \param amx - Pointer to current pawn script
* \param param - parameter to Text
* \return string
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
			cell c = 0;
			uint32_t i = sizeof(cell)-1;
			char ch = 1;

			while ( ch != '\0')
			{
				if ( i == sizeof(cell)-1)
					c=*cstr++;
				ch = (char)(c >> i*CHARBITS);
				if ( ch != '\0')
				{
					str.append( 1, ch );
					i = (i+sizeof(cell)-1) % sizeof(cell);
				}
			}
		}
		else
		{
			while ( *cstr != 0 )
				str.append( 1, (char)*cstr++ );
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

