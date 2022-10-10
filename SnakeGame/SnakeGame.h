#define STAGE_SIZE 20
#define WIDTH 120
#define HEIGHT 30
#define WALL_TEX '*'
#define FOOD_TEX '$'
#define HEAD_TEX '@'
#define BODY_TEX 'O'
#define BASIC_COLOR 15
#define SELECT_COLOR 23

#include<stdio.h>
#include<Windows.h>
#include<time.h>
#include<conio.h>
#include<thread>

enum { UP = 72, DOWN = 80, RIGHT = 77, LEFT = 75, ESC = 27, SPACE = 32, ENTER = 13 }; //����Ű �ƽ�Ű�ڵ� ��

HANDLE hStdout, hNewScreenBuffer, hNewScreenBuffer2;
SMALL_RECT srctWriteRect;
CHAR_INFO chiBuffer[WIDTH * HEIGHT];
COORD coordBufSize;
COORD coordBufCoord;
BOOL fSuccess;

HANDLE curScreenBufferHandle;

struct Position
{
	int x, y;
};

struct Snake
{
	Position head;
	Position body[STAGE_SIZE*STAGE_SIZE];
	Position tail;
	int bodylen = 0;
	int dir;
};

struct Food
{
	Position pos;
};

int Menu(); //�޴�
void Game(); //����(������ȯ)
void highScore();
void developinform();

bool doAgain();

void resetStage(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum); //�迭�ȿ� ��� ���� ǥ��
void printStage(char(*stage)[STAGE_SIZE]); //����ȭ�����
void countDown();

void getDir();
void moveSnake(Snake* sk); // �տ��ִ� ��ġ�� �ű�
Food* makeFood(char(*stage)[STAGE_SIZE]); // ���� ��������
void eatFood(Food* f[], int id, int* fnum);
void deleteAllfood(Food* f[], int fnum);
void addBody(Snake* sk); //���� �Ծ����� ���� ��ġ�� ��ü+1
int checkFront(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum); //�����̱� �� �տ�Ȯ��

void loadScore(FILE* fp); //���� �ҷ���
void saveScore(FILE* fp); //���� ����

void makeBuffer(); // ���۸� ����
void prCharLine(const char* ch, int atb, int x, int y); //�� ������ ���
void prCharArr(const char* ch[], int len, int atb, int x, int y); //�迭 ���
void clearScreen(int color);
void printScreen();

bool isGameOn = true;
int dir = LEFT; //����Ű �Է»�Ȳ
int highscore = 0;
int score = 0; //����