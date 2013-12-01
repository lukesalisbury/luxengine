
	void Lux_TinyJSEntity_Init();
	void Lux_TinyJSEntity_Cleanup();
	bool Lux_TinyJSEntity_Init(std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity);
	void Lux_TinyJSEntity_Destroy(void * entity_data);

	void Lux_TinyJSEntity_Restore(void * entity_data);
	void Lux_TinyJSEntity_Save(void * entity_data);

	bool Lux_TinyJSEntity_Run(void * entity_data, bool & scriptcontinue);
	bool Lux_TinyJSEntity_PushArray( mem_pointer entity_data, int32_t array[], uint32_t size, mem_pointer memstack);
	bool Lux_TinyJSEntity_PushString( mem_pointer entity_data, std::string str, mem_pointer memstack);
	bool Lux_TinyJSEntity_Push( mem_pointer entity_data, int32_t size);
	int32_t Lux_TinyJSEntity_Call(mem_pointer entity_data, char * function, mem_pointer memstack);

	LuxEntityCallback EntitySystemTinyJS = {
		&Lux_TinyJSEntity_Init,
		&Lux_TinyJSEntity_Destroy,
		&Lux_TinyJSEntity_Restore,
		&Lux_TinyJSEntity_Save,
		&Lux_TinyJSEntity_Run,
		&Lux_TinyJSEntity_Call,
		&Lux_TinyJSEntity_PushArray,
		&Lux_TinyJSEntity_PushString,
		&Lux_TinyJSEntity_Push
	};



