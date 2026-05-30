/**
 * @file ai.c
 * @author Daniele
 * @brief Library containing the development of functions for AI mode
 * @date 06/02/2024
 */

// Includes
#include "ai.h"

/**
 * Main function that manages the AI mode: in particular it initializes the recursive function
 * and at the end prints its result.
 * @param tilesOnHand array of generated tiles
 * @param numTilesOnHand number of generated tiles
 */
void playAI(dominoTile tilesOnHand[], int numTilesOnHand)
{
    // The AI mode needs to have special tiles in a separate array
    // so that it can check them after each normal tile inserted.

    /**
     * Number of specials that will be updated by the function dividing them from normal ones,
     * since the function cannot return 2 values
     */
    int numSpecials = 0;

    /** Array of special tiles */
    // dominoTile *specialsOnHand = get_specials_array(tilesOnHand, numTilesOnHand, &numSpecials);

    /** Initial game situation (empty at this point) */
    gameSituation defaultGame = {
        .inGameDominoTiles = createInGameDominoTiles(1, 0),
        .rows = 1,
        .cols = 0,
        .score = 0};

    /** AI result calculated by the recursive function */
    gameSituation *bestGame = getBestGameAI(tilesOnHand, numTilesOnHand, &defaultGame);

    // Given the result, print the final game
    printAI(bestGame);

    // Free the dynamically allocated matrix for the initial game situation
    freeInGameDominoTiles(defaultGame.inGameDominoTiles, defaultGame.rows);

    // Free the result allocated in dynamic mem
    freeGameSituation(bestGame);
}

/**
 * Recursive function that tries all combinations to find the best game situation.
 * @param tilesOnHand generated tiles
 * @param numTilesOnHand number of generated tiles
 * @param defaultGame game situation up to this moment
 * @returns best game situation with the given tiles inserted in the given game situation
 */
gameSituation *getBestGameAI(dominoTile *tilesOnHand,
                             int numTilesOnHand,
                             gameSituation *defaultGame)
{
    // BASE CASE 1: special tiles are finished and !endGame
    // => check normal tiles if they are finished, if there are no more adjacencies with them
    // or if it's the start of the game and trying to put a special one
    if (endGame(numTilesOnHand, tilesOnHand, *defaultGame))
        return defaultGame;

    /** Best game situation */
    gameSituation *best = NULL;
    /**
     * Copy of special array that will be modified by shifting them left to try them all.
     * Creates it static and will be passed directly to a function for modification, to avoid further mallocs.
     */
    dominoTile copyTilesOnHand[numTilesOnHand];
    copyDominoTileAr(tilesOnHand, copyTilesOnHand, numTilesOnHand);
    for (int i = 0; i < numTilesOnHand; ++i)
    {
        // Check if the tile hasn't been checked previously (=> it is not earlier in the original array)
        if (!hasThisTileIn(*copyTilesOnHand, tilesOnHand, i))
        {
            gameSituation *actualBestIn;
            /** Tile to insert special or not */
            bool isSpecial = copyTilesOnHand->left == 0 || copyTilesOnHand->left == 11 || copyTilesOnHand->left == 12;
            // If I am at the beginning of the game I put a normal tile horizontally as the first tile in game
            if (defaultGame->score == 0 && !isSpecial)
            {
                gameSituation *in = copyGame(*defaultGame, HORIZONTAL, 0);

                dominoTileSide left = {.value = copyTilesOnHand->left, .side = LEFT, .orientation = HORIZONTAL};
                in->inGameDominoTiles[0][0] = left;
                dominoTileSide right = {.value = copyTilesOnHand->right, .side = RIGHT, .orientation = HORIZONTAL};
                in->inGameDominoTiles[0][1] = right;
                in->score = copyTilesOnHand->left + copyTilesOnHand->right;

                // Continue the recursion on the next tiles minus this one
                actualBestIn = getBestGameAI(copyTilesOnHand + 1, numTilesOnHand - 1, in);
                if (actualBestIn != in)
                    freeGameSituation(in);
            }
            else
            {
                // Check the current tile in all positions and then use recursion subsequently
                actualBestIn = bestGameTileIn(copyTilesOnHand, numTilesOnHand, *defaultGame);
            }

            if (actualBestIn && (!best || best->score < actualBestIn->score))
            {
                if (best)
                    freeGameSituation(best);
                best = actualBestIn;
            }
            else
            {
                freeGameSituation(actualBestIn);
            }
        }
        move_left(copyTilesOnHand, numTilesOnHand, *copyTilesOnHand);
    }

    return best;
}

/**
 * Returns the best game situation with the current tile inside in its best position by checking them
 * all and exploiting the recursive function.
 * @param tilesOnHand pointer to the first generated tile, after which there are sequentially the others
 * @param numTilesOnHand number of generated tiles
 * @param defaultGame game situation up to this moment
 * @returns best game situation with the current tile inserted in its best position in the given game situation
 */
gameSituation *bestGameTileIn(dominoTile *tilesOnHand,
                              int numTilesOnHand,
                              gameSituation defaultGame)
{

    /** Pointer to the best game situation */
    gameSituation *bestIn = NULL;

    // For each side of the tiles in game (=> each side is in a matrix cell)
    // I must check the adjacency with the passed tile
    for (int row = 0; row < defaultGame.rows; ++row)
    {
        for (int cell = 0; cell < defaultGame.cols; ++cell)
        {
            // Case 1: the adjacent tile in game is horizontal, the current tile can be attached
            // either to the right (H and V) or to the left (H and V) and this depends on which side the tile in game is
            // Check that the move is valid even in case of reversed

            // TILE LINKED TO THE LEFT
            gameSituation *in_H_L = bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, HORIZONTAL, LEFT, row, cell);
            if (in_H_L)
            {
                if (!bestIn || bestIn->score < in_H_L->score)
                {
                    if (bestIn)
                        freeGameSituation(bestIn);
                    bestIn = in_H_L;
                }
                else
                {
                    freeGameSituation(in_H_L);
                }
            }

            // TILE LINKED TO THE LEFT VERTICAL
            gameSituation *in_V_L = bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, VERTICAL, LEFT, row, cell);
            if (in_V_L)
            {
                if (!bestIn || bestIn->score < in_V_L->score)
                {
                    if (bestIn)
                        freeGameSituation(bestIn);
                    bestIn = in_V_L;
                }
                else
                {
                    freeGameSituation(in_V_L);
                }
            }

            // TILE LINKED TO THE RIGHT
            gameSituation *in_H_R = bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, HORIZONTAL, RIGHT, row, cell);
            if (in_H_R)
            {
                if (!bestIn || bestIn->score < in_H_R->score)
                {
                    if (bestIn)
                        freeGameSituation(bestIn);
                    bestIn = in_H_R;
                }
                else
                {
                    freeGameSituation(in_H_R);
                }
            }

            // TILE LINKED TO THE RIGHT VERTICAL
            gameSituation *in_V_R = bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, VERTICAL, RIGHT, row, cell);
            if (in_V_R)
            {
                if (!bestIn || bestIn->score < in_V_R->score)
                {
                    if (bestIn)
                        freeGameSituation(bestIn);
                    bestIn = in_V_R;
                }
                else
                {
                    freeGameSituation(in_V_R);
                }
            }
        }
    }
    return bestIn;
}

/**
 * Returns the best game situation (exploiting the recursive function) with the passed tile after
 * adding it in the position specified by the parameters
 * @param tilesOnHand pointer to the first generated tile, after which there are sequentially the others
 * @param numTilesOnHand number of generated tiles
 * @param defaultGame game situation up to this moment
 * @param tileToPutOrientation orientation of the tile to put
 * @param isLinkedTo which side the tile connects to the one in play
 * @param rowAdjacent row of the tile to connect the one in hand to
 * @param cellAdjacent column of the tile to connect the one in hand to
 * @returns NULL if invalid move or pointer to the best game situation allocated in dynamic memory
 */
gameSituation *bestTileInEachPosition(dominoTile *tilesOnHand,
                                      int numTilesOnHand,
                                      gameSituation defaultGame,
                                      char tileToPutOrientation,
                                      char isLinkedTo,
                                      int rowAdjacent,
                                      int cellAdjacent)
{
    /** Pointer to the best game situation with the current tile inside in the passed position */
    gameSituation *bestIn = NULL;
    /** Tile to insert into the game */
    dominoTile tileToAdd = *tilesOnHand;

    /** Tile to insert special or not */
    bool isSpecial = tilesOnHand->left == 0 || tilesOnHand->left == 11 || tilesOnHand->left == 12;

    /** Saves if the move with the current tile is valid */
    bool validAdjacency = isValidMove(tileToAdd, defaultGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
    // If the default tile is not adjacent also check reversed
    if (!validAdjacency)
    {
        rotateDominoTile(&tileToAdd);
        validAdjacency = isValidMove(tileToAdd, defaultGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
    }
    // If normal or rotated tile has adjacencies
    if (validAdjacency)
    {
        /** Copy of the game situation in which to insert the horizontal tile */
        gameSituation *tileInGame = copyGame(defaultGame, tileToPutOrientation, cellAdjacent);
        if (isSpecial)
            fixSpecialTiles(&tileToAdd, tileInGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
        addDominoTile(tileInGame, tileToAdd, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
        bestIn = getBestGameAI(tilesOnHand + 1, numTilesOnHand - 1, tileInGame);
        // If it is not the same previous game situation because the recursion is finished
        if (bestIn != tileInGame)
            freeGameSituation(tileInGame);
    }
    return bestIn;
}

/**
 * Executes the free of a game situation allocated in dynamic memory
 * @param game dynamically allocated game situation
 */
void freeGameSituation(gameSituation *game)
{
    if (game)
    {
        freeInGameDominoTiles(game->inGameDominoTiles, game->rows);
        free(game);
    }
}

/**
 * Adds the tile to the game based on the position to add and its orientation
 * by splitting between the two sides.
 * @param game game situation to update by adding the tile
 * @param tileToPut tile to add
 * @param rowTileInGame row of the tile to connect the one in hand to
 * @param colTileInGame column of the tile to connect the one in hand to
 * @param orientationTileToPut orientation of the tile to put
 * @param isLinkedTo which side the tile connects to, LEFT or RIGHT
 */
void addDominoTile(gameSituation *game, dominoTile tileToPut, int rowTileInGame, int colTileInGame, char orientationTileToPut, char isLinkedTo)
{
    // If the adjacent tile in play is on column 0, everything must be moved to the right by 1 or 2 columns based
    // on the orientation of the tile in play.
    if (colTileInGame == 0 && orientationTileToPut == HORIZONTAL)
    {
        moveRight(game, 2);
        // Addition of the tile
        game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
        game->inGameDominoTiles[rowTileInGame][0].orientation = HORIZONTAL;
        game->inGameDominoTiles[rowTileInGame][0].side = LEFT;

        game->inGameDominoTiles[rowTileInGame][1].value = tileToPut.right;
        game->inGameDominoTiles[rowTileInGame][1].orientation = HORIZONTAL;
        game->inGameDominoTiles[rowTileInGame][1].side = RIGHT;
    }
    else if (isLinkedTo == LEFT && ((colTileInGame == 0 && orientationTileToPut == VERTICAL) || (colTileInGame == 1 && orientationTileToPut == HORIZONTAL)))
    {
        // Move only by 1
        moveRight(game, 1);
        if (colTileInGame == 0)
        { // Case in which the tile is vertical and is on the edge
            game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][0].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][0].side = TOP;

            game->inGameDominoTiles[rowTileInGame + 1][0].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame + 1][0].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame + 1][0].side = BOTTOM;
        }
        else
        { // Case in which the tile is horizontal and before the edge
            game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][0].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][0].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][1].side = RIGHT;
        }
    }
    else
    {
        // Addition of the tile based on the side it is linked to
        if (isLinkedTo == LEFT && orientationTileToPut == HORIZONTAL)
        {
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 2].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 2].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 2].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].side = RIGHT;
        }
        else if (isLinkedTo == RIGHT && orientationTileToPut == HORIZONTAL)
        {
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][colTileInGame + 2].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 2].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 2].side = RIGHT;
        }
        else if (isLinkedTo == LEFT && orientationTileToPut == VERTICAL)
        {
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame - 1].side = TOP;

            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame - 1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame - 1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame - 1].side = BOTTOM;
        }
        else if (isLinkedTo == RIGHT && orientationTileToPut == VERTICAL)
        {
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame + 1].side = TOP;

            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame + 1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame + 1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame + 1][colTileInGame + 1].side = BOTTOM;
        }
    }

    // Update score
    game->score += tileToPut.left + tileToPut.right;
}

/**
 * Copies an array of dominoTile into another passed to the function, avoiding mallocs for the copy.
 * @param from array to copy from
 * @param to array to copy to
 * @param size array length
 */
void copyDominoTileAr(dominoTile from[], dominoTile *to, int size)
{
    for (int i = 0; i < size; ++i)
    {
        to[i] = from[i];
    }
}

/**
 * Copies a game situation into another already given game situation.
 * And updates the copy based on the tile to insert
 * @param defaultGame original game situation
 * @param toAddOrientation orientation of the tile to insert based on which to update rows or columns
 * @param toAddCell column in which to add the new tile to understand whether to increase rows and columns or not
 */
gameSituation *copyGame(gameSituation defaultGame, char toAddOrientation, int toAddCell)
{
    /** Number of rows for the copy of the game situation, updated on the position of the tile to insert */
    int newRows = defaultGame.rows;
    /** Number of columns for the copy of the game situation, updated on the position of the tile to insert */
    int newCols = defaultGame.cols;
    /** If tile must be inserted at the edges */
    bool borders = toAddCell == 0 || toAddCell == defaultGame.cols - 1;
    /** If tile must be inserted one cell before the edges */
    bool beforeBorders = toAddCell == 1 || toAddCell == defaultGame.cols - 2;
    // If tile must be inserted at the edges, update the number of columns and rows based on the position
    if (toAddOrientation == HORIZONTAL && borders)
    {
        newCols += 2;
    }
    else if (toAddOrientation == HORIZONTAL && beforeBorders)
    {
        newCols += 1;
    }
    else if (toAddOrientation == VERTICAL && borders)
    {
        newRows += 1;
        newCols += 1;
    }

    /** Copy of the dynamic array of arrays containing the sides of the tiles in play */
    dominoTileSide **copyInDominoTileSides = copyMatrixSides(defaultGame, newRows, newCols);
    /** Copy of the game situation in dynamic memory */
    gameSituation *copy = (gameSituation *)malloc(sizeof(gameSituation));
    copy->inGameDominoTiles = copyInDominoTileSides;
    copy->rows = newRows;
    copy->cols = newCols;
    copy->score = defaultGame.score;

    return copy;
}

/**
 * Copies the matrix containing the sides of the tiles in play.
 * @param originalGame game situation to copy the matrix from
 * @param rowsNewMat rows of the new matrix
 * @param colsNewMat columns of the new matrix
 */
dominoTileSide **copyMatrixSides(gameSituation originalGame, int rowsNewMat, int colsNewMat)
{
    /** Dynamic array of arrays in which the original one is copied */
    dominoTileSide **copy = (dominoTileSide **)malloc((rowsNewMat) * sizeof(dominoTileSide *));
    for (int i = 0; i < rowsNewMat; ++i)
    {
        copy[i] = (dominoTileSide *)malloc((colsNewMat) * sizeof(dominoTileSide));
        for (int j = 0; j < colsNewMat; ++j)
        {
            // If the original matrix is smaller, NOT_VALID_INT is put in the newly created places
            if (i >= originalGame.rows || j >= originalGame.cols)
            {
                copy[i][j].value = NOT_VALID_INT;
                copy[i][j].orientation = NOT_VALID_CHAR;
                copy[i][j].side = NOT_VALID_CHAR;
            }
            else
            {
                copy[i][j] = originalGame.inGameDominoTiles[i][j];
            }
        }
    }
    return copy;
}

/**
 * Verifies that the inserted tile is not in a given array
 * @param toFound tile to find
 * @param ar array to search in
 * @param toCheckUntil up to where to search
 * @returns if the tile is present
 */
bool hasThisTileIn(dominoTile toFound, dominoTile *ar, int toCheckUntil)
{
    for (int i = 0; i < toCheckUntil; ++i)
    {
        if (ar[i].left == toFound.left && ar[i].right == toFound.right)
            return true;
    }
    return false;
}

/**
 * Checks if the given tile (non-special) has adjacencies with the tiles of the given game situation
 * @param tile tile to verify
 * @param game game situation
 * @returns if the tile has adjacencies
 */
bool hasTileAdjacency(dominoTile tile, gameSituation game)
{
    // Check if there are available combinations
    for (int row = 0; row < game.rows; row++)
    {
        for (int cell = 0; cell < game.cols; cell++)
        {
            if (game.inGameDominoTiles[row][cell].value != NOT_VALID_INT)
            { // If there is a valid tile in play
                if (isValidMove(tile, game, row, cell, HORIZONTAL, LEFT) || isValidMove(tile, game, row, cell, HORIZONTAL, RIGHT) || isValidMove(tile, game, row, cell, VERTICAL, LEFT))
                {
                    return true;
                }
                dominoTile tileReversed;
                tileReversed.left = tile.right;
                tileReversed.right = tile.left;
                if (isValidMove(tileReversed, game, row, cell, HORIZONTAL, LEFT) || isValidMove(tileReversed, game, row, cell, HORIZONTAL, RIGHT) || isValidMove(tileReversed, game, row, cell, VERTICAL, LEFT))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Shifts the array left and adds the given tile at the end
 * (the first tile is overwritten)
 * @param ar array of tiles to shift
 * @param numTiles number of tiles in the array
 * @param to_add tile to add
 * */
void move_left(dominoTile *ar, int numTiles, dominoTile to_add)
{
    for (int i = 0; i < numTiles - 1; ++i)
    {
        ar[i] = ar[i + 1];
    }
    ar[numTiles - 1] = to_add;
}

/**
 * Prints the AI result, that is the best finished game given the generated tiles.
 * @param game best game situation found by the AI
 */
void printAI(gameSituation *game)
{
    for (int row = 0; row < game->rows; ++row)
    {
        for (int cell = 0; cell < game->cols; ++cell)
        {
            if (game->inGameDominoTiles[row][cell].value == NOT_VALID_INT)
            {
                printf("   ");
            }
            else
            {
                if (game->inGameDominoTiles[row][cell].orientation == HORIZONTAL && game->inGameDominoTiles[row][cell].side == LEFT)
                {
                    printf("[%d|", game->inGameDominoTiles[row][cell].value);
                }
                else if (game->inGameDominoTiles[row][cell].orientation == HORIZONTAL &&
                         game->inGameDominoTiles[row][cell].side == RIGHT)
                {
                    printf("|%d]", game->inGameDominoTiles[row][cell].value);
                }
                else if (game->inGameDominoTiles[row][cell].orientation == VERTICAL &&
                         game->inGameDominoTiles[row][cell].side == TOP)
                {
                    printf("{%d:", game->inGameDominoTiles[row][cell].value);
                }
                else if (game->inGameDominoTiles[row][cell].orientation == VERTICAL &&
                         game->inGameDominoTiles[row][cell].side == BOTTOM)
                {
                    printf(":%d}", game->inGameDominoTiles[row][cell].value);
                }
            }
        }
        if (row != game->rows - 1)
            (printf("\n"));
    }
}
