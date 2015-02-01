#include <helper>

new entity:player_entity; // String to store a moving entity's id 
new timer;

/* Initialise Three Fixed variables to hold our position */
new Fixed:x;
new Fixed:y;
new Fixed:z;

/* Initialise Three Fixed variables to hold 'player' position */
new Fixed:player_x;
new Fixed:player_y;
new Fixed:player_z;

/* This Function will scan the entities on the maps. 
*  We have to do this, cause the Moving Entity has a random id.
*  If you gave the entity a name you could just use that.
*/
find_player()
{
	new entity:entity_ident; // Store a entity's id 
	new map_id = MapCurrentIdent();

	/* EntitiesList start a list of all the entities on a map. 
	* passing 0 as first argument will scan the global entities 
	*/
	new entity_count = EntitiesList( map_id );
	if ( entity_count ) /* Check if there are any entities listed */
	{	
		while ( EntitiesNext(entity_ident, map_id) ) // 0 stands for Global
		{		
			/* In the entity we are looking for we add a 'public isplayer = true;' */
			if ( EntityPublicVariable(entity_ident, "isplayer" )  )
			{
				/* This is should be a player entity id, so we should make a copy it  */
				player_entity = entity_ident;
			}
		}
	}
}

public Init( ... )
{
	EntityGetPosition(x, y, z);
	find_player();
}

main()
{
	/* every 5 second we are going to do something */
	if ( TimerCountdownWithReset( timer, 5000) )
	{
		EntityGetPosition(player_x, player_y, player_z, player_entity ); // Get Player Position
	}
	GraphicsDraw("", LINE, fround(x)+8, fround(y)+8, z,  fround(player_x), fround(player_y), 0xFF0000FF );

}