#include<iostream>
#include<winsock.h>
//#include<thread>
#include"console.h"
using namespace std;
#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)


struct Client {
	SOCKET socket;
	char name[16];
	int roomid;
	int state = 0;
};
struct Room {
	int roomid;
	int numclientofroom = 0;
	Client *player[2];
	bool p1Replay = false;
	bool p2Replay = false;
};

Client clients[64];
Room rooms[32];
char mess[100];
int numroom = 0;
int numclient = 0;
int roomIdIncree = 100;
char logg[100];
void Nocursortype()
{
	CONSOLE_CURSOR_INFO Info;
	Info.bVisible = FALSE;
	Info.dwSize = 20;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}
void handle(int clientId, char* buf);
void xoaphong(int roomId);
void xoaclient(int clientId);
void matketnoi(int clientId);
int timphong(int roomId);//if roomId=-1 la tim phong hop le, roomId>=0 la tim phong co Id=roomId
int taophong(int clientId);
void roiphong(int clientId);
void vaophong(int clinetId, int roomId);
void startgame(int roomId);
void protoTaoPhong(int clientId, char*buf);
void protoTimPhong(int clientId, char* buf);
void protoChoiLuon(int clientId, char* buf);
void protoGame(int clientId, char* buf);
void protoRoiPhong(int clinetId, char* buf);
void protoReplay(int clientId, char *buf);
DWORD WINAPI display(LPVOID p);

//________________MAIN________________

int main()
{
	//thread gui(display);
	CreateThread(NULL, 0, display, NULL, 0, NULL);
	//
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (sockaddr*)&addr, sizeof(addr));
	listen(listener, 5);

	int ret;
	fd_set fdread;
	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numclient; i++)
			FD_SET(clients[i].socket, &fdread);

		ret = select(0, &fdread, 0, 0, 0);

		if (ret < 0)
			break;

		if (FD_ISSET(listener, &fdread))
		{
			SOCKET client = accept(listener, NULL, NULL);
			clients[numclient].socket = client;
			numclient++;
		}

		//nhan du lieu tu client 
		char buf[640];
		for (int i = 0; i < numclient; i++)
			if (FD_ISSET(clients[i].socket, &fdread))
			{
				ret = recv(clients[i].socket, buf, sizeof(buf), 0);
				if (ret <= 0)
				{
					// Xoa socket khoi mang clients
					matketnoi(i);
				}
				else {
					buf[ret] = 0;
					handle(i, buf);
				}
			}
	}
}



void handle(int clientId, char* buf)
{
	char protocol[10];
	sscanf(buf, "%s", protocol);
	buf += strlen(protocol);
	if (strcmp(protocol, "NAME") == 0) {
		if (clients[clientId].state == 0) {
			sscanf(buf, "%s", protocol);
			strcpy(clients[clientId].name, protocol);
			clients[clientId].state = 1;
			clients[clientId].roomid = 0;
		}
	}
	else if (strcmp(protocol, "CHOILUON") == 0) {
		protoChoiLuon(clientId, buf);
	}
	else if (strcmp(protocol, "TAOPHONG") == 0) {
		protoTaoPhong(clientId, buf);
	}
	else if (strcmp(protocol, "TIMPHONG") == 0) {
		protoTimPhong(clientId, buf);
	}
	else if (strcmp(protocol, "GAME") == 0) {
		buf -= strlen(protocol);
		protoGame(clientId, buf);
	}
	else if (strcmp(protocol, "ROIPHONG") == 0) {
		protoRoiPhong(clientId, buf);
	}
	else if (strcmp(protocol, "REPLAY") == 0) {
		protoReplay(clientId, buf);
	}
}

void xoaphong(int id)
{
	int idf = numroom - 1;
	rooms[id].roomid = rooms[idf].roomid;
	rooms[id].player[0] = rooms[idf].player[0];
	rooms[id].player[1] = rooms[idf].player[1];
	rooms[id].numclientofroom = rooms[idf].numclientofroom;
	numroom--;
}

void xoaclient(int clientId)
{
	closesocket(clients[clientId].socket);
	for (int i = 0; i < numroom; i++) {
		if (clients[clientId].roomid == rooms[i].roomid) {
			if (rooms[i].player[0]->socket == clients[clientId].socket) {
				roiphong(clientId);
			}
			break;
		}
	}
	strcpy(clients[clientId].name, clients[numclient - 1].name);
	clients[clientId].socket = clients[numclient - 1].socket;
	clients[clientId].roomid = clients[numclient - 1].roomid;
	clients[numclient - 1].state = 0;
	clients[numclient - 1].name[0] = 0;
	if (clients[clientId].roomid != 0) {
		int id;
		for (id = 0; id < numroom; id++)
			if (rooms[id].roomid == clients[clientId].roomid)
				break;
		if (rooms[id].player[0]->socket == clients[clientId].socket)
			rooms[id].player[0] = &clients[clientId];
		else
			rooms[id].player[1] = &clients[clientId];
	}
	numclient--;
}

void matketnoi(int clientId)
{
	int state = clients[clientId].state;
	switch (state) {
	case 0:
	case 1:
		break;
	case 2:
		roiphong(clientId);
		break;
	case 3:
	{
		int id;
		for (id = 0; id < numroom; id++)
			if (clients[clientId].roomid == rooms[id].roomid)
				break;
		if (rooms[id].player[0]->socket == clients[clientId].socket)
			send(rooms[id].player[1]->socket, "GAME GOTO 404 0 .", 18, 0);
		else
			send(rooms[id].player[0]->socket, "GAME GOTO 404 0 .", 18, 0);
		roiphong(clientId);
		break;
	}
	case 4:
	{
		int id;
		for (id = 0; id < numroom; id++)
			if (clients[clientId].roomid == rooms[id].roomid)
				break;
		if (rooms[id].player[0]->socket == clients[clientId].socket)
			send(rooms[id].player[1]->socket, "REPLAY NO .", 12, 0);
		else
			send(rooms[id].player[0]->socket, "REPLAY NO .", 12, 0);
		roiphong(clientId);
		break;
	}
	}
	xoaclient(clientId);
}

int timphong(int roomId)
{
	//tra ve roomId
	//tra ve -1 : ko tim thay phong
	//tra ve -2 : phong da du 2 nguoi choi
	if (roomId == -1) {
		for (int i = 0; i < numroom; i++) {
			if (rooms[i].numclientofroom == 1)
				return rooms[i].roomid;
		}
		return -1;
	}
	else {
		for (int i = 0; i < numroom; i++) {
			if (rooms[i].roomid == roomId)
				if (rooms[i].numclientofroom == 2)
					return -2;
				else
					return rooms[i].roomid;
		}
		return -1;
	}
}

int taophong(int clientId)
{
	rooms[numroom].numclientofroom = 1;
	rooms[numroom].player[0] = &clients[clientId];
	rooms[numroom].roomid = roomIdIncree;
	clients[clientId].roomid = roomIdIncree;
	roomIdIncree++;
	numroom++;
	return rooms[numroom - 1].roomid;
}

void roiphong(int clientId)
{
	//
	int roomId = clients[clientId].roomid;

	clients[clientId].state = 1;
	clients[clientId].roomid = 0;
	//
	int id;
	for (id = 0; id < numroom; id++) {
		if (rooms[id].roomid == roomId)
			break;
	}
	if (rooms[id].numclientofroom == 1) {
		xoaphong(id);
	}
	else if (rooms[id].numclientofroom == 2) {
		if (rooms[id].player[0]->socket == clients[clientId].socket) {
			rooms[id].player[0] = rooms[id].player[1];
		}
		rooms[id].player[0]->state = 2;
		rooms[id].numclientofroom = 1;
	}
}

void vaophong(int clientId, int roomId)
{
	//
	clients[clientId].roomid = roomId;
	//
	int id;
	for (id = 0; id < numroom; id++)
		if (rooms[id].roomid == roomId) break;
	rooms[id].player[1] = &clients[clientId];
	rooms[id].numclientofroom = 2;

	//test
	startgame(id);
}

void startgame(int roomId)
{
	Sleep(500);
	int id = roomId;

	sprintf(mess, "GAMESTART HOST %s .", rooms[id].player[1]->name);
	send(rooms[id].player[0]->socket, mess, strlen(mess), 0);

	sprintf(mess, "GAMESTART CLIENT %s .", rooms[id].player[0]->name);
	send(rooms[id].player[1]->socket, mess, strlen(mess), 0);


	//danh dau nguoi choi dang choi
	rooms[id].player[0]->state = 3;
	rooms[id].player[1]->state = 3;
	rooms[id].p1Replay = false;
	rooms[id].p2Replay = false;
}

void protoTaoPhong(int clientId, char* buf)
{
	if (clients[clientId].state == 1) {
		int roomId = taophong(clientId);
		sprintf(mess, "MAPHONG OK %d .", roomId);
		send(clients[clientId].socket, mess, strlen(mess), 0);
		clients[clientId].state = 2;
	}
}

void protoTimPhong(int clientId, char* buf)
{
	if (clients[clientId].state == 1) {
		int roomId;
		sscanf(buf, "%d", &roomId);
		roomId = timphong(roomId);
		strcpy(mess, "MAPHONG ");
		if (roomId == -1) {
			strcat(mess, " ERROR 1 .");
		}
		else if (roomId == -2) {
			strcat(mess, " ERROR 2 .");
		}
		else {
			sprintf(mess, "MAPHONG OK %d .", roomId);
			send(clients[clientId].socket, mess, strlen(mess), 0);
			clients[clientId].state = 2;
			vaophong(clientId, roomId);
			return;
		}
		send(clients[clientId].socket, mess, strlen(mess), 0);
	}
}

void protoChoiLuon(int clientId, char* buf)
{
	if (clients[clientId].state == 1) {
		int roomId = timphong(-1);
		if (roomId >= 0) {
			sprintf(mess, "MAPHONG OK %d .", roomId);
			send(clients[clientId].socket, mess, strlen(mess), 0);
			vaophong(clientId, roomId);
		}
		else {
			roomId = taophong(clientId);
			sprintf(mess, "MAPHONG OK %d .", roomId);
			send(clients[clientId].socket, mess, strlen(mess), 0);
			clients[clientId].state = 2;
		}
	}

}

void protoGame(int clientId, char* buf)
{
	if (clients[clientId].state == 3) {
		char value[20], protocol[20];
		int x, y, id;
		for (id = 0; id < numroom; id++)
			if (rooms[id].roomid == clients[clientId].roomid)
				break;
		sscanf(buf, "%s %s %d %d ", protocol, value, &x, &y);
		if (strcmp(protocol, "GAME") == 0) {
			if (strcmp(value, "GOTO") == 0) {
				if (x == 404) {
					roiphong(clientId);
				}
				sprintf(mess, "GAME GOTO %d %d .", x, y);
			}
			else if (strcmp(value, "OK") == 0) {
				strcpy(mess, "GAME OK .");
			}
			else if (strcmp(value, "WIN") == 0) {
				rooms[id].player[0]->state = 4;
				rooms[id].player[1]->state = 4;
			}
			else if (strcmp(value, "REPLAY") == 0) {
				strcpy(mess, "GAME REPLAY .");
				if (rooms[id].player[0]->socket == clients[clientId].socket)
					rooms[id].p1Replay = true;
				else
					rooms[id].p2Replay = true;

				if (rooms[id].p1Replay == true && rooms[id].p2Replay == true) {
					if (rooms[id].player[0]->socket == clients[clientId].socket)
						send(rooms[id].player[1]->socket, mess, strlen(mess), 0);
					else
						send(rooms[id].player[0]->socket, mess, strlen(mess), 0);
					startgame(id);
					return;
				}
			}
			else if (strcmp(value, "OUT") == 0) {
				strcpy(mess, "GAME OUT .");
				roiphong(clientId);
				send(rooms[id].player[0]->socket, mess, strlen(mess), 0);
				return;
			}
			else return;
			SOCKET recver;
			if (rooms[id].player[0]->socket == clients[clientId].socket) {
				recver = rooms[id].player[1]->socket;
			}
			else {
				recver = rooms[id].player[0]->socket;
			}

			send(recver, mess, strlen(mess), 0);
		}
	}
}

void protoRoiPhong(int clientId, char* buf)
{
	if (clients[clientId].state == 2) {
		roiphong(clientId);
	}
}

void protoReplay(int clientId, char * buf)
{
	if (clients[clientId].state == 4) {
		char value[20];
		int  id;
		for (id = 0; id < numroom; id++)
			if (rooms[id].roomid == clients[clientId].roomid)
				break;
		sscanf(buf, "%s", value);
		if (strcmp(value, "OK") == 0) {
			strcpy(mess, "REPLAY OK .");
			if (rooms[id].player[0]->socket == clients[clientId].socket)
				rooms[id].p1Replay = true;
			else
				rooms[id].p2Replay = true;

			if (rooms[id].player[0]->socket == clients[clientId].socket)
				send(rooms[id].player[1]->socket, mess, strlen(mess), 0);
			else
				send(rooms[id].player[0]->socket, mess, strlen(mess), 0);

			if (rooms[id].p1Replay == true && rooms[id].p2Replay == true) {
				startgame(id);
			}
		}
		else if (strcmp(value, "NO") == 0) {
			strcpy(mess, "REPLAY NO .");
			roiphong(clientId);
			send(rooms[id].player[0]->socket, mess, strlen(mess), 0);
		}
	}
}

DWORD WINAPI display(LPVOID p)
{
	Nocursortype();
	//clscr
	while (1) {
		Sleep(2000);
		gotoXY(0, 2);
		TextColor(ColorCode_Red);
		for (int i = 2; i < 25; i++)
			cout << "                                                  |                                                 \n";

		gotoXY(20, 0);
		TextColor(ColorCode_Blue);
		cout << "Num of Clients:" << numclient;
		gotoXY(70, 0);
		TextColor(ColorCode_Green);
		cout << "Num of Room:" << numroom;
		gotoXY(0, 1);
		TextColor(ColorCode_Blue);
		cout << "Name        RoomId      State       Socket";
		gotoXY(51, 1);
		TextColor(ColorCode_Green);
		cout << "RoomId      NumPlayer   Player1     Player2";
		TextColor(ColorCode_Yellow);
		for (int i = 0; i < numclient; i++) {
			gotoXY(0, i + 2);
			cout << clients[i].name << '\t';
			gotoXY(12, i + 2);
			cout << clients[i].roomid << '\t';
			gotoXY(24, i + 2);
			cout << clients[i].state << '\t';
			gotoXY(36, i + 2);
			cout << clients[i].socket << '\t';
		}
		for (int i = 0; i < numroom; i++) {
			gotoXY(51, i + 2);
			cout << rooms[i].roomid << '\t';
			gotoXY(63, i + 2);
			cout << rooms[i].numclientofroom << '\t';
			if (rooms[i].numclientofroom >= 1) {
				gotoXY(75, i + 2);
				cout << rooms[i].player[0]->name << '\t';
			}
			if (rooms[i].numclientofroom == 2) {
				gotoXY(87, i + 2);
				cout << rooms[i].player[1]->name << '\t';
			}
		}
	}
}
