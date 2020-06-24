#include "Caro.h"



void Caro::colorWin(Vector2i pos, Vector2i dir)
{
	int color;
	if (board[pos.x][pos.y] == 1) color = 6;
	else color = 7;
	for (int i = 0; i < 5; i++) {
		boardDisplay[pos.x][pos.y] = color;
		pos += dir;
	}
}

inline Vector2i Caro::getIndexCheck(Vector2i pos, Vector2i dir)
{
	for (int i = 0; i < 4; i++) {
		pos += dir;
		if (pos.x < 0 || pos.x>19 || pos.y < 0 || pos.y>25) {
			pos -= dir;
			break;
		}
	}
	return pos;
}

Caro::Caro(RenderWindow *window, Vector2f position)
{
	this->window = window;
	this->position = position;
	texture.loadFromFile("ceiwin/caro.png");
	this->renderStates.texture = &texture;
	//init board
	board = new short*[20];
	for (int i = 0; i < 20; i++)
		board[i] = new short[26];
	for (int y = 0; y < 20; y++)
		for (int x = 0; x < 26; x++) {
			board[y][x] = 0;
			boardDisplay[y][x] = 0;
		}
	vertexs.resize(4 * 26 * 20);
	vertexs.setPrimitiveType(Quads);
	lastChoose = { 10,13 };
	boardDisplay[10][13] = 3;
	lastMove.x = -1;
}

void Caro::draw()
{
	int index, state;
	int x, y;
	x = position.x;
	y = position.y;
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 26; i++) {
			index = (j * 26 + i) * 4;
			state = boardDisplay[j][i];
			vertexs[index].position = { i*cellSize + x,j*cellSize + y };
			vertexs[index++].texCoords = { state*cellSize,0 };
			vertexs[index].position = { (i + 1)*cellSize + x,j*cellSize + y };
			vertexs[index++].texCoords = { (state + 1)*cellSize,0 };
			vertexs[index].position = { (i + 1)*cellSize + x,(j + 1)*cellSize + y };
			vertexs[index++].texCoords = { (state + 1)*cellSize,cellSize };
			vertexs[index].position = { i*cellSize + x,(j + 1)*cellSize + y };
			vertexs[index++].texCoords = { state*cellSize,cellSize };
		}
	}
	window->draw(vertexs, renderStates);
}

void Caro::changeTurn()
{
	if (xo == O) xo = X;
	else xo = O;
}

void Caro::update(Vector2i move)
{
	if (xo == O)
		xo = X;
	else
		xo = O;
	board[move.x][move.y] = xo;
	boardDisplay[move.x][move.y] = xo;
	lastMove = move;
}

void Caro::mark(Vector2f mousePosition)
{
	mousePosition -= position;
	int i, j;
	i = (int)mousePosition.y / 30;
	j = (int)mousePosition.x / 30;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (j > 25) i = 25;
	if (i > 19) j = 19;
	int h, k;
	h = lastChoose.x;
	k = lastChoose.y;
	if (h != i || k != j) {
		if (boardDisplay[h][k] < 6 && boardDisplay[h][k]>=3)
			boardDisplay[h][k] -= 3;
		if (boardDisplay[i][j] < 3)
			boardDisplay[i][j] += 3;
		lastChoose.x = i;
		lastChoose.y = j;
	}

}
bool Caro::checkEndGame()
{
	int row, col, count;
	Vector2i startxy, endxy;
	row = lastMove.x;
	col = lastMove.y;
	if (row != -1) {
		count = 0;
		startxy = getIndexCheck(lastMove, { 0,-1 });
		endxy = getIndexCheck(lastMove, { 0,1 });
		for (int k = startxy.y; k <= endxy.y; k++) {
			if (board[row][k] == xo) {
				count++;
				if (count == 5) {
					colorWin({ row,k }, { 0,-1 });
					return true;
				}
			}
			else
				count = 0;
		}
		//check doc
		startxy = getIndexCheck(lastMove, { -1,0 });
		endxy = getIndexCheck(lastMove, { 1,0 });
		count = 0; 
		for (int k = startxy.x; k <= endxy.x; k++) {
			if (board[k][col] == xo) {
				count++;
				if (count == 5) {
					colorWin({ k,col }, { -1,0 });
					return true;
				}
			}
			else
				count = 0;
		}
		//check cheo trai
		count = 0;
		startxy = getIndexCheck(lastMove, { -1,-1 });
		endxy = getIndexCheck(lastMove, { 1,1 });
		int t = startxy.x;
		for (int k = startxy.y; k <= endxy.y; k++) {
			if (board[t][k] == xo) {
				count++;
				if (count == 5) {
					colorWin({ t,k }, { -1,-1 });
					return true;
				}
			}
			else
				count = 0;
			t++;
		}
		//check cheo phai
		startxy = getIndexCheck(lastMove, { 1,-1 });
		endxy = getIndexCheck(lastMove, { -1,1 });
		t = startxy.x;
		for (int k = startxy.y; k <= endxy.y; k++) {
			if (board[t][k] == xo) {
				count++;
				if (count == 5) {
					colorWin({ t,k }, { 1,-1 });
					return true;
				}
			}
			else
				count = 0;
			t--;
		}
		lastMove.x = -1;
	}
	return false;
}

short ** Caro::getBoard()
{
	return board;
}

Vector2i Caro::getIndex(Vector2f mousePosition, Vector2f position)
{
	mousePosition -= position;
	int i, j;
	i = (int)mousePosition.y / 30;
	j = (int)mousePosition.x / 30;
	if (i < 0) i = -1;
	if (j < 0) i = -1;
	if (j > 25) i = -1;
	if (i > 19) i = -1;
	return Vector2i(i, j);
}


Caro::~Caro()
{
	for (int i = 0; i < 20; i++)
		delete[] board[i];
}
