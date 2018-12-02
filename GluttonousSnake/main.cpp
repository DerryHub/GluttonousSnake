#include<graphics.h>
#include<conio.h>
#include<stdlib.h>
#include<time.h>

#define true 1
#define false 0
#define MAP_X 30
#define MAP_Y 30
#define DX 20
#define DY 20
#define SNAKE_INIT_LEAGTH 5
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
#define SLEEP_TIME 100
#define NUM_OF_POI_WEEDS 5
#define NUM_OF_LANDMINE 5
#define NUM_OF_FOODS 5
#define FLASH_FREQ 3
#define POI_WEEDS_TIME 1000
#define KINDS_OF_FOODS 12

typedef enum {
	startGame,
	nothing
}Operation;
typedef enum {
	UP,
	DOWN,
	RIGHT,
	LEFT,
	ERR
}Direction;
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

Bool paintPoiWeeds = false;
char map[MAP_Y][MAP_X];
Snake *head, *tail;
Bool hasFood = false;
int food_x[NUM_OF_FOODS];
int food_y[NUM_OF_FOODS];
Bool hasPoiWeeds = false;
int poiWeeds_x[NUM_OF_POI_WEEDS];
int poiWeeds_y[NUM_OF_POI_WEEDS];
Bool hasLandmine = false;
int landmine_x[NUM_OF_LANDMINE];
int landMine_y[NUM_OF_LANDMINE];
Bool keyUsed = false;
int snakeLength = SNAKE_INIT_LEAGTH;
IMAGE startBackground;
IMAGE readyImage;
IMAGE gameBackground;
IMAGE b_red;
IMAGE b_blue;
IMAGE b_green;
IMAGE foods[12];
IMAGE poiWeed;
IMAGE landmine;
int foodIndex[NUM_OF_FOODS];

void initMap();
void paintSnake(int, int, int, int);
void paintMap();
Snake* initSnake();
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
void gameStart();
void loadAllImages();
Operation startMouseListening();
void ready();

int main() {
	srand(time(0));
	initgraph(800, 620);
	loadAllImages();
	putimage(0, 0, &startBackground);
	putimage(30, 560, &b_green);
	putimage(300, 560, &b_blue);
	putimage(570, 560, &b_red);
	Operation op = startMouseListening();
	switch (op) {
	case startGame:
		goto startGame;
	}
	startGame:
	gameStart();
	system("pause");
	return 0;
}

void ready() {

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
			}
		}
	}
}

void loadAllImages() {
	loadimage(&startBackground, "images/background/background.jpg");
	loadimage(&readyImage, "images/background/ready.png");
	loadimage(&gameBackground, "images/background/gameBackground.png");
	loadimage(&b_red, "images/buttons/b_red.png", 200, 40);
	loadimage(&b_blue, "images/buttons/b_blue.png", 200, 40);
	loadimage(&b_green, "images/buttons/b_green.png", 200, 40);
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
}

void gameStart() {
	int poiWeedsTime = 0;
	Direction direction = LEFT, d_temp;
	int keyASC;
	int poi_times = 0;
	Bool start = false;
	putimage(0, 200, &readyImage);
	Sleep(2000);
	initMap();
	head = initSnake();
	cleardevice();
	while (true)
	{
		if (poiWeedsTime <= POI_WEEDS_TIME)
		{
			poiWeedsTime += SLEEP_TIME / FLASH_FREQ;
		}
		paintPoiWeeds = !paintPoiWeeds;
		keyUsed = false;
		cleardevice();
		putimage(0, 0, &gameBackground);
		updateMap();
		paintMap();
		if (!start)
		{
			putimage(0, 200, &readyImage);
			Sleep(1000);
			start = true;
		}
		Sleep(SLEEP_TIME / FLASH_FREQ);
		if (FLASH_FREQ != poi_times++)
		{
			continue;
		}
		else
		{
			poi_times = 0;
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
			reduceCell();
			snakeLength--;
			move(direction);
		}
		else if (eatLandmine(direction)) {
			reduceHalf();
			snakeLength = snakeLength - snakeLength / 2;
			move(direction);
		}
		else
		{
			move(direction);
		}
		if (isGameOver())
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
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landMine_y[i] + 1)
			{
				landmine_x[i] = -2;
				landMine_y[i] = -2;
				return true;
			}
		}
		return false;
	case DOWN:
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			if (head->point.x == landmine_x[i] && head->point.y == landMine_y[i] - 1)
			{
				landmine_x[i] = -2;
				landMine_y[i] = -2;
				return true;
			}
		}
		return false;
	case LEFT:
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			if (head->point.x == landmine_x[i] + 1 && head->point.y == landMine_y[i])
			{
				landmine_x[i] = -2;
				landMine_y[i] = -2;
				return true;
			}
		}
		return false;
	case RIGHT:
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			if (head->point.x == landmine_x[i] - 1 && head->point.y == landMine_y[i])
			{
				landmine_x[i] = -2;
				landMine_y[i] = -2;
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
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			landmine_x[i] = rand() % 30;
			landMine_y[i] = rand() % 30;
			while (map[landmine_x[i]][landMine_y[i]] != 0) {
				landmine_x[i] = rand() % 30;
				landMine_y[i] = rand() % 30;
			}
			map[landmine_x[i]][landMine_y[i]] = LANDMINE;
			putimage(x0 + landmine_x[i] * dx, y0 + landMine_y[i] * dy, &landmine);
			//paintCell(x0, y0, dx, dy, landmine_x[i], landMine_y[i]);
		}
		hasLandmine = true;
	}
	else
	{
		setfillcolor(BLACK);
		for (int i = 0; i < NUM_OF_LANDMINE; i++)
		{
			if (landmine_x[i] != -2 && landMine_y[i] != -2)
			{
				map[landmine_x[i]][landMine_y[i]] = LANDMINE;
				putimage(x0 + landmine_x[i] * dx, y0 + landMine_y[i] * dy, &landmine);
				//paintCell(x0, y0, dx, dy, landmine_x[i], landMine_y[i]);
			}
		}
	}
}

Bool eatPoiWeed(Direction direction) {
	switch (direction)
	{
	case UP:
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_POI_WEEDS; i++)
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
		for (int i = 0; i < NUM_OF_FOODS; i++)
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
		for (int i = 0; i < NUM_OF_FOODS; i++)
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
		for (int i = 0; i < NUM_OF_FOODS; i++)
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
		for (int i = 0; i < NUM_OF_FOODS; i++)
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
		for (int i = 0; i < NUM_OF_FOODS; i++) {
			food_x[i] = -2;
			food_y[i] = -2;
			foodIndex[i] = rand() % KINDS_OF_FOODS;
		}
		hasFood = true;
	}
	for (int i = 0; i < NUM_OF_FOODS; i++)
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
	for (int i = 0; i < MAP_Y; i++)
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
	}
}

Snake* initSnake() {
	Snake*head;
	head = (Snake*)malloc(sizeof(Snake)*SNAKE_INIT_LEAGTH);
	tail = &head[SNAKE_INIT_LEAGTH - 1];
	for (int i = 0; i < SNAKE_INIT_LEAGTH; i++)
	{
		if (i < SNAKE_INIT_LEAGTH - 1)
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
	for (int i = 0; i < MAP_Y; i++)
	{
		for (int j = 0; j < MAP_X; j++) {
			map[j][i] = NOTHING;
		}
	}
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
}
