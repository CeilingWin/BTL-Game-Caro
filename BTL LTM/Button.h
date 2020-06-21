#pragma once
#include "Box.h"
class Button :
	public Box
{
private:
	void setTextPosition();
public:
	Button(Vector2f position, Vector2f size, Color ide, Color hover, Color active);
	void draw(RenderWindow &window);
	void setStringUnicode(wchar_t *s);
	bool isPress();
	void setPress(Vector2f m);
	void setNonePress();
	~Button();
};

