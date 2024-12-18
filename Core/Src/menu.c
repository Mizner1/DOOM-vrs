#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ili9163.h"
#include "gameLogic.h"
#include "spi.h"

int16_t ammo = 10;
int16_t score = 10;
int16_t health = 100;
int16_t armor = 100;
char ammoText[16];
char scoreText[16];
char healthText[16];
char armorText[16];

void drawMenu() {
	lcdPutSSized("DOOM", 250, 0, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),8);
	lcdPutSSized("NEW GAME", 205, 64, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("OPTIONS", 205, 80, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("LOAD GAME", 205, 96, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("SAVE GAME", 205, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("READ THIS!", 205, 128, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("QUIT GAME", 205, 144, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);

	lcdPutS("AMMO", 285, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(ammoText, "%d", ammo);
	lcdPutS(ammoText, 280, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));

	lcdPutS("SCORE", 200, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(scoreText, "%d", score);
	lcdPutS(scoreText, 195, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));

	lcdPutS("HEALTH", 140, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(healthText, "%d%%", health);
	lcdPutS(healthText, 135, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));

	lcdPutS("ARMOR", 50, 230, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
	sprintf(armorText, "%d%%", armor);
	lcdPutS(armorText, 50, 222, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0));
}

void clearMenu() {
    lcdPutSSized("DOOM", 250, 0, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 8);
    lcdPutSSized("NEW GAME", 205, 64, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);
    lcdPutSSized("OPTIONS", 205, 80, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);
    lcdPutSSized("LOAD GAME", 205, 96, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);
    lcdPutSSized("SAVE GAME", 205, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);
    lcdPutSSized("READ THIS!", 205, 128, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);
    lcdPutSSized("QUIT GAME", 205, 144, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0), 2);

    /*lcdPutS("AMMO", 285, 230, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
    lcdPutS("", 280, 222, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));

    lcdPutS("SCORE", 200, 230, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
    lcdPutS("", 195, 222, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));

    lcdPutS("HEALTH", 140, 230, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
    lcdPutS("", 135, 222, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));

    lcdPutS("ARMOR", 50, 230, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));
    lcdPutS("", 50, 222, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0));*/

    lcdFilledRectangle(20, 220, 300, 240, decodeRgbValue(0, 0, 0));
}

void drawDifficulty() {
	lcdPutSSized("options", 240, 0, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),4);
	lcdPutSSized("EASY", 210, 80, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
	lcdPutSSized("MEDIUM", 210, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
	lcdPutSSized("HARD", 210, 144, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),3);
}

void clearDifficulty() {
	lcdPutSSized("options", 240, 0, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),4);
	lcdPutSSized("EASY", 210, 80, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);
	lcdPutSSized("MEDIUM", 210, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);
	lcdPutSSized("HARD", 210, 144, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),3);
}

drawReadme() {
	lcdPutSSized("About game", 300, 40, decodeRgbValue(255, 255, 255), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("STM adaptation of DOOM", 310, 80, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),1);
	lcdPutSSized("press right button to escape", 310, 220, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),1);
}

clearReadme() {
	lcdPutSSized("About game", 300, 40, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
	lcdPutSSized("STM adaptation of DOOM", 310, 80, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),1);
	lcdPutSSized("press right button to escape", 310, 220, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),1);
}

void menu(){
	int16_t difficulity = 0;		//v options sa bude dat menit

	clearMenu();
	drawMenu();
	LL_mDelay(3000);

/*
 	//tieto premenne som sem daval, aby sa do nich dalo ulozit ak budeme chciet save game
	int16_t ammo = 24;
	int16_t health = 78;
	int16_t armor = 0;
	char ammoText[16];
	char healthText[16];
	char armorText[16];
*/
//lowerMenu();

	int select = 1;			//tymto sa vybera iba v main menu

	while(1)	//main menu cycle
	{

		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET)	//choose lower option
		{
			if(select == 0)
				select = 5;
			else if(select != 0)
				select--;
		}

		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET)	//choose option above
		{
			if(select == 5)
				select = 0;
			else if(select != 5)
				select++;
		}

		if(select == 0)
		{
			lcdPutSSized("NEW GAME", 205, 64, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("NEW GAME", 205, 64, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}
		if(select == 1)
		{
			lcdPutSSized("OPTIONS", 205, 80, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("OPTIONS", 205, 80, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}
		if(select == 2)
		{
			lcdPutSSized("LOAD GAME", 205, 96, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("LOAD GAME", 205, 96, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}
		if(select == 3)
		{
			lcdPutSSized("SAVE GAME", 205, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("SAVE GAME", 205, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}
		if(select == 4)
		{
			lcdPutSSized("READ THIS!", 205, 128, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("READ THIS!", 205, 128, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}
		if(select == 5)
		{
			lcdPutSSized("QUIT GAME", 205, 144, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
			lcdPutSSized("QUIT GAME", 205, 144, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
		}

		//zacat novu hru
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET && select == 0) {
			clearMenu();
			startNewGame(difficulity);
			drawMenu();
		}


		// odkaz pre Mira: tak ako robil Matej menu, tak skus podobne urobit options a aj readme.
		// V options sa bude prepinat obtiaznost a ked zacnes hru, tak sa hodnota obtiaznosti
		// priradi hre. Obtiaznost => easy, medium, hard (0 - 1 - 2)

		// takto nejako si predstavujem options, skus to rozvinut
		//enter options menu
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET && select == 1){
			clearMenu();
			drawDifficulty();
			while(1){

				// vyber difficulity podla stlacenia tlacitok
				/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET)	//choose option above
				{
					if(difficulity == 0)
						difficulity = 2;
					else if(difficulity != 0)
						difficulity--;
				}

				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET)	//choose option below
				{
					if(difficulity == 2)
						difficulity = 0;
					else if(difficulity != 2)
						difficulity++;
				}*/


				if(difficulity == 0)
				{
					lcdPutSSized("EASY", 210, 80, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
					LL_mDelay(500);
					lcdPutSSized("EASY", 210, 80, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
				}

				if(difficulity == 1)
				{
					lcdPutSSized("MEDIUM", 210, 112, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
					LL_mDelay(500);
					lcdPutSSized("MEDIUM", 210, 112, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
				}

				if(difficulity == 2)
				{
					lcdPutSSized("HARD", 210, 144, decodeRgbValue(0, 0, 0), decodeRgbValue(0, 0, 0),2);
					LL_mDelay(500);
					lcdPutSSized("HARD", 210, 144, decodeRgbValue(31, 31, 31), decodeRgbValue(0, 0, 0),2);
				}

				//ukoncit options menu potvrdenim vyberu, snad pin3 je spravny pin
				/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET){
					clearDifficulty();
					drawMenu();
					break;
				}*/
			}
		}

		//ked kliknes na "read this"
		/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET && select == 4) {
			clearMenu();
			drawReadme();
			while(1) {
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET){
					clearDifficulty();
					drawMenu();
					break;
				}
			}
			clearReadme();
			drawMenu();
		}*/

		//ked kliknes na "quit", tak sa ukonci cely program, potom treba restartovat STMko
		/*if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_RESET && select == 5) {
			clearMenu();
			break;
		}*/

	}
	lcdPutSSized("the game has been quit.", 300, 120, decodeRgbValue(100, 100, 100), decodeRgbValue(0, 0, 0),1);
}

/*
 * po prejdeni z hlavneho menu na options menu mi tam vypise este save game read this quit neviem preco
 * v options sekcii mi nejde manualne menit obtiaznost stale to select sa rovna len jednotke a neviem preco
*/

