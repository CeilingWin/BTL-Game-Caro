#pragma once
#include "Player.h"
class PlayerOffline :
	public Player
{
private:
	RenderWindow *window;
public:
	Vector2i move();
	void setWindow(RenderWindow *window);
	PlayerOffline(char *name,SOCKET server);
	~PlayerOffline();
};

