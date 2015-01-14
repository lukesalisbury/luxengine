
mem_pointer Lux_PawnEntity_Init( const char * entity_id, const char * entity_base, Entity * entity );
void Lux_PawnEntity_Destroy(void * entity_data);

void Lux_PawnEntity_Restore(elix::File * current_save_file, void * entity_data);
void Lux_PawnEntity_Save(elix::File * current_save_file, void * entity_data);

bool Lux_PawnEntity_Run( void * entity_data, bool & scriptcontinue);
bool Lux_PawnEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, mem_pointer memstack);
bool Lux_PawnEntity_PushString( mem_pointer entity_data, std::string str, mem_pointer memstack);
bool Lux_PawnEntity_Push( mem_pointer entity_data, int32_t value);
int32_t Lux_PawnEntity_Call( mem_pointer entity_data, char * function, mem_pointer memstack);

#if PLATFORMBITS == 64
bool Lux_PawnEntity_PushArrayNative( mem_pointer entity_data, native_type array[], uint32_t size, mem_pointer memstack);
#endif

LuxEntityCallback EntitySystemPawn = {
'p',
&Lux_PawnEntity_Init,
&Lux_PawnEntity_Destroy,
&Lux_PawnEntity_Restore,
&Lux_PawnEntity_Save,
&Lux_PawnEntity_Run,
&Lux_PawnEntity_Call,
&Lux_PawnEntity_PushArray,
&Lux_PawnEntity_PushString,
&Lux_PawnEntity_Push,
#if PLATFORMBITS == 64
&Lux_PawnEntity_PushArrayNative
#else
&Lux_PawnEntity_PushArray
#endif
};




