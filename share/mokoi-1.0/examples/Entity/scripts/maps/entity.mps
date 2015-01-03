#include <graphics> 
#include <entities> 
#include <string> 
#include <core> 

forward public CreateEntity();

new entity_id = 0;
new stringa{64}; 
new last_id = 0; 
new map = 0;

public CreateEntity()
{
	
	EntityCreate("simple", "", random(512),random(384), 4, map); 
	
}

public Init( ... )
{
	map = CURRENT_MAP;
}

PointCollide( px, py, _x_, _y_ )
{
	if(px < _x_)	return 0;
	if(px > _x_ + 16)	return 0;
	if(py > _y_ + 16)	return 0;
	if(py < _y_)	return 0;

	return 1;
}

main() 
{
	new px = InputPointer(0);
	new py = InputPointer(1);
	new i = 1; 
	new c = EntitiesList(map);
	
	strformat(stringa, _,_, "Entities on map: %d", c); /* _ means past default argument */
	GraphicsDraw(stringa, TEXT,16,52,1,0,0 ); 

	if ( c ) /* Start a list */ 
	{ 
		while ( EntitiesNext(entity_id, map) ) 
		{ 
			last_id = entity_id
			if ( PointCollide(px,py, EntityPublicVariable(entity_id,"x"), EntityPublicVariable(entity_id,"y") ) && InputButton(11) == 1 )
			{
				EntityPublicFunction(entity_id, "ChangeID", "");
			}
			i++; 
		}
		
		StringFormat(stringa, _,_, "Press ❷ to Delete Entity ID '%d'", last_id); /* _ means past default argument */
		GraphicsDraw(stringa, TEXT,16,40,1,0,0 ); 

		if ( InputButton(1) == 1 && last_id ) 
		{ 
			EntityDelete(last_id);
			last_id = 0;
		} 
	} 
	if ( InputButton(0) ==1) 
	{ 
		CreateEntity();
	} 
}
