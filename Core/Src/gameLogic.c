

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "ili9163.h"
#include "gameLogic.h"
#include "spi.h"
#include "menu.h"


// Define global variables
Polygon polygons[MAX_POLYGONS];
int polygonCount = 0;
Player player;
Player enemy;
int boundX = 1000;
int boundY = 1000;

//RNG function
float randomFloat(float min, float max) {
    // Seed the random number generator once (use the system time)
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL)); // Seed the RNG using time
        seeded = 1;
    }
    // Generate a random float value between 0 and 1, then scale it to [min, max]
    float scale = (float)rand() / (float)(RAND_MAX); // [0, 1]
    return min + scale * (max - min);               // Scale to [min, max]
}

// Function to initialize a polygon
void initPolygon(Polygon *polygon, int16_t *vertices, int16_t numVertices, int16_t color) {
    polygon->vertices = (int16_t *)malloc(numVertices * 3 * sizeof(int16_t));
    if (polygon->vertices == NULL) {
        printf("Error allocating memory for polygon vertices.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numVertices * 3; i++) {
        polygon->vertices[i] = vertices[i];
    }
    polygon->numVertices = numVertices;
    polygon->color = color;
}

// Function to add a new polygon to the polygons array
void addPolygon(int16_t *vertices, int16_t numVertices, int16_t color) {
    if (polygonCount >= MAX_POLYGONS) {
        printf("Cannot add more polygons. Maximum limit reached.\n");
        return;
    }
    initPolygon(&polygons[polygonCount], vertices, numVertices, color);
    polygonCount++;
}

// Function to remove a polygon from the polygons array
void removePolygon(int16_t index) {
    if (index < 0 || index >= polygonCount) {
        printf("Invalid polygon index.\n");
        return;
    }

    free(polygons[index].vertices);

    for (int i = index; i < polygonCount - 1; i++) {
        polygons[i] = polygons[i + 1];
    }

    polygonCount--;
}

// Function to initialize polygons from preset map
void initPolygons() {

	//arena s pouzitim polyline
	for(int8_t i=0; i<5; i++) {
		int16_t EastWall3[] = {200+200*i, 0, 0,   200*i, 0, 0,   200*i, 0, 100,   200+200*i, 0, 100};
		addPolygon(EastWall3, 4, decodeRgbValue(255, 255, 0)); // east
	}
	for(int8_t i=0; i<5; i++) {
		int16_t EastWall4[] = {1000, 200+200*i, 0,   1000, 200*i, 0,   1000, 200*i, 100,   1000, 200+200*i, 100};
		addPolygon(EastWall4, 4, decodeRgbValue(255, 255, 0)); // north
	}
	for(int8_t i=0; i<5; i++) {
		int16_t EastWall3[] = {800-200*i, 1000, 0,   1000-200*i, 1000, 0,   1000-200*i, 1000, 100,   800-200*i, 1000, 100};
		addPolygon(EastWall3, 4, decodeRgbValue(255, 255, 0)); // west
	}
	for(int8_t i=0; i<5; i++) {
		int16_t EastWall4[] = {0, 800-200*i, 0,   0, 1000-200*i, 0,   0, 1000-200*i, 100,   0, 800-200*i, 100};
		addPolygon(EastWall4, 4, decodeRgbValue(255, 255, 0)); // south
	}

}

// Function to free polygon memory
void freePolygons() {
    for (int i = 0; i < polygonCount; i++) {
        free(polygons[i].vertices);
    }
}

// Function to initialize the player
void initPlayer() {
	player.x = 100;        // Initial X position
    player.y = 100;        // Initial Y position
    player.z = 50;         // Initial Z position
    player.rotation = 0.5; // Initial rotation angle
    player.health = 100;    // Starting health
    player.ammo = 50;       // Starting ammo
}

// Function to initialize the player
void initEnemy() {
	enemy.x = randomFloat(0, 1000); // Initial X position
	enemy.y = randomFloat(0, 1000); // Initial Y position
	enemy.z = 50;         // Initial Z position
	enemy.rotation = 0.5; // Initial rotation angle
	enemy.health = 100;    // Starting health
	enemy.ammo = 50;       // Starting ammo
}

// pohyb hraca na osi X a Y
// este to nefunguje uplne na osi Y
void movePlayer(Player *player, float stepX, float stepY, uint8_t polar) {

	float moveX = 0;
	float moveY = 0;

	//treba doladit znamienka a sin/cos funkcie na moveX a moveY
	if (polar){	//ak sa hybeme vzhladom na natocenie hraca
		float moveX = stepX*cos(player->rotation) + stepY*sin(player->rotation);
		float moveY = stepX*sin(player->rotation) + stepY*cos(player->rotation);
	}
	else{		//ak chceme hraca premiestnit bez ohladu jeho na rotaciu
		float moveX = stepX;
		float moveY = stepY;
	}

	//when hitting wall the player slides along the other axis
	if ((player->x + moveX < boundX)&&(player->x + moveX > 0)) {
		player->x += moveX;
	}
	if ((player->y + moveY < boundY)&&(player->y + moveY > 0)) {
		player->y += moveY;
	}
}

void rotatePlayer(Player *player, float angle) {
	player->rotation += angle;
	if (player->rotation > 3.1416) {
		player->rotation -= 6.2832;
	}
	if (player->rotation < -3.1416) {
		player->rotation += 6.2832;
	}
}

void startNewGame(int16_t difficulity){
	initPolygons(0);
	initPlayer(0);
	gameLoop(difficulity);
}

void gameLoop(int16_t difficulity){
	// odkaz pre Mateja: sfunkcni tie piny a potom prerob gameLoop aby sa tu vykonavala hra
	// pre nepriatelov mozes pouzit strukturu hraca. Ich vykreslovanie potom ja dokoncim

	int8_t step = 5;	// kazdych 10 pixelov bude bodka, samozrejme 5 je lepsie, ale viac laguje

	lcdPutS("test 3D rendera:", 220, 10, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	lcdRectangle(20, 225, 300, 250, decodeRgbValue(255, 255, 255)); //dolny status bar
	lcdCircle(160,232,5,decodeRgbValue(255, 255, 255)); //akysi kruh, v povodnej doom je tam hlava hraca

	int16_t kills2win = 1; // podmienka na skoncenie levelu, zavisi od obtiaznosti
	int16_t kills = 0;
	float vzX = 0;			//vzdialenost medzi hracom a nepriatelom
	float vzY = 0;
	float vz = 0;

	int16_t enemySpeed = 0;
	int16_t enemyDamage = 0;
	switch(difficulity){	//podla urcenej obtiaznosti sa nastavia parametre hry
	case 0:
		enemySpeed = 10;
		enemyDamage = 5;
		break;
	case 1:
		enemySpeed = 20;
		enemyDamage = 10;
		break;
	case 2:
		enemySpeed = 30;
		enemyDamage = 40;
		break;
	}

	while(kills < kills2win){


		// vykreslovanie mapy
		for (int i = 0; i < polygonCount; i++) {	//nakresli mapu so stenami
			lcd3DPolyline(polygons[i].vertices, polygons[i].numVertices, polygons[i].color, player.x, player.y, player.z, player.rotation, 1, step);
		}
		for (int i = 0; i < polygonCount; i++) {	//nakresli mapu so stenami
			lcd3DPolyline(polygons[i].vertices, polygons[i].numVertices, decodeRgbValue(0, 0, 0), player.x, player.y, player.z, player.rotation, 1, step);
		}

		//ovladanie cez tlacitka - FUNGUJE ale takto to nechceme
		/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET){
			movePlayer(&player, 50, 0, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET){
			movePlayer(&player, -50, 0, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET){
			rotatePlayer(&player, +3.1416/10);
		}*/



		//ovladanie cez joystick (testujeme)
		// treba zistit v akom formate su vystupy z jousticku a ake maju hodnoty
		// domnievam za, ze je to od 0 do nieco, asi 255., tiez vraj 0-553-1000
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) > 603){
			movePlayer(&player, 50, 0, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) < 503){
			movePlayer(&player, -50, 0, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) > 603){
			movePlayer(&player, 0, 50, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) < 503){
			movePlayer(&player, 0, -50, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET){
			rotatePlayer(&player, +3.1416/10);
		}



		//nejako takto si predstavujem menu pocas hry, ze stlacis tlacitko
		// a nasledne sa spusti cyklus, kde bude "in game" menu s moznostami
		// end game, resume, mozno aj quicksave(nech je vsetko na jednom mieste)
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) < 0){
			//premaz celu obrazovku nekou funkciou...
			menu();
			// pripadne samostatne menu do hry, ak bude treba...
			// gamemenu();
		}



		if(enemy.health == 0) {		//respawnujeme nepriatela ked ho zabijeme
			kills++;
			initEnemy();
		}
		if(enemy.health > 0) {		//logika nepriatela kym je nazive
			vzX = enemy->x - player->x;
			vzY = enemy->y - player->y;
			vz = sqrt(vzX^2 + vzY^2);
			if(vz>50){						// nahana hraca po mape
				if(vzX>0) {
					enemy->x -= enemySpeed;
				}
				if(vzX<0) {
					enemy->x += enemySpeed;
				}
				if(vzY>0) {
					enemy->y -= enemySpeed;
				}
				if(vzY<0) {
					enemy->y += enemySpeed;
				}
			}
			else if(vz<50) {				//melee damage near player
				player->health -= enemyDamage;
			}
		}

	}
}



