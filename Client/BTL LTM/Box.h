#pragma once
#include"Player.h"
enum State {
	IDE, HOVER, ACTIVE
};
enum TypeInput {
	USERNAME, NUMBER, ANY
};
class Box
{
protected:
	RectangleShape rectang;
	State state;
	Color ideColor;
	Color hoverColor;
	Color activeColor;
	char s[100];
	Text text;
	Font *font;
	bool press = false;
	virtual void setTextPosition() = 0;
public:
	Box(Vector2f position, Vector2f size, Color ide, Color hover, Color active);
	Vector2f getSize();
	Vector2f getPosition();
	void setString(char *k);
	void setText(Font *font, Color color);
	void setTexture(Texture &texture);
	void reSizeText(float size);
	void setPress(Vector2f posMouse);
	char * getString();
	void update(Vector2f posMouse);
	void setBold();
	virtual void draw(RenderWindow & window) = 0;
	~Box();
};

