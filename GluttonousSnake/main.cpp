#include<graphics.h>
#include<conio.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include"cJSON.h"

#define true 1
#define false 0
#define MAP_X 30
#define MAP_Y 30
#define DX 20
#define DY 20
#define SNAKE_INIT_LENGTH 5
#define KEY_LEFT 75
#define KEY_UP 72
#define KEY_RIGHT 77
#define KEY_DOWN 80
#define NOTHING 0
#define BODY 1
#define HEAD 2
#define FOOD 3
#define POI_WEED 4
#define LANDMINE 5
#define SLEEP_TIME_SIMPLE 200
#define SLEEP_TIME_DIFFICULT 100
#define SLEEP_TIME_IMPOSSIBLE 50
#define NUM_OF_POI_WEEDS_SIMPLE 2
#define NUM_OF_POI_WEEDS_DIFFICULT 5
#define NUM_OF_POI_WEEDS_IMPOSSIBLE 10
#define NUM_OF_LANDMINE_SIMPLE 1
#define NUM_OF_LANDMINE_DIFFICULT 3
#define NUM_OF_LANDMINE_IMPOSSIBLE 7
#define NUM_OF_FOODS_SIMPLE 10
#define NUM_OF_FOODS_DIFFICULT 5
#define NUM_OF_FOODS_IMPOSSIBLE 1
#define FLASH_FREQ 3
#define POI_WEEDS_TIME 1000
#define KINDS_OF_FOODS 12


#pragma warning(disable:4996)

typedef enum {
	single_Model,
	double_Model
}Model;
typedef enum {
	simple_Difficulty,
	difficult_Difficulty,
	impossible_Difficulty
}Difficulty;
typedef enum {
	startGame,
	modelSetting,
	difficultySetting,
	continueGame,
	nothing
}Operation;
typedef enum {
	UP,
	DOWN,
	RIGHT,
	LEFT,
	ERR
}Direction;
typedef enum {
	P1,
	P2
}Player;
typedef char Bool;
typedef struct Point {
	int x;
	int y;
}Point;
typedef struct snake {
	Point point;
	Bool isHead = false;
	Bool isTail = false;
	struct snake *previous = NULL;
	struct snake *next = NULL;
}Snake;

int sleepTime = SLEEP_TIME_SIMPLE;
int numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
int numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
int numOfFood = NUM_OF_FOODS_SIMPLE;
int food_x[NUM_OF_FOODS_SIMPLE];
int food_y[NUM_OF_FOODS_SIMPLE];
int poiWeeds_x[NUM_OF_POI_WEEDS_IMPOSSIBLE];
int poiWeeds_y[NUM_OF_POI_WEEDS_IMPOSSIBLE];
int landmine_x[NUM_OF_LANDMINE_IMPOSSIBLE];
int landmine_y[NUM_OF_LANDMINE_IMPOSSIBLE];
int snakeLength = SNAKE_INIT_LENGTH;
int foodIndex[NUM_OF_FOODS_SIMPLE];

char map[MAP_Y][MAP_X];

Snake *head, *tail;

Bool paintPoiWeeds = false;
Bool hasFood = false;
Bool hasPoiWeeds = false;
Bool hasLandmine = false;
Bool keyUsed = false;
Bool doContinue = false;
Bool isOver = false;

IMAGE startBackground;
IMAGE readyImage;
IMAGE gameBackground;
IMAGE settingBackground;
IMAGE b_continueGame;
IMAGE b_difficultySetting;
IMAGE b_modelSetting;
IMAGE foods[12];
IMAGE poiWeed;
IMAGE landmine;
IMAGE snake_1[5][4];
IMAGE snake_2[5][4];
IMAGE singleModel;
IMAGE doubleModel;
IMAGE choose;
IMAGE settingBack;
IMAGE simpleModel;
IMAGE difficultModel;
IMAGE impossibleModel;

Model currentModel = single_Model;

Difficulty currentDifficulty = simple_Difficulty;

void initMap();
void paintSnake(int, int, int, int);
void paintMap();
Snake* initSnake(int);
void updateMap();
void move(Direction);
Direction listenDirection(int, Direction);
void createFood(int, int, int, int);
void paintCell(int, int, int, int, int x, int y);
void growUp(Direction);
Bool eatFood(Direction);
void createPoiWeeds(int, int, int, int);
void reduceCell();
Bool eatPoiWeed(Direction);
void createLandmine(int, int, int, int);
void reduceHalf();
Bool eatLandmine(Direction);
Bool isGameOver();
void gameStart(Bool);
void loadAllImages();
Operation startMouseListening();
IMAGE* getSnakeImage(Snake*, Player);
void paintModelSetting();
void paintDifficultySetting();
void paintScore(int score);
void writeFile(Direction);
Bool readFile(Direction*);

int main() {
	srand(time(0));
	initgraph(800, 620);
	loadAllImages();
start:
	putimage(0, 0, &startBackground);
	putimage(30, 560, &b_modelSetting);
	putimage(300, 560, &b_difficultySetting);
	putimage(570, 560, &b_continueGame);
	Operation op = startMouseListening();
	switch (op) {
	case startGame:
		doContinue = false;
		goto startGame;
	case modelSetting:
		goto modelSetting;
	case difficultySetting:
		goto difficultySetting;
	case continueGame:
		doContinue = true;
		goto startGame;
	}
startGame:
	gameStart(doContinue);
	goto pause;
modelSetting:
	paintModelSetting();
	goto start;
difficultySetting:
	paintDifficultySetting();
	goto start;
pause:
	system("pause");
	return 0;
}

Bool readFile(Direction *direction) {
	FILE *fp;
	if ((fp=fopen("memory.json", "r")) != NULL)
	{
		char fileStr[3000];
		char *sp = fileStr;
		char temp;
		temp = fgetc(fp);
		while (temp != EOF) {
			*sp = temp;
			sp++;
			temp = fgetc(fp);
		}
		*sp = '\0';

		cJSON *memory = cJSON_Parse(fileStr);
		cJSON *snakes = cJSON_GetObjectItem(memory, "snakes");
		cJSON *snake_1 = cJSON_GetObjectItem(snakes, "snake_1");
		cJSON *snake_2 = cJSON_GetObjectItem(snakes, "snake_2");
		cJSON *snake_1_x_arr = cJSON_GetObjectItem(snake_1, "snake_1_x");
		cJSON *snake_2_x_arr = cJSON_GetObjectItem(snake_2, "snake_2_x");
		cJSON *snake_1_y_arr = cJSON_GetObjectItem(snake_1, "snake_1_y");
		cJSON *snake_2_y_arr = cJSON_GetObjectItem(snake_2, "snake_2_y");
		cJSON *food = cJSON_GetObjectItem(memory, "food");
		cJSON *poi_weed = cJSON_GetObjectItem(memory, "poi_weed");
		cJSON *landmine = cJSON_GetObjectItem(memory, "landmine");
		cJSON *food_x_arr = cJSON_GetObjectItem(food, "food_x");
		cJSON *food_y_arr = cJSON_GetObjectItem(food, "food_y");
		cJSON *foodIndex_arr = cJSON_GetObjectItem(food, "foodIndex");
		cJSON *poi_weed_x_arr = cJSON_GetObjectItem(poi_weed, "poi_weed_x");
		cJSON *poi_weed_y_arr = cJSON_GetObjectItem(poi_weed, "poi_weed_y");
		cJSON *landmine_x_arr = cJSON_GetObjectItem(landmine, "landmine_x");
		cJSON *landmine_y_arr = cJSON_GetObjectItem(landmine, "landmine_y");
		
		Model model = (Model)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "model")));
		Difficulty difficulty = (Difficulty)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "difficulty")));
		Direction direction_1 = (Direction)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "direction_1")));

		int temp_x, temp_y;
		int snakeSize_1 = cJSON_GetArraySize(snake_1_x_arr);
		head = initSnake(snakeSize_1);
		Snake *snakeTemp = head;
		for (int i = 0; i < snakeSize_1; i++)
		{
			temp_x = atoi(cJSON_Print(cJSON_GetArrayItem(snake_1_x_arr, i)));
			temp_y = atoi(cJSON_Print(cJSON_GetArrayItem(snake_1_y_arr, i)));
			snakeTemp->point.x = temp_x;
			snakeTemp->point.y = temp_y;
			snakeTemp = snakeTemp->next;
		}

		int foodSize = cJSON_GetArraySize(food_x_arr);
		for (int i = 0; i < NUM_OF_FOODS_SIMPLE; i++)
		{
			if (i < foodSize)
			{
				food_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(food_x_arr, i)));
				food_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(food_y_arr, i)));
				foodIndex[i] = atoi(cJSON_Print(cJSON_GetArrayItem(foodIndex_arr, i)));
			}
			else {
				food_x[i] = -2;
				food_y[i] = -2;
			}
			
		}

		int poi_weedSize = cJSON_GetArraySize(poi_weed_x_arr);
		for (int i = 0; i < NUM_OF_POI_WEEDS_IMPOSSIBLE; i++)
		{
			if (i < poi_weedSize) {
				poiWeeds_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(poi_weed_x_arr, i)));
				poiWeeds_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(poi_weed_y_arr, i)));
			}
			else {
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
			}
		}

		int landmineSize = cJSON_GetArraySize(landmine_x_arr);
		for (int i = 0; i < NUM_OF_LANDMINE_IMPOSSIBLE; i++)
		{
			if (i < landmineSize) {
				landmine_x[i] = atoi(cJSON_Print(cJSON_GetArrayItem(landmine_x_arr, i)));
				landmine_y[i] = atoi(cJSON_Print(cJSON_GetArrayItem(landmine_y_arr, i)));
			}
			else {
				landmine_x[i] = -2;
				landmine_y[i] = -2;
			}
		}

		paintPoiWeeds = true;
		hasFood = true;
		hasPoiWeeds = true;
		hasLandmine = true;

		*direction = direction_1;
		currentModel = model;
		currentDifficulty = difficulty;
		snakeLength = snakeSize_1;
		return true;
	}
	else
	{
		return false;
	}
}

void writeFile(Direction direction) {
	FILE *fp;
	cJSON *memory = cJSON_CreateObject();
	cJSON *snakes = cJSON_CreateObject();
	cJSON *snake_1 = cJSON_CreateObject();
	cJSON *snake_2 = cJSON_CreateObject();
	cJSON *snake_1_x_arr = cJSON_CreateArray();
	cJSON *snake_2_x_arr = cJSON_CreateArray();
	cJSON *snake_1_y_arr = cJSON_CreateArray();
	cJSON *snake_2_y_arr = cJSON_CreateArray();
	cJSON *food = cJSON_CreateObject();
	cJSON *poi_weed = cJSON_CreateObject();
	cJSON *landmine = cJSON_CreateObject();
	cJSON *food_x_arr = cJSON_CreateArray();
	cJSON *food_y_arr = cJSON_CreateArray();
	cJSON *foodIndex_arr = cJSON_CreateArray();
	cJSON *poi_weed_x_arr = cJSON_CreateArray();
	cJSON *poi_weed_y_arr = cJSON_CreateArray();
	cJSON *landmine_x_arr = cJSON_CreateArray();
	cJSON *landmine_y_arr = cJSON_CreateArray();

	cJSON_AddNumberToObject(memory, "model", currentModel);
	cJSON_AddNumberToObject(memory, "difficulty", currentDifficulty);
	cJSON_AddNumberToObject(memory, "direction_1", direction);
	cJSON_AddItemToObject(memory, "snakes", snakes);
	cJSON_AddItemToObject(snakes, "snake_1", snake_1);
	cJSON_AddItemToObject(snakes, "snake_2", snake_2);
	cJSON_AddItemToObject(snake_1, "snake_1_x", snake_1_x_arr);
	cJSON_AddItemToObject(snake_1, "snake_1_y", snake_1_y_arr);
	cJSON_AddItemToObject(snake_2, "snake_2_x", snake_2_x_arr);
	cJSON_AddItemToObject(snake_2, "snake_2_y", snake_2_y_arr);
	cJSON_AddItemToObject(memory, "food", food);
	cJSON_AddItemToObject(food, "food_x", food_x_arr);
	cJSON_AddItemToObject(food, "food_y", food_y_arr);
	cJSON_AddItemToObject(food, "foodIndex", foodIndex_arr);
	cJSON_AddItemToObject(memory, "poi_weed", poi_weed);
	cJSON_AddItemToObject(poi_weed, "poi_weed_x", poi_weed_x_arr);
	cJSON_AddItemToObject(poi_weed, "poi_weed_y", poi_weed_y_arr);
	cJSON_AddItemToObject(memory, "landmine", landmine);
	cJSON_AddItemToObject(landmine, "landmine_x", landmine_x_arr);
	cJSON_AddItemToObject(landmine, "landmine_y", landmine_y_arr);

	Snake *temp = head;
	while (temp != NULL) {
		cJSON_AddItemToArray(snake_1_x_arr, cJSON_CreateNumber(temp->point.x));
		cJSON_AddItemToArray(snake_1_y_arr, cJSON_CreateNumber(temp->point.y));
		temp = temp->next;
	}

	for (int i = 0; i < numOfFood; i++)
	{
		cJSON_AddItemToArray(food_x_arr, cJSON_CreateNumber(food_x[i]));
		cJSON_AddItemToArray(food_y_arr, cJSON_CreateNumber(food_y[i]));
		cJSON_AddItemToArray(foodIndex_arr, cJSON_CreateNumber(foodIndex[i]));
	}

	for (int i = 0; i < numOfPoiWeeds; i++)
	{
		cJSON_AddItemToArray(poi_weed_x_arr, cJSON_CreateNumber(poiWeeds_x[i]));
		cJSON_AddItemToArray(poi_weed_y_arr, cJSON_CreateNumber(poiWeeds_y[i]));
	}

	for (int i = 0; i < numOfLandmine; i++)
	{
		cJSON_AddItemToArray(landmine_x_arr, cJSON_CreateNumber(landmine_x[i]));
		cJSON_AddItemToArray(landmine_y_arr, cJSON_CreateNumber(landmine_y[i]));
	}

	fp = fopen("memory.json", "w");
	fputs(cJSON_Print(memory), fp);
	fclose(fp);
}

void paintScore(int score) {
	char scoreStr[5];
	_itoa_s(score, scoreStr, 10);
	setbkmode(TRANSPARENT);
	settextcolor(BROWN);
	settextstyle(60, 0, "Á¥Êé");
	outtextxy(650, 50, "µÃ·Ö");
	settextcolor(RED);
	settextstyle(40, 0, "Ó×Ô²");
	outtextxy(700, 130, scoreStr);
}

IMAGE* getSnakeImage(Snake* snakeBody, Player player) {
	IMAGE(*snakeArr)[4] = NULL;
	switch (player)
	{
		case P1:
			snakeArr = snake_1;
			break;
		case P2:
			snakeArr = snake_2;
			break;
	}

	int px, py;
	int x, y;
	int nx, ny;
	if (snakeBody == head)
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		nx = snakeBody->next->point.x;
		ny = snakeBody->next->point.y;

		if (x == nx && y == ny - 1)
		{
			return &snakeArr[0][0];
		}
		else if (x == nx + 1 && y == ny)
		{
			return &snakeArr[0][1];
		}
		else if (x == nx && y == ny + 1)
		{
			return &snakeArr[0][2];
		}
		else if (x == nx - 1 && y == ny)
		{
			return &snakeArr[0][3];
		}
		return NULL;
	}
	else if (snakeBody == tail)
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		px = snakeBody->previous->point.x;
		py = snakeBody->previous->point.y;

		if (px == x && py == y - 1)
		{
			return &snakeArr[4][0];
		}
		else if (px == x + 1 && py == y)
		{
			return &snakeArr[4][1];
		}
		else if (px == x && py == y + 1)
		{
			return &snakeArr[4][2];
		}
		else if (px == x - 1 && py == y)
		{
			return &snakeArr[4][3];
		}
	}
	else
	{
		x = snakeBody->point.x;
		y = snakeBody->point.y;

		px = snakeBody->previous->point.x;
		py = snakeBody->previous->point.y;

		nx = snakeBody->next->point.x;
		ny = snakeBody->next->point.y;

		if (px == nx && py == ny - 2)
		{
			return &snakeArr[1][0];
		}
		else if (px == nx + 2 && py == ny)
		{
			return &snakeArr[1][1];
		}
		else if (px == nx && py == ny + 2)
		{
			return &snakeArr[1][2];
		}
		else if (px == nx - 2 && py == ny)
		{
			return &snakeArr[1][3];
		}
		else if (px == nx - 1 && py == ny + 1 && x == nx)
		{
			return &snakeArr[2][0];
		}
		else if (px == nx - 1 && py == ny - 1 && x == px)
		{
			return &snakeArr[2][1];
		}
		else if (px == nx + 1 && py == ny - 1 && x == nx)
		{
			return &snakeArr[2][2];
		}
		else if (px == nx + 1 && py == ny + 1 && x == px)
		{
			return &snakeArr[2][3];
		}
		else if (px == nx + 1 && py == ny + 1 && x == nx)
		{
			return &snakeArr[3][0];
		}
		else if (px == nx - 1 && py == ny + 1 && x == px)
		{
			return &snakeArr[3][1];
		}
		else if (px == nx - 1 && py == ny - 1 && x == nx)
		{
			return &snakeArr[3][2];
		}
		else if (px == nx + 1 && py == ny - 1 && x == px)
		{
			return &snakeArr[3][3];
		}
	}
	return NULL;
}

Operation startMouseListening() {
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 315 && x < 490 && y>415 && y < 470)
				{
					return startGame;
				}
				else if (x > 30 && x < 230 && y > 560 && y < 600)
				{
					//model setting
					return modelSetting;
				}
				else if (x > 300 && x < 500 && y > 560 && y < 600)
				{
					//difficulty setting
					return difficultySetting;
				}
				else if (x > 570 && x < 770 && y > 560 && y < 600) {
					//continue game
					return continueGame;
				}
			}
		}
	}
}

void paintDifficultySetting() {
	int choose_y = 120;
startPaint:
	putimage(0, 0, &settingBackground);
	putimage(300, 130, &simpleModel);
	putimage(300, 230, &difficultModel);
	putimage(300, 330, &impossibleModel);
	putimage(300, 430, &settingBack);
	putimage(520, choose_y, &choose);
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 300 && x < 500 && y>130 && y < 190)
				{
					//simple model
					choose_y = 120;
					sleepTime = SLEEP_TIME_SIMPLE;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_SIMPLE;
					numOfLandmine = NUM_OF_LANDMINE_SIMPLE;
					numOfFood = NUM_OF_FOODS_SIMPLE;
					currentDifficulty = simple_Difficulty;
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 230 && y < 290)
				{
					//difficult model
					choose_y = 220;
					sleepTime = SLEEP_TIME_DIFFICULT;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_DIFFICULT;
					numOfLandmine = NUM_OF_LANDMINE_DIFFICULT;
					numOfFood = NUM_OF_FOODS_DIFFICULT;
					currentDifficulty = difficult_Difficulty;
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 330 && y < 390)
				{
					//impossible model
					choose_y = 320;
					sleepTime = SLEEP_TIME_IMPOSSIBLE;
					numOfPoiWeeds = NUM_OF_POI_WEEDS_IMPOSSIBLE;
					numOfLandmine = NUM_OF_LANDMINE_IMPOSSIBLE;
					numOfFood = NUM_OF_FOODS_IMPOSSIBLE;
					currentDifficulty = impossible_Difficulty;
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 430 && y < 490)
				{
					goto done;
				}
			}
		}
	}
done:;
}

void paintModelSetting() {
	int choose_y = 140;
startPaint:
	putimage(0, 0, &settingBackground);
	putimage(300, 150, &singleModel);
	putimage(300, 275, &doubleModel);
	putimage(300, 400, &settingBack);
	putimage(520, choose_y, &choose);
	MOUSEMSG msg;
	int x, y;
	while (true) {
		if (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 300 && x < 500 && y>150 && y < 210)
				{
					//single model
					choose_y = 140;
					currentModel = single_Model;
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 275 && y < 335)
				{
					//double model
					choose_y = 265;
					currentModel = double_Model;
					goto startPaint;
				}
				else if (x > 300 && x < 500 && y > 400 && y < 460)
				{
					goto done;
				}
			}
		}
	}
done:;
}

void loadAllImages() {
	loadimage(&startBackground, "images/background/background.jpg");
	loadimage(&readyImage, "images/background/ready.png");
	loadimage(&gameBackground, "images/background/gameBackground.png");
	loadimage(&settingBackground, "images/background/settingBackground.png");
	loadimage(&b_continueGame, "images/buttons/b_continueGame.png", 200, 40);
	loadimage(&b_difficultySetting, "images/buttons/b_difficultySetting.png", 200, 40);
	loadimage(&b_modelSetting, "images/buttons/b_modelSetting.png", 200, 40);
	loadimage(&settingBack, "images/buttons/b_settingBack.png", 200, 60);
	loadimage(&singleModel, "images/buttons/b_single.png", 200, 60);
	loadimage(&doubleModel, "images/buttons/b_double.png", 200, 60);
	loadimage(&simpleModel, "images/buttons/b_simple.png", 200, 60);
	loadimage(&difficultModel, "images/buttons/b_difficult.png", 200, 60);
	loadimage(&impossibleModel, "images/buttons/b_impossible.png", 200, 60);
	loadimage(&choose, "images/buttons/choose.png", 70, 70);
	loadimage(&poiWeed, "images/foods/poi_weed.png");
	loadimage(&landmine, "images/foods/landmine.png");
	loadimage(foods, "images/foods/food_1.png");
	loadimage(foods + 1, "images/foods/food_2.png");
	loadimage(foods + 2, "images/foods/food_3.png");
	loadimage(foods + 3, "images/foods/food_4.png");
	loadimage(foods + 4, "images/foods/food_5.png");
	loadimage(foods + 5, "images/foods/food_6.png");
	loadimage(foods + 6, "images/foods/food_7.png");
	loadimage(foods + 7, "images/foods/food_8.png");
	loadimage(foods + 8, "images/foods/food_9.png");
	loadimage(foods + 9, "images/foods/food_10.png");
	loadimage(foods + 10, "images/foods/food_11.png");
	loadimage(foods + 11, "images/foods/food_12.png");
	//up right down left
	//snake_1 init
	//init head
	loadimage(&snake_1[0][0], "images/snake/head_up_1.png");
	loadimage(&snake_1[0][1], "images/snake/head_right_1.png");
	loadimage(&snake_1[0][2], "images/snake/head_down_1.png");
	loadimage(&snake_1[0][3], "images/snake/head_left_1.png");
	//init body
	loadimage(&snake_1[1][0], "images/snake/body_up_1.png");
	loadimage(&snake_1[1][1], "images/snake/body_right_1.png");
	loadimage(&snake_1[1][2], "images/snake/body_down_1.png");
	loadimage(&snake_1[1][3], "images/snake/body_left_1.png");
	//init body_r
	//utol rtou dtor ltod
	loadimage(&snake_1[2][0], "images/snake/body_utol_1.png");
	loadimage(&snake_1[2][1], "images/snake/body_rtou_1.png");
	loadimage(&snake_1[2][2], "images/snake/body_dtor_1.png");
	loadimage(&snake_1[2][3], "images/snake/body_ltod_1.png");
	//utor rtod dtol ltou
	loadimage(&snake_1[3][0], "images/snake/body_utor_1.png");
	loadimage(&snake_1[3][1], "images/snake/body_rtod_1.png");
	loadimage(&snake_1[3][2], "images/snake/body_dtol_1.png");
	loadimage(&snake_1[3][3], "images/snake/body_ltou_1.png");
	//init tail
	loadimage(&snake_1[4][0], "images/snake/tail_up_1.png");
	loadimage(&snake_1[4][1], "images/snake/tail_right_1.png");
	loadimage(&snake_1[4][2], "images/snake/tail_down_1.png");
	loadimage(&snake_1[4][3], "images/snake/tail_left_1.png");
	
	//snake_2 init
	//init head
	loadimage(&snake_2[0][0], "images/snake/head_up_2.png");
	loadimage(&snake_2[0][1], "images/snake/head_right_2.png");
	loadimage(&snake_2[0][2], "images/snake/head_down_2.png");
	loadimage(&snake_2[0][3], "images/snake/head_left_2.png");
	//init body
	loadimage(&snake_2[1][0], "images/snake/body_up_2.png");
	loadimage(&snake_2[1][1], "images/snake/body_right_2.png");
	loadimage(&snake_2[1][2], "images/snake/body_down_2.png");
	loadimage(&snake_2[1][3], "images/snake/body_left_2.png");
	//init body_r
	//utol rtou dtor ltod
	loadimage(&snake_2[2][0], "images/snake/body_utol_2.png");
	loadimage(&snake_2[2][1], "images/snake/body_rtou_2.png");
	loadimage(&snake_2[2][2], "images/snake/body_dtor_2.png");
	loadimage(&snake_2[2][3], "images/snake/body_ltod_2.png");
	//utor rtod dtol ltou
	loadimage(&snake_2[3][0], "images/snake/body_utor_2.png");
	loadimage(&snake_2[3][1], "images/snake/body_rtod_2.png");
	loadimage(&snake_2[3][2], "images/snake/body_dtol_2.png");
	loadimage(&snake_2[3][3], "images/snake/body_ltou_2.png");
	//init tail
	loadimage(&snake_2[4][0], "images/snake/tail_up_2.png");
	loadimage(&snake_2[4][1], "images/snake/tail_right_2.png");
	loadimage(&snake_2[4][2], "images/snake/tail_down_2.png");
	loadimage(&snake_2[4][3], "images/snake/tail_left_2.png");
}

void gameStart(Bool doContinue) {
	int score;
	int poiWeedsTime = 0;
	Direction direction = LEFT, d_temp;
	int keyASC;
	int poi_times = 0;
	Bool start = false;
	putimage(0, 200, &readyImage);
	Sleep(2000);
	if (doContinue)
	{
		if (readFile(&direction)) {
			
		}
		else
		{
			initMap();
			head = initSnake(SNAKE_INIT_LENGTH);
		}
	}
	else
	{
		initMap();
		head = initSnake(SNAKE_INIT_LENGTH);
	}
	cleardevice();
	while (true)
	{
		if (poiWeedsTime <= POI_WEEDS_TIME)
		{
			poiWeedsTime += sleepTime / FLASH_FREQ;
		}
		paintPoiWeeds = !paintPoiWeeds;
		keyUsed = false;
		cleardevice();
		putimage(0, 0, &gameBackground);
		updateMap();
		paintMap();
		score = 10 * (snakeLength - SNAKE_INIT_LENGTH);
		paintScore(score);
		if (!start)
		{
			putimage(0, 200, &readyImage);
			Sleep(1000);
			start = true;
		}
		Sleep(sleepTime / FLASH_FREQ);
		if (FLASH_FREQ != poi_times++)
		{
			continue;
		}
		else
		{
			poi_times = 0;
			writeFile(direction);
		}
		while (_kbhit() && !keyUsed)
		{
			keyASC = _getch();
			d_temp = listenDirection(keyASC, direction);
			if (d_temp != ERR)
			{
				direction = d_temp;
				keyUsed = true;
			}
		}
		if (eatFood(direction))
		{
			growUp(direction);
			snakeLength++;
		}
		else if (eatPoiWeed(direction))
		{
			if (snakeLength <= 2)
			{
				isOver = true;
			}
			reduceCell();
			snakeLength--;
			move(direction);
		}
		else if (eatLandmine(direction)) {
			if (snakeLength <= 2)
			{
				isOver = true;
			}
			reduceHalf();
			snakeLength = snakeLength - snakeLength / 2;
			move(direction);
		}
		else
		{
			move(direction);
		}
		if (isGameOver() || isOver)
		{
			break;
		}
	}
}

Bool isGameOver() {
	int x = head->point.x;
	int y = head->point.y;
	if (x < 0 || x > MAP_X - 1 || y < 0 || y > MAP_Y - 1 || map[x][y] == BODY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool eatLandmine(Direction direction) {
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landmine_y[i] + 1)
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landmine_y[i] - 1)
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] + 1 && head->point.y == landmine_y[i])
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head->point.x == landmine_x[i] - 1 && head->point.y == landmine_y[i])
			{
				landmine_x[i] = -2;
				landmine_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void reduceHalf() {
	int t = snakeLength / 2;
	for (int i = 0; i < t; i++)
	{
		tail = tail->previous;
		//free(tail->next);
	}
	tail->isTail = true;
	tail->next = NULL;
}

void createLandmine(int x0, int y0, int dx, int dy) {
	if (!hasLandmine)
	{
		setfillcolor(BLACK);
		for (int i = 0; i < numOfLandmine; i++)
		{
			landmine_x[i] = rand() % 30;
			landmine_y[i] = rand() % 30;
			while (map[landmine_x[i]][landmine_y[i]] != 0) {
				landmine_x[i] = rand() % 30;
				landmine_y[i] = rand() % 30;
			}
			map[landmine_x[i]][landmine_y[i]] = LANDMINE;
			putimage(x0 + landmine_x[i] * dx, y0 + landmine_y[i] * dy, &landmine);
			//paintCell(x0, y0, dx, dy, landmine_x[i], landmine_y[i]);
		}
		hasLandmine = true;
	}
	else
	{
		setfillcolor(BLACK);
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (landmine_x[i] != -2 && landmine_y[i] != -2)
			{
				map[landmine_x[i]][landmine_y[i]] = LANDMINE;
				putimage(x0 + landmine_x[i] * dx, y0 + landmine_y[i] * dy, &landmine);
				//paintCell(x0, y0, dx, dy, landmine_x[i], landmine_y[i]);
			}
		}
	}
}

Bool eatPoiWeed(Direction direction) {
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] && head->point.y == poiWeeds_y[i] + 1)
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] && head->point.y == poiWeeds_y[i] - 1)
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] + 1 && head->point.y == poiWeeds_y[i])
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head->point.x == poiWeeds_x[i] - 1 && head->point.y == poiWeeds_y[i])
			{
				poiWeeds_x[i] = -2;
				poiWeeds_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void reduceCell() {
	tail = tail->previous;
	tail->isTail = true;
	//free(tail->next);
	tail->next = NULL;
}

void createPoiWeeds(int x0, int y0, int dx, int dy) {
	if (!hasPoiWeeds)
	{
		setfillcolor(YELLOW);
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			poiWeeds_x[i] = rand() % 30;
			poiWeeds_y[i] = rand() % 30;
			while (map[poiWeeds_x[i]][poiWeeds_y[i]] != 0) {
				poiWeeds_x[i] = rand() % 30;
				poiWeeds_y[i] = rand() % 30;
			}
			map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
			putimage(x0 + poiWeeds_x[i] * dx, y0 + poiWeeds_y[i] * dy, &poiWeed);
			//paintCell(x0, y0, dx, dy, poiWeeds_x[i], poiWeeds_y[i]);
		}
		hasPoiWeeds = true;
	}
	else
	{
		setfillcolor(YELLOW);
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (poiWeeds_x[i] != -2 && poiWeeds_y[i] != -2)
			{
				map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
				if (paintPoiWeeds)
				{
					putimage(x0 + poiWeeds_x[i] * dx, y0 + poiWeeds_y[i] * dy, &poiWeed);
					//paintCell(x0, y0, dx, dy, poiWeeds_x[i], poiWeeds_y[i]);
				}
			}
		}
	}
}

Bool eatFood(Direction direction) {
	switch (direction)
	{
	case UP:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] && head->point.y == food_y[i] + 1)
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] && head->point.y == food_y[i] - 1)
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] + 1 && head->point.y == food_y[i])
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head->point.x == food_x[i] - 1 && head->point.y == food_y[i])
			{
				food_x[i] = -2;
				food_y[i] = -2;
				return true;
			}
		}
		return false;
	default:
		return false;
	}
}

void growUp(Direction direction) {
	Snake* newCell = (Snake*)malloc(sizeof(Snake));
	newCell->next = head;
	head->previous = newCell;
	head->isHead = false;
	newCell->isHead = true;
	head = newCell;
	switch (direction)
	{
	case UP:
		head->point.x = head->next->point.x;
		head->point.y = head->next->point.y - 1;
		break;
	case DOWN:
		head->point.x = head->next->point.x;
		head->point.y = head->next->point.y + 1;
		break;
	case LEFT:
		head->point.x = head->next->point.x - 1;
		head->point.y = head->next->point.y;
		break;
	case RIGHT:
		head->point.x = head->next->point.x + 1;
		head->point.y = head->next->point.y;
		break;
	default:
		break;
	}
	
}

void paintCell(int x0, int y0, int dx, int dy, int x, int y) {
	solidrectangle(x0 + x * dx, y0 + y * dy, x0 + x * dx + dx, y0 + y * dy + dy);
}

void createFood(int x0, int y0, int dx, int dy) {
	if (!hasFood)
	{
		for (int i = 0; i < numOfFood; i++) {
			food_x[i] = -2;
			food_y[i] = -2;
			foodIndex[i] = rand() % KINDS_OF_FOODS;
		}
		hasFood = true;
	}
	for (int i = 0; i < numOfFood; i++)
	{
		if (food_x[i] == -2 || food_y[i] == -2)
		{
			food_x[i] = rand() % 30;
			food_y[i] = rand() % 30;
			while (map[food_x[i]][food_y[i]] != 0)
			{
				food_x[i] = rand() % 30;
				food_y[i] = rand() % 30;
			}
			foodIndex[i] = rand() % KINDS_OF_FOODS;
		}
		map[food_x[i]][food_y[i]] = FOOD;
		setfillcolor(BLUE);
		putimage(x0 + food_x[i] * dx, y0 + food_y[i] * dy, &foods[foodIndex[i]]);
		//paintCell(x0, y0, dx, dy, food_x[i], food_y[i]);
	}
}

Direction listenDirection(int key, Direction direction) {
	switch (key)
	{
	case KEY_UP:
		if (direction == DOWN)
		{
			return ERR;
		}
		return UP;
	case KEY_DOWN:
		if (direction == UP)
		{
			return ERR;
		}
		return DOWN;
	case KEY_RIGHT:
		if (direction == LEFT)
		{
			return ERR;
		}
		return RIGHT;
	case KEY_LEFT:
		if (direction == RIGHT)
		{
			return ERR;
		}
		return LEFT;
	default:
		return ERR;
	}
}

void move(Direction direction) {
	Snake *temp;
	switch (direction)
	{
	case LEFT:
		tail->point.x = head->point.x - 1;
		tail->point.y = head->point.y;
		break;
	case RIGHT:
		tail->point.x = head->point.x + 1;
		tail->point.y = head->point.y;
		break;
	case UP:
		tail->point.x = head->point.x;
		tail->point.y = head->point.y - 1;
		break;
	case DOWN:
		tail->point.x = head->point.x;
		tail->point.y = head->point.y + 1;
		break;
	default:
		break;
	}
	temp = head;
	head = tail;
	tail = tail->previous;
	temp->previous = head;
	head->next = temp;
	tail->next = NULL;
	head->previous = NULL;
}

void initMap() {
	for (int i = 0; i < MAP_Y; i++)
	{
		for (int j = 0; j < MAP_X; j++)
		{
			map[j][i] = NOTHING;
		}
	}
}

void paintMap() {
	int x0 = 10, y0 = 10;
	int dx = DX, dy = DY;
	setcolor(BLUE);
	//rectangle(10, 10, 10 + 30 * DX, 10 + 30 * DY);
	paintSnake(x0, y0, dx, dy);
	createPoiWeeds(x0, y0, dx, dy);
	createLandmine(x0, y0, dx, dy);
	createFood(x0, y0, dx, dy);
}

void paintSnake(int x0, int y0, int dx, int dy) {
	/*for (int i = 0; i < MAP_Y; i++)
	{
		for (int j = 0; j < MAP_X; j++) {
			if (map[j][i] == NOTHING)
			{
				continue;
			}
			if (map[j][i] == BODY)
			{
				setfillcolor(GREEN);
			}
			else if (map[j][i] == HEAD)
			{
				setfillcolor(RED);
			}
			else
			{
				continue;
			}
			paintCell(x0, y0, dx, dy, j, i);
		}
	}*/
	IMAGE* cell;
	Snake* snakeCell;
	snakeCell = head;
	int x, y;
	while (snakeCell != NULL)
	{
		cell = getSnakeImage(snakeCell, P1);
		x = snakeCell->point.x;
		y = snakeCell->point.y;
		putimage(x0 + dx * x, y0 + dy * y, cell);
		snakeCell = snakeCell->next;
	}
}

Snake* initSnake(int length) {
	Snake*head;
	head = (Snake*)malloc(sizeof(Snake)*length);
	tail = &head[length - 1];
	for (int i = 0; i < length; i++)
	{
		if (i < length - 1)
		{
			head[i].next = &head[i + 1];
			head[i + 1].previous = &head[i];
		}
		head[i].point.x = MAP_X / 2 + i;
		head[i].point.y = MAP_Y / 2;
	}
	head->isHead = true;
	tail->isTail = true;
	tail->next = NULL;
	head->previous = NULL;
	return head;
}

void updateMap() {
	initMap();
	Snake* temp = head;
	int x, y;
	while (temp != NULL) {
		if (temp == head)
		{
			x = temp->point.x;
			y = temp->point.y;
			map[x][y] = HEAD;
		}
		else
		{
			x = temp->point.x;
			y = temp->point.y;
			map[x][y] = BODY;
		}
		temp = temp->next;
	}

	for (int i = 0; i < NUM_OF_FOODS_SIMPLE; i++)
	{
		if (food_x[i] != -2 && food_y[i] != -2) {
			map[food_x[i]][food_y[i]] = FOOD;
		}
	}

	for (int i = 0; i < NUM_OF_POI_WEEDS_IMPOSSIBLE; i++)
	{
		if (poiWeeds_x[i] != -2 && poiWeeds_y[i] != -2) {
			map[poiWeeds_x[i]][poiWeeds_y[i]] = POI_WEED;
		}
	}

	for (int i = 0; i < NUM_OF_LANDMINE_IMPOSSIBLE; i++)
	{
		if (landmine_x[i] != -2 && landmine_y[i] != -2) {
			map[landmine_x[i]][landmine_y[i]] = LANDMINE;
		}
	}
}
