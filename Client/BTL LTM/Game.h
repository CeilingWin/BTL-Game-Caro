#pragma once
#include"PlayerOffline.h"
#include"PlayerOnline.h"
#include"Button.h"
#include"TextBox.h"
#include"Caro.h"
const char serverIp[20] = "52.147.199.60";
const short serverPort = 9000;
class Game
{
private:
	Caro *caro;
	RenderWindow *window;

	SOCKET server;
	WSADATA wsa;
	SOCKADDR_IN serverAddr;
	unsigned long ul = 1;

	bool turn; //false =oppenent turn
	PlayerOffline *you;
	PlayerOnline *oppenent;
	int yourPoint;
	int oppenentPoint;
	Font font,font2;
	float timePlay, turnTime;
	int roomId; 

	Sprite background;
	Texture bgTexture;
	Color cIde;
	Color cHover;
	Color cActive;
	int getRoomId(char *mess); 
	void GD1(); //giao diện đăng nhập 
	void GD2(); //giao diện chọn chế độ 
	bool GD3(); //giao diện chờ 
	int gameStart(); //bắt đầu game 
public:
	Game();
	void gameInit();
	~Game();
};

