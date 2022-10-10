#include "SnakeGame.h"
using std::thread;

int main() {
	FILE* fp = NULL;

	int mode = 0;
	srand(time(NULL)); //���� �õ� ����
	makeBuffer();

	thread t1(getDir);

	//�޴�â

	loadScore(fp);


	while (isGameOn) {
		clearScreen(BASIC_COLOR);
		
		mode = Menu();
		if (mode == 0) {
			Game(); //�ΰ���
			saveScore(fp);
			isGameOn = doAgain();
		}
		else if (mode == 1) {
			highScore();
		}
		else if (mode == 2) {
			developinform();
		}

	}
	t1.join();
	return 0;
}

void makeBuffer() {
	//�ܼ�â ����
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	coordBufSize.X = WIDTH;
	coordBufSize.Y = HEIGHT;

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	srctWriteRect.Left = srctWriteRect.Top = 0;
	srctWriteRect.Right = WIDTH - 1;
	srctWriteRect.Bottom = HEIGHT - 1;

	hNewScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL 
	hNewScreenBuffer2 = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL 

	SetConsoleTitle(L"Snake Game");
	SetConsoleScreenBufferSize(hNewScreenBuffer, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer, TRUE, &srctWriteRect);
	SetConsoleScreenBufferSize(hNewScreenBuffer2, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer2, TRUE, &srctWriteRect);

	if (hNewScreenBuffer2 == INVALID_HANDLE_VALUE ||
		hNewScreenBuffer == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}
}

void prCharLine(const char* ch, int atb, int x, int y) {
	for (int j = 0;; j++) {
		if (ch[j] == '\0') {
			break;
		}
		else
		{
			chiBuffer[y * WIDTH + x + j].Char.UnicodeChar = ch[j]; //��������
			chiBuffer[y * WIDTH + x + j].Attributes = atb; // ��������

		}
	}
}

void prCharArr(const char* ch[], int len, int atb, int x, int y) {
	for (int i = 0; i < len; i++) {
		prCharLine(ch[i], atb, x, y + i);
	}
}

void clearScreen(int color) {
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			chiBuffer[x + WIDTH * y].Char.UnicodeChar = ' ';
			chiBuffer[x + WIDTH * y].Attributes = color; //126
		}
	}
}

void printScreen() {
	static int i = 0; //���� ���� ������� ����

	if (i % 2 == 0) {
		curScreenBufferHandle = hNewScreenBuffer;
	}
	else {
		curScreenBufferHandle = hNewScreenBuffer2;
	}

	fSuccess = WriteConsoleOutput(
		curScreenBufferHandle, // screen buffer to write to 
		chiBuffer,        // buffer to copy from 
		coordBufSize,     // col-row size of chiBuffer 
		coordBufCoord,    // top left src cell in chiBuffer 
		&srctWriteRect);

	if (!SetConsoleActiveScreenBuffer(curScreenBufferHandle)) {
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}

	Sleep(1 / 60.0f * 1000.0f); //��� ����
	i++; //���� �ٲٱ�
}

int Menu() {
	dir = 0;
	//& ���ڴ� ��� ���ϰ� �����س���(������ ����)
	const char* menu[10] = { "**********************************",
							"*           Snake Game           *",
							"*   move: arrow, select: space   *",
							"*                                *",
							"*             START              *",
							"*                                *",
							"*           HIGH_SCORE           *",
							"*                                *",
							"*      DEVELOPER INFORMATION     *",
							"**********************************" };
	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	static int mode = 0;

	while (1) {
		if (dir == SPACE) {
			break;
		}
		if (dir == UP && mode != 0) {
			mode--;
			dir = 0;
		}
		else if (dir == DOWN && mode != 2) {
			mode++;
			dir = 0;
		}

		prCharArr(menu, 10, BASIC_COLOR, lx, ly);

		switch (mode)
		{
		case 0:
			prCharLine("START", SELECT_COLOR, lx + 14, ly + 4);
			break;
		case 1:
			prCharLine("HIGH SCORE", SELECT_COLOR, lx + 12, ly + 6);
			break;
		case 2:
			prCharLine("DEVELOPER INFORMATION", SELECT_COLOR, lx + 7, ly + 8);
			break;
		default:
			break;
		}

		printScreen();
	}
	return mode;
}

void Game() {
	char gamestage[STAGE_SIZE][STAGE_SIZE];
	Snake player;
	Food* foods[STAGE_SIZE * STAGE_SIZE + 1] = { NULL };
	int fnum = 0;
	player.head.x = STAGE_SIZE / 2;
	player.head.y = STAGE_SIZE / 2;
	player.tail = player.head;
	player.dir = LEFT;
	dir = player.dir;
	bool isDead = false;

	clock_t last_movetime; //���������� ������ �ð�
	double speed = 0.1f; //�̵��ӵ�

	clock_t last_gentime; //���������� ������ �ð�
	double gentime = 1.5f; //�����ӵ�

	//Game start
	addBody(&player);
	resetStage(gamestage, &player, foods, &fnum);
	printStage(gamestage);

	countDown();

	last_movetime = clock();
	last_gentime = clock();
	while (!isDead)
	{
		//���� �ð����� dir Ȯ��
		//�� �������� �̵�(��������, ��������)
		//dir 0���� �ʱ�ȭ
		//esc������ �Ͻ����� �����ϰ�
		//�������
		if ((float)(clock() - last_movetime) / CLOCKS_PER_SEC > speed) {
			isDead = !checkFront(gamestage, &player, foods, &fnum);
			moveSnake(&player);
			last_movetime = clock();
		}
		if ((clock() - last_gentime) / CLOCKS_PER_SEC > gentime) {
			foods[fnum] = makeFood(gamestage);
			fnum++;
			foods[fnum] = NULL;

			last_gentime = clock();
		}

		resetStage(gamestage, &player, foods, &fnum);
		printStage(gamestage);
	}
	deleteAllfood(foods, fnum);
	return;
}

void highScore() {
	const char* menu[6] = { "**********************************",
							"*                                *",
							"*  Your high score:              *",
							"*  (press esc to get out)        *",
							"*                                *",
							"**********************************" };
	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	clearScreen(BASIC_COLOR);

	prCharArr(menu, 6, BASIC_COLOR, lx, ly);
	int a, b, c, d;

	a = highscore / 1000;
	b = (highscore % 1000) / 100;
	c = (highscore % 100) / 10;
	d = (highscore % 10);
	chiBuffer[lx + (ly + 2) * WIDTH + 20].Char.UnicodeChar = a + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 21].Char.UnicodeChar = b + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 22].Char.UnicodeChar = c + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 23].Char.UnicodeChar = d + '0';
	printScreen();

	while (1) {
		if (dir == ESC) {
			dir = 0;
			break;
		}
		Sleep(1 / 60.0f * 1000.0f);
	}

	return;
}

void developinform() {
	const char* menu[9] = { "**********************************",
							"*                                *",
							"*    name: HYD                   *",
							"*                                *",
							"*    blog.naver.com/ydhan1024    *",
							"*                                *",
							"*    (press esc to get out)      *",
							"*                                *",
							"**********************************" };

	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	clearScreen(BASIC_COLOR);

	prCharArr(menu, 9, BASIC_COLOR, lx, ly);

	printScreen();

	while (1) {
		if (dir == ESC) {
			dir = 0;
			break;
		}
		Sleep(1 / 60.0f * 1000.0f);
	}

	return;
}

bool doAgain() {
	dir = 0;
	bool YN = true;
	const char* ment1 = "Back to Menu?";
	const char* ment2 = "Yes";
	const char* ment3 = "No";
	while (1) {

		if (dir == SPACE) {
			break;
		}

		if (dir != 0) {
			if ((dir == RIGHT && YN == true) || (dir == LEFT && YN == false)) {
				YN = !YN;
				dir = 0;
			}
		}

		prCharLine(ment1, BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
		if (YN == true) {
			prCharLine(ment2, SELECT_COLOR, WIDTH / 2, HEIGHT / 2 + 1);
			prCharLine(ment3, BASIC_COLOR, WIDTH / 2 + 7, HEIGHT / 2 + 1);
		}
		else if (YN == false) {
			prCharLine(ment2, BASIC_COLOR, WIDTH / 2, HEIGHT / 2 + 1);
			prCharLine(ment3, SELECT_COLOR, WIDTH / 2 + 7, HEIGHT / 2 + 1);
		}

		printScreen();
	}

	return YN;
}

void resetStage(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum) {
	//��ü�迭 �������� �ʱ�ȭ
	int i;
	for (i = 0; i < STAGE_SIZE; i++) {
		for (int j = 0; j < STAGE_SIZE; j++) {
			stage[i][j] = ' ';
		}
	}

	for (i = 0; i < STAGE_SIZE; i++) {
		stage[0][i] = WALL_TEX;
		stage[i][0] = WALL_TEX;
		stage[STAGE_SIZE - 1][i] = WALL_TEX;
		stage[i][STAGE_SIZE - 1] = WALL_TEX;
	}

	//������ġ�� O ����
	for (i = 0; i < sk->bodylen; i++) {
		stage[sk->body[i].x][sk->body[i].y] = BODY_TEX;
	}
	//�Ӹ� ��ġ�� @ ����
	stage[sk->head.x][sk->head.y] = HEAD_TEX;
	//���̿� $����
	for (i = 0; i < *fnum; i++) {
		stage[f[i]->pos.x][f[i]->pos.y] = FOOD_TEX;
	}

	return;
}

void printStage(char(*stage)[STAGE_SIZE]) {


	//ȭ�� �ʱ�ȭ

	clearScreen(BASIC_COLOR);

	//���ӽ������� ���
	for (int i = 0; i < STAGE_SIZE; i++) {
		for (int j = 0; j < STAGE_SIZE; j++) {
			chiBuffer[j * 2 + i * WIDTH].Char.UnicodeChar = stage[j][i];
			chiBuffer[j * 2 + i * WIDTH + 1].Char.UnicodeChar = ' ';
		}
	}

	//�������� ���

	int scorex = WIDTH / 2, scorey = STAGE_SIZE / 2;
	int a, b, c, d;

	prCharLine("Score : ", BASIC_COLOR, scorex - 8, scorey);

	a = score / 1000;
	b = (score % 1000) / 100;
	c = (score % 100) / 10;
	d = (score % 10);
	chiBuffer[scorex + scorey * WIDTH].Char.UnicodeChar = a + '0';
	chiBuffer[scorex + scorey * WIDTH + 1].Char.UnicodeChar = b + '0';
	chiBuffer[scorex + scorey * WIDTH + 2].Char.UnicodeChar = c + '0';
	chiBuffer[scorex + scorey * WIDTH + 3].Char.UnicodeChar = d + '0';

	//�ְ� ���� ���

	prCharLine("HighScore : ", BASIC_COLOR, scorex - 12, scorey + 2);

	a = highscore / 1000;
	b = (highscore % 1000) / 100;
	c = (highscore % 100) / 10;
	d = (highscore % 10);
	chiBuffer[scorex + (scorey + 2) * WIDTH].Char.UnicodeChar = a + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 1].Char.UnicodeChar = b + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 2].Char.UnicodeChar = c + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 3].Char.UnicodeChar = d + '0';

	//����
	printScreen();
	return;
}

void countDown() {
	prCharLine("1", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("2", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("3", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("GO!", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("   ", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
}

void addBody(Snake* sk) {
	//������ġ�� ���� ����
	sk->body[sk->bodylen] = sk->tail;
	//������+1
	sk->bodylen++;
	return;
}

int checkFront(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum) {
	int _dir = dir;
	Position next = sk->head;

	if ((_dir + sk->dir) != (UP + DOWN) && (_dir == RIGHT || _dir == LEFT || _dir == UP || _dir == DOWN)) {
		sk->dir = _dir;
	}

	switch (sk->dir)
	{
	case UP:
		next.y = sk->head.y - 1; //�迭�� 0���� �����̶� -1
		break;
	case DOWN:
		next.y = sk->head.y + 1;
		break;
	case RIGHT:
		next.x = sk->head.x + 1;
		break;
	case LEFT:
		next.x = sk->head.x - 1;
		break;
	default:
		break;
	}
	//���� 0 �̵����� 1
	if (stage[next.x][next.y] != ' ') {
		if (stage[next.x][next.y] == WALL_TEX) {
			return 0;
		}
		else if (stage[next.x][next.y] == FOOD_TEX) {
			for (int i = 0; i < *fnum; i++) {
				if (f[i]->pos.x == next.x && f[i]->pos.y == next.y) {
					eatFood(f, i, fnum);
					addBody(sk);
					return 1;
				}
			}
		}
		else if (stage[next.x][next.y] == BODY_TEX) {
			//���� Ȯ���� �ʿ� ����
			for (int i = 1; i < sk->bodylen; i++) {
				if (sk->body[i].x == next.x && sk->body[i].y == next.y) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void moveSnake(Snake* sk) {
	//�ڱ� ���������δ� ������ ����ó�� �ʿ�(dir �Է¹����� ���� ��ݵǸ� �� ���� �ʱ�)
	for (int i = sk->bodylen - 1; i > 0; i--) {
		//����� �պκ� ���� ��ǥ�� �̵�
		sk->body[i] = sk->body[i - 1];
	}
	if (sk->bodylen != 0) {
		//���� ���̰� 0�� �ƴ϶�� �Ӹ� ��ġ�� body[0] �̵�
		sk->body[0] = sk->head;
		sk->tail = sk->body[sk->bodylen - 1];
	}
	//�̵����⿡ ���� �Ӹ� �̵�
	switch (sk->dir)
	{
	case UP:
		sk->head.y--; //�迭�� 0���� �����̶� -1
		break;
	case DOWN:
		sk->head.y++;
		break;
	case RIGHT:
		sk->head.x++;
		break;
	case LEFT:
		sk->head.x--;
		break;
	default:
		break;
	}

	return;
}

void getDir() {
	while (isGameOn) {
		//���� �̵������ ���ų� ��ݵǸ� ���� �ٲ��� ����
		dir = _getch();

		Sleep(1 / 60.0f);
	}
}

Food* makeFood(char(*stage)[STAGE_SIZE]) {
	//static int fnum = 0; //���� ����
	//Food* food = (Food*)malloc(sizeof(Food)); //�� ���� �Ҵ�
	Food* food = new Food;

	while (1) {
		food->pos.x = rand() % (STAGE_SIZE - 2) + 1;
		food->pos.y = rand() % (STAGE_SIZE - 2) + 1;
		if (stage[food->pos.x][food->pos.y] == ' ') {
			break;
		}
	}
	return food;
}

void eatFood(Food* f[], int id, int* fnum) {
	delete f[id];
	for (int i = id; i < *fnum; i++) {
		f[i] = f[i + 1];
	}
	*fnum -= 1;
	score += 5;
}

void deleteAllfood(Food* f[], int fnum) {
	for (int i = 0; i < fnum; i++) {
		delete f[i];
	}
}

void loadScore(FILE* fp) {
	fopen_s(&fp, "highscore.bin", "rb");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &highscore);
		fclose(fp);
	}
}

void saveScore(FILE* fp) {
	if (score > highscore) {
		highscore = score;
		fopen_s(&fp, "highscore.bin", "wb");
		if (fp != NULL) {
			fprintf_s(fp, "%d", score);
			fclose(fp);
		}
	}
	score = 0;
}