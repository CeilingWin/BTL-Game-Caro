#pragma once
#include "Player.h"
class PlayerOnline :
	public Player
{
public:
	PlayerOnline(char *name, SOCKET server);
	Vector2i move();
	~PlayerOnline();
};

