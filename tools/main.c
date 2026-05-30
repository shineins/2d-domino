/**
 * @file main.c
 * @author Daniele
 * @mainpage Play Domino
 * @section intro_sec Introduction
 *
 * Welcome to Domino. You can play trying to get the best sequence.
 * You can try it yourself, but we also have an AI mode to help you. The domino
 * can be developed both horizontally and vertically.
 *
 *
 * @section rules_sec Rules
 * Rule 1: Start with a horizontal tile.\n
 * Rule 2: Vertical tiles can only be attached to the extremes of the playing board.\n
 * Rule 3: Vertical tiles can only be attached to the bottom side of an adjacent vertical tile.\n
 * Rule 4: The domino can only grow downwards and not upwards.\n
 * Rule 5: You will decide the number of special tiles to play with, but at least one must be normal.\n
 * Rule 6: Special tile 0|0 can be attached anywhere.\n
 * Rule 7: Special tile 11|11 can be attached anywhere and increments all tiles by one except 6s which become 1s.\n
 * Rule 8: Special tile 12|21 can be attached anywhere and mirrors the adjacent tile.\n
 * Rule 9: Tiles inserted between two others must be adjacent from both right and left.\n
 *
 *
 * @subsection modalita_1 Game modes
 * \li Mode 1: Interactive
 * \li Mode 2: AI
 * @subsection modalita_2 Interactive mode:
 * A sequence of classic + special tiles will be generated and you must try
 * to get the best sequence, meaning the maximum score. If there are no more adjacencies
 * between the tiles in your hand and those on the board, the game will end.
 * @subsection modalita_3 AI mode:
 * A sequence of classic + special tiles will be generated and the computer will find
 * the best sequence with them.
 */

#include "ai.h"
#include "interactive.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main()
{
    srand(time(0)); // Random
    int numTotalTiles, numSpecials, choice = 0;
    do
    {
        printf("How many tiles do you want to play with? (At least 2):");
        scanf("%d", &numTotalTiles);
        printf("How many of these do you want to be special tiles?");
        scanf("%d", &numSpecials);
        if (numTotalTiles < 2 || numSpecials < 0 || numSpecials >= numTotalTiles)
            printf("Number of tiles not allowed! Enter valid numbers and remember that you must play with at least 1 normal tile and 2 in total\n");
    } while (numTotalTiles < 2 || numSpecials < 0 || numSpecials >= numTotalTiles); // Keep asking until the number of normal tiles is at least 1

    // Start game, generate normal and special tiles
    dominoTile dominoTiles[numTotalTiles];
    generateTiles(numTotalTiles, numSpecials, dominoTiles);

    printTiles(numTotalTiles, dominoTiles);

    // Ask for game mode
    printf("\n--> Choose the game mode: \n");
    printf("1) Interactive mode -> you choose how to position the given tiles \n");
    printf("2) AI mode -> our AI suggests the best strategy with the given tiles");
    do
    {
        printf("\n--> Choice: ");
        scanf("%d", &choice);
    } while (choice != 1 && choice != 2);

    switch (choice)
    {
    case 1:
        printf("--- You chose the interactive mode! ---\n");
        playInteractive(numTotalTiles, dominoTiles);
        break;
    case 2:
        printf("--- You chose the AI mode! ---\n");
        // Call the AI function that generates and prints the best sequence
        playAI(dominoTiles, numTotalTiles);
        break;
    }
    return 0;
}
