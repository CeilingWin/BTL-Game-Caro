#include "Player.h"



char * Player::getName()
{
	return this->name;
}
void Player::lostTurn()
{
	this->state = 0;
	this->lastMove.x = -1;
	send(server, "GAME GOTO 100 100 .", 20, 0);
}
Player::Player(char *name, SOCKET server)
{
	strcpy(this->name, name);
	this->server = server;
}
void Player::setBoard(short ** board)
{
	this->board = board;
}

Player::~Player()
{
}
