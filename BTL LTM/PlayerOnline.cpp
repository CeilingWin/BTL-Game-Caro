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
		cout << "nguoi choi phan hoi: " << mess << endl;
		sscanf(mess, "%s %s %d %d", protocol, values, &i, &j);
		cout << "doc dk vi tri: " << i << "," << j << endl;
		if (strcmp(protocol, "GAME") == 0 && strcmp(values, "GOTO") == 0) {
			cout << "phan hoi hop le" << endl;
			if (i < 20 && j < 26) {
				cout << "gui phan hoi GAME OK . cho doi thu" << endl;
				//for (int t=0;t<10;t++)
				send(server, "GAME OK .", 10, 0);
				return Vector2i(i, j);
			}
			else if (i == 100) {
				//mat luot
				cout << "mat luot" << endl;
				return Vector2i(-2, 0);
			}
			else if (i == 404) {
				cout << "roi tran" << endl;
				return Vector2i(-3, 0);
			}
		}
	}
	return Vector2i(-1,0);
}


PlayerOnline::~PlayerOnline()
{
}
