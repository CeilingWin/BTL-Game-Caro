#include "Player.h"



char * Player::getName()
{
	return this->name;
}
void Player::lostTurn()
{
	this->state = 0;
	this->lastMove.x = -1;
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
