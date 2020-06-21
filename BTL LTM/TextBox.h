#pragma once
#include "Box.h"
class TextBox :
	public Box
{
private:
	void setTextPosition();
	int startKey, endKey;
	char sDiplay[100];
	int len = 0;
	bool isOkEnter = 0;
	wchar_t noneS[40];
public:
	TextBox(Vector2f position, Vector2f size,
		Color ide = Color::White, Color hover = Color::White, Color active = Color::White);
	void draw(RenderWindow &window);
	void handleKey(Uint32 key);
	void setTypeInput(TypeInput t);
	void setNoneS(wchar_t *);
	~TextBox();
};

