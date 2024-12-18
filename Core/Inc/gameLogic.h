/*
 * gameLogic.h
 *
 *  Created on: Dec 7, 2024
 *      Author: marti
 */

#ifndef INC_GAMELOGIC_H_
#define INC_GAMELOGIC_H_

#include <stdint.h>

// Definitions
#define MAX_POLYGONS 100
#define MAX_VERTICES 8

// Structures
typedef struct {
    int16_t *vertices; // Dynamically allocated array of vertices (x0, y0, x1, y1, ...)
    int16_t numVertices; // Number of vertices
    int16_t color;       // Polygon color (e.g., RGB as an int)
} Polygon;

typedef struct {
    float x, y, z;      // Player position
    float rotation;  // Horizontal rotation angle (in degrees)
    int16_t health;      // Player health
    int16_t armor;       // Player armor
    int16_t ammo;        // Player ammo
} Player;

// Declare global variables as extern
extern Polygon polygons[MAX_POLYGONS];
extern int polygonCount;

// Function prototypes
float randomFloat(float min, float max);
void initPolygon(Polygon *polygon, int16_t *vertices, int16_t numVertices, int16_t color);
void addPolygon(int16_t *vertices, int16_t numVertices, int16_t color);
void removePolygon(int16_t index);
void initPolygons();
void freePolygons();
void initPlayer();
void initEnemy();
void movePlayer(Player *player, float stepX, float stepY, uint8_t polar);
void rotatePlayer(Player *player, float angle);
void drawAmmo();
void drawScore(int16_t score);
void drawHealth();
void drawArmor();
void gameLoop(int16_t difficulity);
void startNewGame(int16_t difficility);

#endif /* INC_GAMELOGIC_H_ */
