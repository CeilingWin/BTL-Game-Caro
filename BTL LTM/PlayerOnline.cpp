#include "PlayerOnline.h"



PlayerOnline::PlayerOnline(char *name, SOCKET server) :Player(name, server)
{
}

Vector2i PlayerOnline::move()
{

	int ret = recv(server, mess, sizeof(mess), 0);
	if (ret > 0) {
		mess[ret] = 0;
		int i, j;
		cout << "server: " << mess << endl;
		sscanf(mess, "%s %s %d %d", protocol, values, &i, &j);
		if (strcmp(protocol, "GAME") == 0 && strcmp(values, "GOTO") == 0) {
			if (i < 20 && j < 26) {
				send(server, "GAME OK .", 10, 0);
				return Vector2i(i, j);
			}
			else if (i == 100) {
				//mat luot
				return Vector2i(-2, 0);
			}
			else if (i == 404) {
				//roi tran
				return Vector2i(-3, 0);
			}
		}
	}
	return Vector2i(-1,0);
}


PlayerOnline::~PlayerOnline()
{
}
