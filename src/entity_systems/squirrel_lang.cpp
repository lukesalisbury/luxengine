#include <stdarg.h> 
#include <stdio.h> 
#include "squirrel/squirrel.h"

HSQUIRRELVM squirrelVM; 

void SquirrelVM_print(HSQUIRRELVM v, const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	va_end(vl);
}

void SquirrelVM_error(HSQUIRRELVM v, const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
}



/** SquirrelVM_Init
* 
* \param 
*/
void SquirrelVM_Init()
{
	squirrelVM = sq_open(1024);
	sq_setprintfunc(squirrelVM, SquirrelVM_print, SquirrelVM_error);
}

/** SquirrelVM_Cleanup
* 
* \param 
*/
void SquirrelVM_Cleanup()
{
	sq_close(squirrelVM); 
}

	/*
	typedef struct {
		bool (*Init) ( std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity );
		void (*Destroy) ( mem_pointer entity_data );
		void (*Restore) ( mem_pointer entity_data );
		void (*Save) ( mem_pointer entity_data );
		bool (*Run) ( mem_pointer entity_data, bool & scriptcontinue );
		int32_t (*Call) ( mem_pointer entity_data, char * function, mem_pointer stack_mem  );
		bool (*PushArray) ( mem_pointer entity_data, int32_t array[], uint32_t size, mem_pointer stack_mem);
		bool (*PushString) ( mem_pointer entity_data, std::string str, mem_pointer stack_mem);
		bool (*Push) ( mem_pointer entity_data, int32_t value);
	} LuxEntityCallback;
	*/

bool SquirrelEntity_Init( std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity )
{

}
