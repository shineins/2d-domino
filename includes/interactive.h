#ifndef INTERACTIVE_H
#define INTERACTIVE_H

/**
 * @file interactive.h
 * @author Daniele
 * @brief Library containing functions used in interactive mode
 * @date 06/02/2024
 */

#include "common_functions.h"
#include <ctype.h>

// Function prototypes
void playInteractive(int, dominoTile[]);
int chooseTile(int);
void insertTile(gameSituation *, dominoTile *, dominoTile, int, int, char, int *);
void printTilesInGame(gameSituation);
bool hasSpecials(dominoTile[], int);

#endif // INTERACTIVE_H
