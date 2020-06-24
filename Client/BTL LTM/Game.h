#pragma once
#include"PlayerOffline.h"
#include"PlayerOnline.h"
#include"Button.h"
#include"TextBox.h"
#include"Caro.h"
const char serverIp[20] = "127.0.0.1";
const short serverPort = 9000;
class Game
{
private:
	Caro *caro;
	RenderWindow *window;
	bool turn; //false =oppenent turn 
	SOCKET server;
	WSADATA wsa;
	SOCKADDR_IN serverAddr;
	unsigned long ul = 1;
	PlayerOffline *you;
	PlayerOnline *oppenent;
	Font font,font2;
	float timePlay, turnTime;
	int roomId; 
	Sprite background;
	Texture bgTexture;
	Color cIde;
	Color cHover;
	Color cActive;
	int getRoomId(char *mess);
public:
	Game();
	void GD1();
	void GD2();
	bool GD3();
	int gameStart();
	void gameInit();
	void test();
	~Game();
};

