#include "PlayerOffline.h"
#include"Caro.h"


Vector2i PlayerOffline::move()
{
	if (state == 0) {
		Vector2i m = Mouse::getPosition(*window);
		m = Caro::getIndex({ (float)m.x,(float)m.y }, { 20,20 });
		if (m.x == -1) return Vector2i{ -1,0 };
		if (Mouse::isButtonPressed(Mouse::Left)) {
			if (board[m.x][m.y] == 0) {
				lastMove = m;
				sprintf(mess, "GAME GOTO %d %d .", m.x, m.y);
				send(server, mess, strlen(mess), 0);
				state = 1;
			}
		}
	}
	int ret = recv(server, mess, sizeof(mess), 0);
	if (ret > 0) {
		mess[ret] = 0;
		cout << "server:" << mess << endl;
		sscanf(mess, "%s %s ", protocol, values);
		if (strcmp(protocol, "GAME") == 0 && strcmp(values, "OK") == 0) {
			if (lastMove.x != -1) {
				Vector2i move = lastMove;
				lastMove.x = -1;
				state = 0;
				return move;
			}
		}
		else if (strcmp(protocol, "GAME") == 0 && strcmp(values, "GOTO") == 0) {
			int x;
			cout << "nhan duoc game goto 404 \n";
			sscanf(mess, "%s %s %d", protocol, values, &x);
			if (x == 404)
				return Vector2i(-3, 0);
		}
	}
	return Vector2i(-1, 0);
}



void PlayerOffline::setWindow(RenderWindow *window)
{
	this->window = window;
}



PlayerOffline::PlayerOffline(char * name, SOCKET server) :Player(name, server)
{
}




PlayerOffline::~PlayerOffline()
{
}
