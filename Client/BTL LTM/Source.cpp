#include"Game.h"
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
int main() {
	Game game;
	game.gameInit();
	return 0;
}