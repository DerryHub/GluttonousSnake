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
#define SLEEP_TIME_SIMPLE 150
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
#define SCORE_SIMPLE 200
#define SCORE_DIFFICULT 300
#define SCORE_IMPOSSIBLE 400
#define FLASH_FREQ 3
#define POI_WEEDS_TIME 5000
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
int scoreThresholdValue = SCORE_SIMPLE;

char map[MAP_Y][MAP_X];

Snake *head_1, *tail_1;
Snake *head_2, *tail_2;

Bool paintPoiWeeds = false;
Bool hasFood = false;
Bool hasPoiWeeds = false;
Bool hasLandmine = false;
Bool keyUsed = false;
Bool doContinue = false;
Bool isOver = false;
Bool isWin;
Bool isPause = false;
Bool poi_weedOff = false;

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
IMAGE singleLoss;
IMAGE singleWin;
IMAGE b_menu;
IMAGE b_pause;

Model currentModel = single_Model;

Difficulty currentDifficulty = simple_Difficulty;

Direction direction_1 = LEFT;

void initMap();
void paintSnake(int, int, int, int);
void paintMap();
Snake* initSnake(int);
void updateMap();
void move();
Direction listenDirection(int);
void createFood(int, int, int, int);
void paintCell(int, int, int, int, int x, int y);
void growUp();
Bool eatFood();
void createPoiWeeds(int, int, int, int);
void reduceCell();
Bool eatPoiWeed();
void createLandmine(int, int, int, int);
void reduceHalf();
Bool eatLandmine();
Bool isGameOver();
void gameStart(Bool);
void loadAllImages();
Operation startMouseListening();
IMAGE* getSnakeImage(Snake*, Player);
void paintModelSetting();
void paintDifficultySetting();
void paintScore(int score);
void writeFile();
Bool readFile();
void paintGameOver(Bool);
void initParameter();
void paintButtons();
void doContinueGame(Bool);


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
	goto start;
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

void doContinueGame(Bool doContinue) {
	if (doContinue)
	{
		if (readFile()) {

		}
		else
		{
			initMap();
			head_1 = initSnake(SNAKE_INIT_LENGTH);
		}
	}
	else
	{
		initMap();
		head_1 = initSnake(SNAKE_INIT_LENGTH);
	}
}

void paintButtons() {
	putimage(635, 480, &b_pause);
	putimage(635, 550, &b_menu);
}

void initParameter() {
	paintPoiWeeds = false;
	hasFood = false;
	hasPoiWeeds = false;
	hasLandmine = false;
	keyUsed = false;
	doContinue = false;
	isOver = false;
	snakeLength = SNAKE_INIT_LENGTH;
	poi_weedOff = false;
}

void paintGameOver(Bool isWin) {
	IMAGE *img;
	if (isWin)
	{
		img = &singleWin;
	}
	else {
		img = &singleLoss;
	}
	putimage(0, 200, img);
	Sleep(5000);
}

Bool readFile() {
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
		
		direction_1 = (Direction)atoi(cJSON_Print(cJSON_GetObjectItem(memory, "direction_1")));
		poi_weedOff = (Bool)atoi(cJSON_Print(cJSON_GetObjectItem(poi_weed, "poi_weedOff")));

		int temp_x, temp_y;
		int snakeSize_1 = cJSON_GetArraySize(snake_1_x_arr);
		head_1 = initSnake(snakeSize_1);
		Snake *snakeTemp = head_1;
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

		currentModel = model;
		currentDifficulty = difficulty;
		snakeLength = snakeSize_1;
		if (memory == NULL)
		{
			return false;
		}
		cJSON_Delete(memory);
		return true;
	}
	else
	{
		return false;
	}
}

void writeFile() {
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
	cJSON_AddNumberToObject(poi_weed, "poi_weedOff", poi_weedOff);
	cJSON_AddNumberToObject(memory, "direction_1", direction_1);
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

	Snake *temp = head_1;
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

	cJSON_Delete(memory);
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
	if (snakeLength == 1)
	{
		switch (direction_1) {
		case UP:
			return &snakeArr[0][0];
		case RIGHT:
			return &snakeArr[0][1];
		case DOWN:
			return &snakeArr[0][2];
		case LEFT:
			return &snakeArr[0][3];
		}
	}

	int px, py;
	int x, y;
	int nx, ny;
	if (snakeBody == head_1)
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
	else if (snakeBody == tail_1)
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
					scoreThresholdValue = SCORE_SIMPLE;
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
					scoreThresholdValue = SCORE_DIFFICULT;
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
					scoreThresholdValue = SCORE_IMPOSSIBLE;
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
	loadimage(&singleLoss, "images/background/single_loss.png");
	loadimage(&singleWin, "images/background/single_win.png");
	loadimage(&b_menu, "images/buttons/b_menu.png");
	loadimage(&b_pause, "images/buttons/b_pause.png");

	//up right down left
	//snake_1 init
	//init head_1
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
	//init tail_1
	loadimage(&snake_1[4][0], "images/snake/tail_up_1.png");
	loadimage(&snake_1[4][1], "images/snake/tail_right_1.png");
	loadimage(&snake_1[4][2], "images/snake/tail_down_1.png");
	loadimage(&snake_1[4][3], "images/snake/tail_left_1.png");
	
	//snake_2 init
	//init head_1
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
	//init tail_1
	loadimage(&snake_2[4][0], "images/snake/tail_up_2.png");
	loadimage(&snake_2[4][1], "images/snake/tail_right_2.png");
	loadimage(&snake_2[4][2], "images/snake/tail_down_2.png");
	loadimage(&snake_2[4][3], "images/snake/tail_left_2.png");
}

void gameStart(Bool doContinue) {
	int score;
	int poiWeedsTime = 0;
	Direction d_temp;
	int keyASC;
	int poi_times = 0;
	Bool start = false;
	putimage(0, 200, &readyImage);
	Sleep(2000);
	doContinueGame(doContinue);
	cleardevice();
	while (true)
	{
		
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
		MOUSEMSG msg;
		int x, y;
		while (MouseHit())
		{
			msg = GetMouseMsg();
			x = msg.x;
			y = msg.y;
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (x > 635 && x < 785 && y>480 && y < 530)
				{
					isPause = !isPause;
				}
				else if (x > 635 && x < 785 && y > 550 && y < 600)
				{
					goto start;
				}
			}
		}
		while (_kbhit()) {
			keyASC = _getch();
			switch (keyASC)
			{
			case ' ':
				isPause = !isPause;
			}
			if (!keyUsed)
			{
				d_temp = listenDirection(keyASC);
				if (d_temp != ERR)
				{
					direction_1 = d_temp;
					keyUsed = true;
				}
			}
		}
		if (FLASH_FREQ != poi_times)
		{
			if (!isPause)
			{
				poi_times++;
				if (poiWeedsTime <= POI_WEEDS_TIME)
				{
					poiWeedsTime += (int)(sleepTime / FLASH_FREQ);
				}
				else
				{
					poiWeedsTime = 0;
					hasPoiWeeds = false;
					poi_weedOff = !poi_weedOff;
				}
			}
			continue;
		}
		else
		{
			poi_times = 0;
			writeFile();
		}
		if (eatFood())
		{
			growUp();
			snakeLength++;
		}
		else if (eatPoiWeed())
		{
			if (snakeLength <= 1)
			{
				isOver = true;
			}
			else {
				reduceCell();
				snakeLength--;
				move();
			}
		}
		else if (eatLandmine()) {
			if (snakeLength <= 1)
			{
				isOver = true;
			}
			else {
				reduceHalf();
				snakeLength = snakeLength - snakeLength / 2;
				move();
			}
		}
		else
		{
			move();
		}
		if (isGameOver() || isOver)
		{
			isWin = false;
			break;
		}
		else if (score >= scoreThresholdValue)
		{
			isWin = true;
			break;
		}
	}
	paintGameOver(isWin);
start:;
}

Bool isGameOver() {
	int x = head_1->point.x;
	int y = head_1->point.y;
	if (x < 0 || x > MAP_X - 1 || y < 0 || y > MAP_Y - 1 || map[x][y] == BODY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool eatLandmine() {
	switch (direction_1)
	{
	case UP:
		for (int i = 0; i < numOfLandmine; i++)
		{
			if (head_1->point.x == landmine_x[i] && head_1->point.y == landmine_y[i] + 1)
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
			if (head_1->point.x == landmine_x[i] && head_1->point.y == landmine_y[i] - 1)
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
			if (head_1->point.x == landmine_x[i] + 1 && head_1->point.y == landmine_y[i])
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
			if (head_1->point.x == landmine_x[i] - 1 && head_1->point.y == landmine_y[i])
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
		tail_1 = tail_1->previous;
		//free(tail_1->next);
	}
	tail_1->isTail = true;
	tail_1->next = NULL;
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

Bool eatPoiWeed() {
	switch (direction_1)
	{
	case UP:
		for (int i = 0; i < numOfPoiWeeds; i++)
		{
			if (head_1->point.x == poiWeeds_x[i] && head_1->point.y == poiWeeds_y[i] + 1)
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
			if (head_1->point.x == poiWeeds_x[i] && head_1->point.y == poiWeeds_y[i] - 1)
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
			if (head_1->point.x == poiWeeds_x[i] + 1 && head_1->point.y == poiWeeds_y[i])
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
			if (head_1->point.x == poiWeeds_x[i] - 1 && head_1->point.y == poiWeeds_y[i])
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
	tail_1 = tail_1->previous;
	tail_1->isTail = true;
	//free(tail_1->next);
	tail_1->next = NULL;
}

void createPoiWeeds(int x0, int y0, int dx, int dy) {
	if (poi_weedOff) {
		return;
	}
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

Bool eatFood() {
	switch (direction_1)
	{
	case UP:
		for (int i = 0; i < numOfFood; i++)
		{
			if (head_1->point.x == food_x[i] && head_1->point.y == food_y[i] + 1)
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
			if (head_1->point.x == food_x[i] && head_1->point.y == food_y[i] - 1)
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
			if (head_1->point.x == food_x[i] + 1 && head_1->point.y == food_y[i])
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
			if (head_1->point.x == food_x[i] - 1 && head_1->point.y == food_y[i])
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

void growUp() {
	Snake* newCell = (Snake*)malloc(sizeof(Snake));
	newCell->next = head_1;
	head_1->previous = newCell;
	head_1->isHead = false;
	newCell->isHead = true;
	head_1 = newCell;
	switch (direction_1)
	{
	case UP:
		head_1->point.x = head_1->next->point.x;
		head_1->point.y = head_1->next->point.y - 1;
		break;
	case DOWN:
		head_1->point.x = head_1->next->point.x;
		head_1->point.y = head_1->next->point.y + 1;
		break;
	case LEFT:
		head_1->point.x = head_1->next->point.x - 1;
		head_1->point.y = head_1->next->point.y;
		break;
	case RIGHT:
		head_1->point.x = head_1->next->point.x + 1;
		head_1->point.y = head_1->next->point.y;
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

Direction listenDirection(int key) {
	switch (key)
	{
	case KEY_UP:
		if (direction_1 == DOWN)
		{
			return ERR;
		}
		return UP;
	case KEY_DOWN:
		if (direction_1 == UP)
		{
			return ERR;
		}
		return DOWN;
	case KEY_RIGHT:
		if (direction_1 == LEFT)
		{
			return ERR;
		}
		return RIGHT;
	case KEY_LEFT:
		if (direction_1 == RIGHT)
		{
			return ERR;
		}
		return LEFT;
	default:
		return ERR;
	}
}

void move() {
	Snake *temp;
	switch (direction_1)
	{
	case LEFT:
		tail_1->point.x = head_1->point.x - 1;
		tail_1->point.y = head_1->point.y;
		break;
	case RIGHT:
		tail_1->point.x = head_1->point.x + 1;
		tail_1->point.y = head_1->point.y;
		break;
	case UP:
		tail_1->point.x = head_1->point.x;
		tail_1->point.y = head_1->point.y - 1;
		break;
	case DOWN:
		tail_1->point.x = head_1->point.x;
		tail_1->point.y = head_1->point.y + 1;
		break;
	default:
		break;
	}
	if (tail_1 == head_1)
	{
		head_1->next = NULL;
		head_1->previous = NULL;
	}
	else {
		temp = head_1;
		head_1 = tail_1;
		tail_1 = tail_1->previous;
		temp->previous = head_1;
		head_1->next = temp;
		tail_1->next = NULL;
		head_1->previous = NULL;
	}
	
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
	paintButtons();
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
	snakeCell = head_1;
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
	Snake*head_1;
	head_1 = (Snake*)malloc(sizeof(Snake)*length);
	tail_1 = &head_1[length - 1];
	for (int i = 0; i < length; i++)
	{
		if (i < length - 1)
		{
			head_1[i].next = &head_1[i + 1];
			head_1[i + 1].previous = &head_1[i];
		}
		head_1[i].point.x = MAP_X / 2 + i;
		head_1[i].point.y = MAP_Y / 2;
	}
	head_1->isHead = true;
	tail_1->isTail = true;
	tail_1->next = NULL;
	head_1->previous = NULL;
	return head_1;
}

void updateMap() {
	initMap();
	Snake* temp = head_1;
	int x, y;
	while (temp != NULL) {
		if (temp == head_1)
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
