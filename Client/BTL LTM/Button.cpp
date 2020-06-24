#include "Button.h"



Button::Button(Vector2f position, Vector2f size, Color ide, Color hover, Color active)
	:Box(position, size, ide, hover, active)
{
}

void Button::setTextPosition()
{
	Vector2f position, rec;
	rec = rectang.getPosition();
	position.x = rec.x + (rectang.getSize().x - this->text.getGlobalBounds().width) / 2;
	position.y = rec.y + (rectang.getSize().y - this->text.getGlobalBounds().height) / 2 - rectang.getSize().y*0.1;
	this->text.setPosition(position);
}

void Button::draw(RenderWindow & window)
{
	window.draw(rectang);
	string s = text.getString();
	window.draw(text);
	this->press = false;
}

void Button::setStringUnicode(wchar_t * s)
{
	this->text.setString(s);
	this->setTextPosition();
}



bool Button::isPress()
{
	if (this->press) return true;
	else
		return false;
}

void Button::setPress(Vector2f m)
{
	if (this->rectang.getGlobalBounds().contains(m)) {
		this->press = true;
	}
}

void Button::setNonePress()
{
	this->press = false;
}



Button::~Button()
{
}
