

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
    player.armor = 100;     // Starting armor
    player.ammo = 10;       // Starting ammo
}

// Function to initialize the player
void initEnemy() {
	enemy.x = randomFloat(0, 1000); // Initial X position
	enemy.y = randomFloat(0, 1000); // Initial Y position
	enemy.z = 50;         // Initial Z position
	enemy.rotation = 0.5; // Initial rotation angle
	enemy.health = 100;    // Starting health
	enemy.armor = 0;     // Starting armor
	enemy.ammo = 10;       // Starting ammo
}

// pohyb hraca na osi X a Y
// este to nefunguje uplne na osi Y
void movePlayer(Player *player, float stepX, float stepY, uint8_t polar) {

	float moveX = 0;
	float moveY = 0;

	//treba doladit znamienka a sin/cos funkcie na moveX a moveY
	if (polar){	//ak sa hybeme vzhladom na natocenie hraca
		moveX = stepX*cos(player->rotation) + stepY*sin(player->rotation);
		moveY = stepX*sin(player->rotation) + (-1)*stepY*cos(player->rotation);
	}
	else{		//ak chceme hraca premiestnit bez ohladu jeho na rotaciu
		moveX = stepX;
		moveY = stepY;
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

void drawAmmo() {
	char ammoText[16];
	lcdPutS("AMMO", 285, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(ammoText, "%d ", player.ammo);
	lcdPutS(ammoText, 280, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
}

void drawScore(int16_t score) {
	char scoreText[16];
	lcdPutS("SCORE", 200, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(scoreText, "%d ", score);
	lcdPutS(scoreText, 195, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
}

void drawHealth() {
	char healthText[16];
	lcdPutS("HEALTH", 140, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(healthText, "%d ", player.health);
	lcdPutS(healthText, 135, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
}

void drawArmor() {
	char armorText[16];
	lcdPutS("ARMOR", 50, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(armorText, "%d ", player.armor);
	lcdPutS(armorText, 50, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
}

void startNewGame(int16_t difficulity){
	initPlayer();
	initEnemy();
	gameLoop(difficulity);
}

void gameLoop(int16_t difficulity){
	// odkaz pre Mateja: sfunkcni tie piny a potom prerob gameLoop aby sa tu vykonavala hra
	// pre nepriatelov mozes pouzit strukturu hraca. Ich vykreslovanie potom ja dokoncim

	int8_t step = 5;	// kazdych 10 pixelov bude bodka, samozrejme 5 je lepsie, ale viac laguje

	//premenne potrebne na zastavenie hraca
	uint8_t stay = 0;
	uint8_t xm = 0;
	uint8_t ym = 0;
	uint8_t predX = 0;
	uint8_t predY = 0;

	float vzX = 0;			//vzdialenost medzi hracom a nepriatelom
	float vzY = 0;
	float vz = 0;

	// premenne potrebne k mechanike strelby
	uint8_t loadedState = 1;		//flag potrebny pocas nabijania zbrane
	uint16_t loadedAmmo = 0;			//premenna potrebna pocas nabijania
	uint16_t loadSpeed = 3;			//rychlost nabijania
	uint16_t magazineCapacity = 10;	//kapacita zasobnika
	int16_t bulletSpeed = 10;				//rychlost strely
	int16_t bulletDamage = 120;				//damage strely (1 hit kill)
	float bulletX = 0;			//suradnice strely
	float bulletY = 0;
	float bulletAngle = 0;			//smer strely

	int16_t kills = 0;
	int16_t kills2win = 0; // podmienka na skoncenie levelu, zavisi od obtiaznosti
	int16_t enemySpeed = 0;
	int16_t enemyDamage = 0;
	switch(difficulity){	//podla urcenej obtiaznosti sa nastavia parametre hry
	case 0:
		enemySpeed = 10;
		enemyDamage = 5;
		kills2win = 1;
		break;
	case 1:
		enemySpeed = 20;
		enemyDamage = 10;
		kills2win = 5;
		break;
	case 2:
		enemySpeed = 40;
		enemyDamage = 20;
		kills2win = 10;
		break;
	}

	lcdRectangle(20, 220, 300, 250, decodeRgbValue(255, 255, 255)); //dolny status bar
	lcdCircle(160,230,8,decodeRgbValue(255, 255, 255)); //akysi kruh, v povodnej doom je tam hlava hraca

	drawAmmo();
	drawHealth();
	drawArmor();
	drawScore(kills);




	while(kills < kills2win){		// hlavny cyklus hry

		//debugujem vystup z joysticku
		/*char joyX[16];
		char joyY[16];
		lcdPutS("joyX output", 280, 50, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
		sprintf(joyX, "%d ", HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) );
		lcdPutS(joyX, 200, 50, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
		lcdPutS("joyY output", 280, 60, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
		sprintf(joyY, "%d ", HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) );
		lcdPutS(joyY, 200, 60, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));

		lcdPutS("joyX output", 280, 50, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
		sprintf(joyX, "%d ", HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) );
		lcdPutS(joyX, 200, 50, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
		lcdPutS("joyY output", 280, 60, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
		sprintf(joyY, "%d ", HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) );
		lcdPutS(joyY, 200, 60, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));*/


		//ovladanie cez joystick (testujeme)
		// treba zistit v akom formate su vystupy z jousticku a ake maju hodnoty
		// domnievam za, ze je to od 0 do nieco, asi 255., tiez vraj 0-553-1000
		/*predX = xm;
		predY = ym;
		xm = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
		ym = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		if((xm!=predX)||(ym!=predY)) {
			stay = 1;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) == GPIO_PIN_RESET){
			stay = 0;
		}*/


		/*if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_10) == GPIO_PIN_SET){
			stay = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_10) == GPIO_PIN_RESET){
			stay = 1;
		}*/

		//ovladan ie cez tlacitka
		/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET){
			movePlayer(&player, 0, +50, 1);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET){
			movePlayer(&player, 0, -50, 1);
		}*/


		if(!stay) {
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == 1){
				movePlayer(&player, 50, 0, 1);
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == 0){
				movePlayer(&player, -50, 0, 1);
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1){
				movePlayer(&player, 0, 50, 1);
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0){
				movePlayer(&player, 0, -50, 1);
			}
		}




		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_RESET){
			rotatePlayer(&player, +3.1416/10);
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET){
			rotatePlayer(&player, -3.1416/10);
		}



		if(enemy.health <= 0) {		//respawnujeme nepriatela ked ho zabijeme
			kills++;
			drawScore(kills);
			initEnemy();
		}
		if(enemy.health > 0) {		//logika nepriatela kym je nazive

			vzX = enemy.x - player.x;
			vzY = enemy.y - player.y;
			vz = sqrt(vzX*vzX + vzY*vzX);
			if(vz>50){						// nahana hraca po mape
				if(vzX>0) {
					enemy.x -= enemySpeed;
				}
				if(vzX<0) {
					enemy.x += enemySpeed;
				}
				if(vzY>0) {
					enemy.y -= enemySpeed;
				}
				if(vzY<0) {
					enemy.y += enemySpeed;
				}
			}
			else if(vz<50) {				//melee damage near player
				if(player.armor>0) {
					player.armor -= enemyDamage;
					drawArmor();
				}
				else{
					player.health -= enemyDamage;
					drawHealth();
				}
			}

		}







		//nakreslit nepriatela
		if(enemy.health > 0) {
			int16_t enemyCoords[3] = {(int16_t)(enemy.x), (int16_t)(enemy.y), (int16_t)(enemy.z)};
			float_t playerCoords[3] = {player.x, player.y, player.z};
			float twoDCoords[2];
			threeDto2D(&enemyCoords, playerCoords, player.rotation, twoDCoords);
			int16_t enX = (int16_t)(twoDCoords[0]);
			int16_t enY = (int16_t)(twoDCoords[1]);
			int16_t rad = (int16_t)(5000/vz);
			lcdCircle(enX, enY, rad, decodeRgbValue(255, 0, 0));
		}

		// vykreslovanie mapy
		for (int i = 0; i < polygonCount; i++) {	//nakresli mapu so stenami
			lcd3DPolyline(polygons[i].vertices, polygons[i].numVertices, polygons[i].color, player.x, player.y, player.z, player.rotation, 1, step);
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








		//mechanika strelby
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET){
			if(player.ammo>0) {			//ak mame naboje
				//zobrazit vystrel
				lcdFilledRectangle(130, 195, 150, 215, decodeRgbValue(255, 255, 0));
				lcdLine(140, 195, 160, 120, decodeRgbValue(255, 255, 0));

				lcdFilledRectangle(130, 195, 150, 215, decodeRgbValue(0, 0, 0));
				lcdLine(140, 195, 160, 120, decodeRgbValue(0, 0, 0));

				player.ammo--;
				drawAmmo();

				float enemyDist = 0;
				bulletX = player.x;
				bulletY = player.y;
				bulletAngle = player.rotation;

				//samotny let strely
				while( (bulletX<boundX)&&(bulletX>0)&&(bulletY<boundY)&&(bulletY>0) ) {
					bulletX += bulletSpeed*cos(bulletAngle) ;
					bulletX += bulletSpeed*sin(bulletAngle) ;
					enemyDist = sqrt((enemy.x-bulletX)*(enemy.x-bulletX)+(enemy.y-bulletY)*(enemy.y-bulletY));
					if(enemyDist<200) {			//ak zasiahne nepriatela
						enemy.health -= bulletDamage;
						break;
					}
				}
				if(player.ammo<=0) {	//ked dojdu naboje tak sa prepnu stavy
					loadedState = 0;
					loadedAmmo = 0;
				}
			}
		}
		if(player.ammo<=0) {	//ked dojdu naboje, tak reload
			if(!loadedState) {
				loadedAmmo += loadSpeed;
				if(loadedAmmo>=magazineCapacity) {
					loadedState = 1;
					player.ammo = magazineCapacity;
					drawAmmo();
				}
			}
		}

		//vymazat nepriatela
		int16_t enemyCoords[3] = {(int16_t)(enemy.x), (int16_t)(enemy.y), (int16_t)(enemy.z)};
		float_t playerCoords[3] = {player.x, player.y, player.z};
		float twoDCoords[2];
		threeDto2D(&enemyCoords, playerCoords, player.rotation, twoDCoords);
		int16_t enX = (int16_t)(twoDCoords[0]);
		int16_t enY = (int16_t)(twoDCoords[1]);
		int16_t rad = (int16_t)(5000/vz);
		lcdCircle(enX, enY, rad, decodeRgbValue(0, 0, 0));

		for (int i = 0; i < polygonCount; i++) {	//vymaze mapu so stenami
			lcd3DPolyline(polygons[i].vertices, polygons[i].numVertices, decodeRgbValue(0, 0, 0), player.x, player.y, player.z, player.rotation, 1, step);
		}

		//ukoncit level predcasne a vstupit do main menu
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET){
			break;
		}

		if (player.health<=0) {
			//deathScreen()		//treba vyrobit funkciu, ktora zobrazi GAME OVER
			lcdPutSSized("GAME OVER", 230, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
			LL_mDelay(1000);
			lcdPutSSized("GAME OVER", 230, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);
			break;				//ukonci sa hra a dostaneme sa do main menu

		}

	}

	if((player.health>0)&&(kills==kills2win)) {
		//winScreen();				//treba vyrobit funkciu, ktroa zobrazi LEVEL COMPLETE
		lcdPutSSized("level complete", 270, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
		LL_mDelay(1000);
		lcdPutSSized("level complete", 270, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);

	}
	if((player.health>0)&&(kills<kills2win)) {
		//winScreen();				//treba vyrobit funkciu, ktroa zobrazi LEVEL COMPLETE
		lcdPutSSized("ending level", 270, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
		LL_mDelay(1000);
		lcdPutSSized("ending level", 270, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);

	}
	//potom sa vratime do menu...
}



