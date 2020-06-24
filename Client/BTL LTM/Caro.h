#pragma once
#include"Player.h"
enum TYPE {
	X=1,O=2
};
class Caro
{
private:
	short **board;
	short boardDisplay[20][26];
	float cellSize = 30;
	Vector2f lastChoose;
	Vector2i lastMove;
	TYPE xo = O;
	Vector2f position;
	Texture texture;
	RenderStates renderStates;
	VertexArray vertexs;
	RenderWindow *window;
	void colorWin(Vector2i pos, Vector2i dir);
	inline Vector2i getIndexCheck(Vector2i pos, Vector2i dir);
public:
	Caro(RenderWindow *window,Vector2f position);
	void draw();
	void changeTurn();
	void update(Vector2i move);
	void mark(Vector2f mousePosition);
	bool checkEndGame();
	short ** getBoard();
	static Vector2i getIndex(Vector2f mousePosition,Vector2f position);
	~Caro();
};

