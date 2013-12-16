new stringa[128];

main() 
{
	new section_name[64] = "";
	new section_grid_x = 0;
	new section_grid_y = 0;
	new map_id = 0;

	map_id = MapCurrent();
	SectionGet( section_name, section_grid_x , section_grid_y );

	strformat(stringa, _,_, "Section '%s' Grid %dx%d - ID:0x%04x", section_name, section_grid_x , section_grid_y, map_id ); /* _ means past default argument */
	GraphicsDraw(stringa, TEXT,16,48,1,0,0 ); 

}
