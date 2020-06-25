#include "TextBox.h"



void TextBox::setTextPosition()
{
	Vector2f rec = rectang.getPosition();
	Vector2f recs = rectang.getSize();
	Vector2f position;
	position.x = rec.x + rec.x*0.01;
	position.y = rec.y + (recs.y - text.getCharacterSize()) / 2 - recs.y*0.1;
	this->text.setPosition(position);
}

TextBox::TextBox(Vector2f position, Vector2f size,
	Color ide, Color hover, Color active)
	:Box(position, size, ide, hover, active)
{
	this->setString("");
	this->startKey = 32;
	this->endKey = 120;
}

void TextBox::draw(RenderWindow & window)
{
	if (this->len == 0) {
		text.setString(noneS);
	}
	if (this->state != IDE) {
		this->sDiplay[len] = '_';
		this->sDiplay[len + 1] = 0;
		this->text.setString(sDiplay);
		this->sDiplay[len] = 0;
	}
	window.draw(rectang);
	window.draw(text);
	this->text.setString(s);
}

void TextBox::handleKey(Uint32 key)
{
	if (this->state == IDE) {
		return;
	}
	if (key >= startKey && key <= endKey) {
		int l = strlen(s);
		sDiplay[l] = (char)key;
		sDiplay[l + 1] = '_';
		sDiplay[l + 2] = 0;
		text.setString(sDiplay);
		int y = rectang.getSize().x*0.1 + text.getGlobalBounds().width;
		if (y > rectang.getSize().x) {
			text.setString(s);
			sDiplay[l] = 0;
		}
		else {
			s[l] = (char)key;
			sDiplay[l] = (char)key;
			this->len++;
			s[l + 1] = 0;
			sDiplay[l + 1] = 0;
		}
	}
	else if (key == 8) {
		int vtc = strlen(s);
		if (vtc > 0) {
			vtc--;
			s[vtc] = 0;
			sDiplay[vtc] = 0;
			len--;
		}
	}
	else if (key == 13) {
		this->isOkEnter = 1;
		this->state = IDE;
	}
}

void TextBox::setTypeInput(TypeInput t)
{
	if (t == NUMBER) {
		this->startKey = 48;
		this->endKey = 57;
	}
	else if (t == USERNAME) {
		this->startKey = 48;
		this->endKey = 122;
	}
	else {
		this->startKey = 32;
		this->endKey = 126;
	}
}

void TextBox::setNoneS(wchar_t *ws)
{
	wcscpy(this->noneS, ws);
}




TextBox::~TextBox()
{
}
