#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

/**
 * @file common_functions.h
 * @author Daniele
 * @brief Library containing functions used in both AI and interactive modes.
 * @date 06/02/2024
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define LEFT 'L'
#define RIGHT 'R'
#define TOP 'T'
#define BOTTOM 'B'
#define HORIZONTAL 'H'
#define VERTICAL 'V'
#define NOT_VALID_INT (-1)
#define NOT_VALID_CHAR '*'

/**
 * Data type for the generated domino tiles
 * @var left left side of the tile
 * @var right right side of the tile
 */
typedef struct
{
    int left;
    int right;
} dominoTile;

/**
 * Data type for a single side of the tile inserted in the game field
 * @var value value
 * @var side specifies the side of the tile: R(Right) or L(Left) || T(Top) or B(Bottom)
 * @var orientation specifies if the tile is horizontal or vertical: H(Horizontal) || V(Vertical)
 */
typedef struct
{
    int value;
    char side;
    char orientation;
} dominoTileSide;

/**
 * Data type containing all the info about the game field
 * @var **inGameDominoTiles pointer to the matrix where cells contain the sides of the tiles in play
 * @var score score
 * @var rows rows of the game board
 * @var cols columns of the game board
 */
typedef struct
{
    dominoTileSide **inGameDominoTiles;
    int score;
    int rows;
    int cols;
} gameSituation;

// Function prototypes
void generateTiles(int numTotalTiles, int numSpecials, dominoTile[]);
void printTiles(int numTiles, dominoTile[]);
void moveRight(gameSituation *, int);
dominoTileSide **createInGameDominoTiles(int, int);
void initializeMatrix(dominoTileSide **, int, int);
void freeInGameDominoTiles(dominoTileSide **dominoTilesInGame, int rows);
void rotateDominoTile(dominoTile *);
void fixSpecialTiles(dominoTile *, gameSituation *, int, int, char, char);
bool isValidMove(dominoTile, gameSituation, int, int, char, char);
bool endGame(int numTotalTiles, dominoTile[], gameSituation game);

#endif // COMMON_FUNCTIONS_H
