#include "Game.h"



int Game::getRoomId(char * mess)
{
	char protocol[10], value[10];
	int roomId;
	sscanf(mess, "%s %s %d", protocol, value, &roomId);
	if (strcmp(protocol, "MAPHONG") == 0) {
		if (strcmp(value, "OK") == 0) {
			return roomId;
		}
		else if (strcmp(value, "ERROR") == 0)
			if (roomId == 1) return -1; //ko tim thay phong
			else if (roomId == 2) return -2;// phong da du 2 nguoi
	}
	return -3;
}

Game::Game()
{
	//init window 
	ContextSettings settings;
	settings.antialiasingLevel = 6;
	this->window = new RenderWindow(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "CARO", Style::Close, settings);
	this->window->setFramerateLimit(60);

	//init socket server
	WSAStartup(MAKEWORD(2, 2), &wsa);
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverAddr.sin_addr.s_addr = inet_addr(serverIp);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);

	//font and color
	this->font.loadFromFile("ceiwin/arial.ttf");
	this->font2.loadFromFile("ceiwin/font2.otf");
	cIde = Color(0, 158, 216);
	cHover = Color(0, 121, 65);
	cActive = Color::Green;

	//thời gian cho mỗi lượt đánh 
	this->turnTime = 20; 
}

void Game::GD1()
{
	//background 
	this->bgTexture.loadFromFile("ceiwin/bg13.jpg");
	this->background.setTexture(bgTexture);

	//
	int x;
	Vector2f ipnamePosition, okPosition, ipnameSize, okSize;
	ipnamePosition = { 350,350 };
	ipnameSize = { 400,50 };
	okPosition = { 450,450 };
	okSize = { 200,50 };

	//intro 
	Text intro, error;
	intro.setString(L"GAME CARO");
	intro.setCharacterSize(100);
	intro.setFont(font2);
	intro.setFillColor(Color(54, 54, 54));
	x = (1100 - intro.getGlobalBounds().width) / 2;
	intro.setPosition(x, 100);

	//error 
	error.setCharacterSize(20);
	error.setFont(font);
	error.setFillColor(Color::White);
	error.setString("");
	error.setPosition(350, 320);

	//Text box : input name
	TextBox inputName(ipnamePosition, ipnameSize);
	inputName.setTypeInput(USERNAME);
	inputName.setText(&font, Color(105, 105, 105));
	inputName.setNoneS(L"Nhập tên");

	//OK Button
	Button okButton(okPosition, okSize, Color(254, 95, 85), Color(254, 55, 55), Color(41, 204, 126));
	okButton.setStringUnicode(L"Đăng nhập");
	okButton.setText(&font, Color::White);
	okButton.reSizeText(30);

	//LOOP nhap ten
	char nameInput[30];
	bool notEnterName = true;
	bool cannotConnect = true;
	while ((notEnterName != false || cannotConnect != false) && window->isOpen()) {
		Event e;
		Vector2f m = { (float)Mouse::getPosition(*window).x, (float)Mouse::getPosition(*window).y };
		while (window->pollEvent(e)) {
			switch (e.type) {
			case Event::Closed:
				window->close();
			case Event::TextEntered:
				inputName.handleKey(e.text.unicode);
				break;
			case Event::MouseButtonPressed:
				okButton.setPress(m);
				break;
			}
		}
		okButton.update(m);
		if (notEnterName) {
			inputName.update(m);
			if (okButton.isPress()) {
				strcpy(nameInput, inputName.getString());
				if (strlen(nameInput) < 4) {
					error.setString(L"Tên phải dài hơn 4 ký tự");
				}
				else {
					notEnterName = false;
					okButton.setStringUnicode(L"Connecting....");
				}
			}
		}
		else {
			int ret = connect(server, (SOCKADDR*)&this->serverAddr, sizeof(serverAddr));
			if (ret != SOCKET_ERROR) {
				ioctlsocket(server, FIONBIO, &ul);
				char mess[50];
				sprintf(mess, "NAME %s .", nameInput);
				send(server, mess, strlen(mess), 0);
				cout << "Da ket noi ts server" << endl;
				cannotConnect = false;
			}
		}
		window->clear();
		window->draw(background);
		window->draw(intro);
		window->draw(error);
		inputName.draw(*window);
		okButton.draw(*window);
		window->display();
	}

	//sau khi nhập tên thành công -> khỏi tạo player 
	this->you = new PlayerOffline(nameInput, this->server);
	this->you->setWindow(window);
}

void Game::GD2()
{
	if (!window->isOpen()) return;

	//background
	this->bgTexture.loadFromFile("ceiwin/bg17.jpg");
	this->background.setTexture(bgTexture);

	// init
	Vector2f pPN, pCR, pFR, sPN, sCR, sFR, pIP, sIP;
	pIP = { 554,500 };
	pPN = { 347,300 };
	pCR = { 347,400 };
	pFR = { 347,500 };
	sPN = { 405,50 };
	sCR = sPN;
	sFR = { 200,50 };
	sIP = { 200,50 };
	char mess[100];

	//intro and error
	Text intro, error;
	intro.setFont(font2);
	intro.setCharacterSize(100);
	intro.setFillColor(Color::White);
	intro.setString(L"Chọn chế độ");
	intro.setLetterSpacing(2);
	intro.setPosition(350, 50);
	error.setFont(font);
	error.setCharacterSize(20);
	error.setFillColor(Color::Red);
	error.setString("");
	error.setPosition(375, 470);

	//textbox : intput room id
	TextBox inputRoomId(pIP, sIP);
	inputRoomId.setText(&font, Color(105, 105, 105));
	inputRoomId.setTypeInput(NUMBER);
	inputRoomId.setNoneS(L"Mã phòng");
	inputRoomId.reSizeText(25);

	// 3 button : tìm phòng, chơi luôn, tạo phòng 
	Button PN(pPN, sPN, Color(0, 158, 216), Color(15, 121, 160), Color(7, 55, 72));
	Button CR(pCR, sCR, Color(244, 70, 59), Color(172, 51, 43), Color(109, 27, 22));
	Button FR(pFR, sFR, Color(57, 230, 63), Color(31, 139, 34), Color(17, 76, 18));
	PN.setText(&font, Color::White);
	CR.setText(&font, Color::White);
	FR.setText(&font, Color::White);
	PN.setStringUnicode(L"Chơi luôn");
	CR.setStringUnicode(L"Tạo phòng");
	FR.setStringUnicode(L"Tìm phòng");
	PN.reSizeText(25); PN.setBold();
	CR.reSizeText(25); CR.setBold();
	FR.reSizeText(25); FR.setBold();

	//LOOP 
	bool notGetRoomId = true;
	char roomId[20];
	int room;
	while (notGetRoomId && window->isOpen()) {
		Event e;
		Vector2f m = { (float)Mouse::getPosition(*window).x, (float)Mouse::getPosition(*window).y };
		while (window->pollEvent(e)) {
			switch (e.type) {
			case Event::Closed:
				window->close();
				return;
			case Event::TextEntered:
				inputRoomId.handleKey(e.text.unicode);
				break;
			case Event::MouseButtonPressed:
				PN.setPress(m);
				FR.setPress(m);
				CR.setPress(m);
				break;
			}
		}
		PN.update(m);
		FR.update(m);
		CR.update(m);
		inputRoomId.update(m);

		//xu ly button
		if (PN.isPress()) {
			strcpy(mess, "CHOILUON .");
			send(server, mess, strlen(mess), 0);
		}
		else if (CR.isPress()) {
			strcpy(mess, "TAOPHONG .");
			send(server, mess, strlen(mess), 0);
		}
		else if (FR.isPress()) {
			strcpy(roomId, inputRoomId.getString());
			if (strlen(roomId) > 0) {
				int id = atoi(roomId);
				sprintf(mess, "TIMPHONG %d .", id);
				send(server, mess, strlen(mess), 0);
			}
			else {
				error.setString(L"Chưa nhập mã phòng");
			}
		}

		// nhan du lieu tu server
		int ret = recv(server, mess, sizeof(mess), 0);
		if (ret > 0) {
			cout << "server: " << mess << endl;
			room = getRoomId(mess);
			if (room >= 0) {
				notGetRoomId = false;
				this->roomId = room;
				cout << "vao phong :" << room << endl;
				return;
			}
			else if (room == -1)
				error.setString(L"Không tìm thấy phòng");
			else if (room == -2)
				error.setString(L"Phòng đã đủ 2 người chơi");
		}

		//display
		window->clear();
		window->draw(background);
		window->draw(intro);
		PN.draw(*window);
		FR.draw(*window);
		CR.draw(*window);
		window->draw(error);
		inputRoomId.draw(*window);
		window->display();
	}

}

bool Game::GD3()
{
	if (window->isOpen() == false) return false;

	//background
	this->bgTexture.loadFromFile("ceiwin/bg11.jpg");
	this->background.setTexture(bgTexture);

	//Waitting 
	Sprite wait;
	Texture waittexture;
	waittexture.loadFromFile("ceiwin/wait1.png");
	wait.setTexture(waittexture);
	wait.setScale(0.5, 0.5);
	wait.setOrigin(wait.getGlobalBounds().width, wait.getGlobalBounds().height);
	wait.setPosition(510, 350);

	//button exit,label ROOM
	Vector2f pExit, sExit, pR, sR;
	pExit = { 400,500 };
	sExit = { 200,50 };
	pR = { 10, 10 };
	sR = { 200, 50 };
	Button Exit(pExit, sExit, Color(244, 70, 59), Color(172, 51, 43), Color(109, 27, 22));
	Button roomLabel(pR, sR, Color(0, 0, 0, 0), cHover, cActive);
	Exit.setText(&font, Color::White);
	Exit.setStringUnicode(L"EXIT");
	roomLabel.setText(&font, Color::Black);
	char l[30];
	sprintf(l, "Room:%d", this->roomId);
	roomLabel.setString(l);

	//INTRO
	Text intro;
	intro.setCharacterSize(50);
	intro.setFont(font);
	intro.setFillColor(Color(54, 54, 54));
	intro.setString(L"Chờ người chơi khác...");
	intro.setPosition(230, 170);

	//LOOP
	char mess[100];
	char protocol[30];
	char nameOP[30];
	while (true && window->isOpen()) {
		Event e;
		Vector2f m = { (float)Mouse::getPosition(*window).x, (float)Mouse::getPosition(*window).y };
		while (window->pollEvent(e)) {
			switch (e.type) {
			case Event::Closed:
				window->close();
				return false;
				break;
			case Event::MouseButtonPressed:
				Exit.setPress(m);
				break;
			}
		}
		Exit.update(m);
		wait.rotate(0.99);
		//
		if (Exit.isPress()) {
			send(server, "ROIPHONG .", 11, 0);
			return false;
		}

		//chờ lệnh game start từ server 
		int ret = recv(server, mess, sizeof(mess), 0);
		if (ret > 0) {
			mess[ret] = 0;
			cout << "server: " << mess << endl;
			sscanf(mess, "%s", protocol);
			if (strcmp(protocol, "GAMESTART") == 0) {
				sscanf(mess + strlen(protocol), "%s %s", protocol, nameOP);
				if (strcmp(protocol, "HOST") == 0)
					turn = true;
				else if (strcmp(protocol, "CLIENT") == 0)
					turn = false;
				cout << "da ket noi voi nguoi choi:" << nameOP << endl;
				this->oppenent = new PlayerOnline(nameOP, server);
				return true;
			}
		}

		//display
		window->clear();
		window->draw(background);
		window->draw(intro);
		window->draw(wait);
		Exit.draw(*window);
		roomLabel.draw(*window);
		window->display();

	}

	return false;
}

int Game::gameStart()
{
	if (window->isOpen() == false) return -1;

	//background
	this->bgTexture.loadFromFile("ceiwin/bg.jpg");
	this->background.setTexture(bgTexture);

	//init 
	Vector2f pY, pO, pTY, pTO, pTime, pTimeOut, pOut;
	Vector2f sY, sO, sTY, sTO, sTime, sTimeOut, sOut;
	pY = { 805,40 };
	sY = { 125,60 };
	pO = { 165, 0 }; pO += pY;
	sO = sY;
	pTY = { 30,sY.y + 10 }; pTY += pY;
	pTO = { 160,0 }; pTO += pTY;
	pTimeOut = { -20,50 }; pTimeOut += pTY;
	sTimeOut = { 260,40 };
	pOut = { 820,560 };
	sOut = { 260,50 };

	//Button exit
	Button bOut(pOut, sOut, Color(244, 70, 59), Color(172, 51, 43), Color(109, 27, 22));
	bOut.setText(&font, Color::White);
	bOut.setStringUnicode(L"OUT GAME");

	//label 2 player 
	Button bP1(pY, sY, Color(30, 234, 85,200), cHover, cActive);
	Button bP2(pO, sO, Color(248, 14, 14,200), cHover, cActive);
	bP1.setText(&font, Color::Black);
	bP1.setString(you->getName());
	bP1.reSizeText(30);
	bP2.setText(&font, Color::Black);
	bP2.setString(oppenent->getName());
	bP2.reSizeText(30);

	//text
	Text vs;
	vs.setCharacterSize(25);
	vs.setFillColor(Color(0,255,255));
	vs.setFont(font);
	vs.setStyle(Text::Bold);
	vs.setPosition(931, 55);
	vs.setString("VS");

	// XO
	Sprite x, o;
	Texture tx, to;
	tx.loadFromFile("ceiwin/x.png");
	to.loadFromFile("ceiwin/o.png");
	x.setTexture(tx);
	o.setTexture(to);
	if (turn) {
		x.setPosition(pTY);
		o.setPosition(pTO);
	}
	else {
		x.setPosition(pTO);
		o.setPosition(pTY);
	}

	//điểm số 
	Vector2f dir;
	dir = { 35,0 };
	pTY += dir;
	sY = { 30,30 };
	Button yp(pTY, sY, Color::White, Color::White, Color::White);
	pTO += dir;
	Button op(pTO, sY, Color::White, Color::White, Color::White);
	char point[5];
	yp.setText(&font, Color::Blue);
	sprintf(point, "%d", yourPoint);
	yp.setString(point);
	yp.setBold();
	op.setText(&font, Color::Red);
	sprintf(point, "%d", oppenentPoint);
	op.setString(point);
	op.setBold();
	
	// time out
	RectangleShape recOut, recIn;
	recOut.setPosition(pTimeOut);
	recOut.setSize(sTimeOut);
	recOut.setFillColor(Color(0,0,0,0));
	recOut.setOutlineThickness(5);
	recOut.setOutlineColor(Color(0,191,255));

	recIn.setPosition(pTimeOut.x + 5, pTimeOut.y + 5);
	recIn.setSize({ sTimeOut.x - 10, sTimeOut.y - 10 });
	if (turn) recIn.setFillColor(Color::Green);
	else recIn.setFillColor(Color::Red);

	//GAME LOOP
	float time = this->turnTime;
	float timeElapse;
	float sizeRecInx = recIn.getSize().x;
	float sizeRecIny = recIn.getSize().y;
	bool sended = false;
	char buff[100],protocol[10],values[10];
	Clock clock;
	bool playing = true;
	while (playing && window->isOpen()) {
		Event e;
		timeElapse = clock.restart().asSeconds();
		Vector2f m = { (float)Mouse::getPosition(*window).x, (float)Mouse::getPosition(*window).y };
		while (window->pollEvent(e)) {
			switch (e.type) {
			case Event::Closed:
				window->close();
				return -1;
				break;
			case Event::MouseMoved:
				caro->mark(m);
				break;
			case Event::MouseButtonPressed:
				bOut.setPress(m);
				break;
			}
		}
		bOut.update(m);
		if (bOut.isPress()) {
			send(server, "GAME GOTO 404 0 .", 18, 0);
			return -1;
		}
		//xu ly thanh thời gian 
		time -= timeElapse;
		if (time <= 0)    time = 0;
		recIn.setSize({ sizeRecInx*(time / turnTime), sizeRecIny });
		if (turn) recIn.setFillColor(Color::Green);
		else recIn.setFillColor(Color::Red);

		//xu ly nuoc di cua 2 nguoi choi
		Vector2i move;
		if (turn) {
			if (time == 0) {
				if (!sended) {
					send(server, "GAME GOTO 100 0 .", 18, 0);
					sended = true;
				}
				else {
					int ret = recv(server, buff, sizeof(buff), 0);
					if (ret > 0) {
						buff[ret] = 0;
						if (strstr(buff, "GAME OK") != NULL) {
							cout << "da bi mat luot \n";
							sended = false;
							time = this->turnTime;
							you->lostTurn();
							caro->changeTurn();
							turn = !turn;
						}
						else if (strstr(buff, "GAME GOTO 404") != NULL)
							return 0;
					}
				}
			}
			else {
				move = you->move();
				if (move.x >= 0) {
					cout << "ban da di nuoc : " << move.x << " " << move.y << endl;
					caro->update(move);
					if (caro->checkEndGame())
						break;
					turn = !turn;
					time = this->turnTime;
				}
				else if (move.x == -3)
					return 0;
			}
		}
		else {
			move = oppenent->move();
			if (move.x == -2) {
				cout << "doi thu mat luot\n";
				turn = !turn;
				caro->changeTurn();
				time = this->turnTime;
				send(server, "GAME OK .", 10, 0);
			}
			else if (move.x == -3) {
				cout << "doi thu da thoat\n";
				return 0;
			}
			else if (move.x >= 0) {
				cout << "doi thu da di nuoc : " << move.x << " " << move.y << endl;
				caro->update(move);
				if (caro->checkEndGame())
					break;
				turn = !turn;
				time = this->turnTime;
			}
		}
		//display
		window->clear();
		window->draw(background);
		bOut.update(m);

		bOut.draw(*window);
		bP1.draw(*window);
		bP2.draw(*window);
		bOut.draw(*window);
		yp.draw(*window);
		op.draw(*window);
		caro->draw();
		window->draw(x);
		window->draw(o);
		window->draw(recOut);
		window->draw(recIn);
		window->draw(vs);
		window->display();
	}


	//xu ly end game 
	Text result;
	result.setCharacterSize(100);
	result.setFont(font2);
	result.setStyle(Text::Bold);
	if (turn) {
		yourPoint++;
		result.setString("YOU WIN");
		send(server, "GAME WIN .", 11, 0);
		result.setFillColor(Color::Green);
	}
	else {
		oppenentPoint++;
		result.setString("YOU LOSE");
		result.setFillColor(Color::Red);
	}

	result.setPosition((1100 - result.getGlobalBounds().width) / 2, 250);
	pO = { 350,450 };
	sO = { 400,50 };
	pY = { 350,510 };
	sY = sO;
	Button replay(pO, sO, Color(57, 230, 63), Color(31, 139, 34), Color(17, 76, 18));
	Button exit(pY, sY, Color(244, 70, 59), Color(172, 51, 43), Color(109, 27, 22));
	replay.setText(&font, Color::White);
	replay.setStringUnicode(L"Chơi lại");
	replay.reSizeText(25);
	replay.setBold();
	exit.setText(&font, Color::White);
	exit.setStringUnicode(L"Chạy");
	exit.reSizeText(25);
	exit.setBold();

	//LOOP chờ replay 
	while (window->isOpen()) {
		Event e;
		Vector2f m = { (float)Mouse::getPosition(*window).x, (float)Mouse::getPosition(*window).y };
		while (window->pollEvent(e)) {
			switch (e.type) {
			case Event::Closed:
				window->close();
				return -1;
				break;
			case Event::MouseMoved:
				break;
			case Event::MouseButtonPressed:
				replay.setPress(m);
				exit.setPress(m);
				break;
			}
		}
		replay.update(m);
		exit.update(m);
		//xy ly
		if (replay.isPress()) { 
			send(server, "REPLAY OK .", 12, 0);
			return 1;
		}
		if (exit.isPress()) { 
			send(server, "REPLAY NO .", 12, 0);
			return -1;
		}
		int ret = recv(server, buff, sizeof(buff), 0);
		if (ret > 0) {
			buff[ret] = 0;
			if (strstr(buff, "REPLAY") != NULL) {
				if (strstr(buff, "REPLAY OK") != NULL)
					cout << "doi thu cung muon choi lai\n";
				if (strstr(buff, "REPLAY NO") != NULL)
					return 0;
			}
		}

		//display
		window->clear();
		window->draw(background);
		window->draw(result);
		replay.draw(*window);
		exit.draw(*window);
		window->display();
	}
	return 1;
}

void Game::gameInit()
{
	this->GD1();
	while (window->isOpen()) {
		this->GD2();
		int status;
		yourPoint = 0;
		oppenentPoint = 0;
		bool inRoom = true;
		while (inRoom && window->isOpen()) {
			bool canStartGame = this->GD3();
			if (canStartGame) {
				this->caro = new Caro(window, { 20,20 });
				this->you->setBoard(caro->getBoard());
				this->you->setWindow(window);

				status = this->gameStart();
				if (status == -1) {
					inRoom = false;
				}
				else {
					inRoom = true;
				}
				delete this->oppenent;
				delete this->caro;
			}
			else
				inRoom = false;
		}
	}
	delete this->you;
}



Game::~Game()
{

}
