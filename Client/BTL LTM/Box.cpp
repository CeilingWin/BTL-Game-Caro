#include "Box.h"





Box::Box(Vector2f position, Vector2f size, Color ide, Color hover, Color active)
{
	this->rectang.setPosition(position);
	this->rectang.setSize(size);
	this->ideColor = ide;
	this->hoverColor = hover;
	this->activeColor = active;
	this->text.setCharacterSize(size.y*0.7);
	this->state = IDE;
	this->rectang.setFillColor(ide);
}

Vector2f Box::getSize()
{
	return rectang.getSize();
}

Vector2f Box::getPosition()
{
	return this->rectang.getPosition();
	this->setTextPosition();
}

void Box::setString(char *k)
{
	strcpy(s, k);
	this->text.setString(s);
	this->setTextPosition();
}

void Box::setText(Font * font, Color color)
{
	this->font = font;
	this->text.setFont(*font);
	this->text.setFillColor(color);
	this->setTextPosition();
}

void Box::setTexture(Texture & texture)
{
	this->rectang.setTexture(&texture);
}

void Box::reSizeText(float size)
{
	this->text.setCharacterSize(size);
	this->setTextPosition();
}

void Box::setPress(Vector2f posMouse)
{
	if (rectang.getGlobalBounds().contains(posMouse)) {
		this->press = true;
	}
	else
		this->press = false;
}

char * Box::getString()
{
	return this->s;
}


void Box::update(Vector2f posMouse)
{
	this->state = IDE;
	if (this->rectang.getGlobalBounds().contains(posMouse)) {
		this->state = HOVER;
	}
	if (this->press) {
		this->state = ACTIVE;
	}
	switch (this->state) {
	case IDE:
		this->rectang.setFillColor(ideColor);
		break;
	case HOVER:
		this->rectang.setFillColor(hoverColor);
		break;
	case ACTIVE:
		this->rectang.setFillColor(activeColor);
		break;
	}

}

void Box::setBold()
{
	this->text.setStyle(Text::Bold);
	this->setTextPosition();
}


Box::~Box()
{
}
