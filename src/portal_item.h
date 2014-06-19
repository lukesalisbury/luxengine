#ifndef PORTAL_ITEM_H
#define PORTAL_ITEM_H

class portal_item
{
public:
	portal_item();


public:
	game( std::string gamepack, UserInterface * gui = NULL);
	game(UserInterface * gui= NULL);
	~game();

	bool SetIcon( DisplaySystem* display );
	bool ClearIcon( DisplaySystem* display );
	std::string url;
	Widget * button;
	UserInterface * _gui;
	LuxSprite * icon;
	MokoiGame * file;
	bool valid;
	bool isDir;
	DisplaySystem * display;

};

#endif // PORTAL_ITEM_H
