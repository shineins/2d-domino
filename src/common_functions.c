/**
 * @file common_functions.c
 * @author Daniele
 * @brief Library containing common functions between the interactive domino mode and the
 * AI mode.
 * @date 06/02/2024
 */

#include "common_functions.h"

/**
 * Randomly generates normal and special tiles.
 * @param numTotalTiles total number of tiles to generate
 * @param numSpecials number of special tiles to generate
 * @param tilesOnHand array to fill with the generated tiles
 */

void generateTiles(int numTotalTiles, int numSpecials, dominoTile tilesOnHand[])
{
    // Normal tiles
    for (int i = 0; i < numTotalTiles - numSpecials; i++)
    {
        tilesOnHand[i].left = rand() % 6 + 1;
        // The number on the right side must be greater than or equal to the one on the left side
        tilesOnHand[i].right = rand() % (7 - tilesOnHand[i].left) + tilesOnHand[i].left;
    }

    // Special tiles
    for (int i = numTotalTiles - numSpecials; i < numTotalTiles; i++)
    {
        switch (rand() % 3)
        {
        case 0:
            tilesOnHand[i].left = tilesOnHand[i].right = 0;
            break;
        case 1:
            tilesOnHand[i].left = tilesOnHand[i].right = 11;
            break;
        case 2:
            tilesOnHand[i].left = 12;
            tilesOnHand[i].right = 21;
            break;
        }
    }
}

/**
 * Prints the generated tiles
 * @param numTiles number of generated tiles
 * @param tilesOnHand array with the tiles to print
 */
void printTiles(int numTiles, dominoTile tilesOnHand[])
{
    for (int i = 0; i < numTiles; i++)
    {
        printf("%d) [%d|%d]\n", i + 1, tilesOnHand[i].left, tilesOnHand[i].right);
    }
}

/**
 * Shifts the tiles by [posMoved] columns and sets the cells of the first [posMoved] columns to -1 (NOT_VALID_INT)
 * @param game game situation whose positions are to be shifted and whose columns were previously updated
 * (by + posMoved), but the last columns are empty
 * @param posMoved number of columns to shift
 */
void moveRight(gameSituation *game, int posMoved)
{
    for (int row = 0; row < game->rows; ++row)
    {
        /** The new index, matches the last empty column of the matrix */
        int newIndex = game->cols - 1;
        // The first cell to move is the last one with a valid value, meaning the last column before
        // having updated the columns
        for (int cell = game->cols - posMoved - 1; cell >= 0; --cell)
        {
            game->inGameDominoTiles[row][newIndex] = game->inGameDominoTiles[row][cell];
            if (cell < posMoved)
                game->inGameDominoTiles[row][cell].value = NOT_VALID_INT;
            newIndex--;
        }
    }
}

/**
 * Creates a game board by setting default values
 * @param rows rows of the board to create
 * @param cols columns of the board to create
 * @return newGame created game board
 */
dominoTileSide **createInGameDominoTiles(int rows, int cols)
{
    dominoTileSide **newGame = (dominoTileSide **)malloc(sizeof(dominoTileSide *) * rows);
    for (int row = 0; row < rows; row++)
    {
        newGame[row] = (dominoTileSide *)malloc(sizeof(dominoTileSide) * cols);
        for (int cell = 0; cell < cols; cell++)
        {
            newGame[row][cell].value = NOT_VALID_INT;
            newGame[row][cell].side = NOT_VALID_CHAR;
            newGame[row][cell].orientation = NOT_VALID_CHAR;
        }
    }
    return newGame;
}

/**
 * Frees the memory allocated on the heap for the matrix
 * @param dominoTilesInGame matrix of the game board
 * @param rows rows of the matrix
 */
void freeInGameDominoTiles(dominoTileSide **dominoTilesInGame, int rows)
{
    for (int row = 0; row < rows; row++)
    {
        free(dominoTilesInGame[row]);
    }
    free(dominoTilesInGame);
}

/**
 * Swaps the two values of the tile taken into consideration
 * @param tile tile to rotate
 */

void rotateDominoTile(dominoTile *tile)
{
    int temp = tile->left;
    tile->left = tile->right;
    tile->right = temp;
}

/**
 * Arranges the playing board and the tile taken into consideration based on the rules of the special tiles
 * @param dominoTile tile in hand
 * @param game game situation to update
 * @param row row where to insert tile
 * @param col column where to insert tile
 * @param orientation orientation of the tile taken from the hand
 * @param isLinkedTo which side the tile connects to, LEFT or RIGHT
 */

void fixSpecialTiles(dominoTile *dominoTile, gameSituation *game, int row, int col, char orientation, char isLinkedTo)
{
    switch (dominoTile->left)
    {
    case 11: // Case tile [11|11]
        for (int i = 0; i < game->rows; i++)
        {
            for (int j = 0; j < game->cols; j++)
            {
                if (game->inGameDominoTiles[i][j].value != NOT_VALID_INT)
                { // If the board position is not empty
                    if (game->inGameDominoTiles[i][j].value == 6)
                    {
                        game->inGameDominoTiles[i][j].value = 1; // If the tile contains a 6, I turn it into 1
                        game->score -= 5;                        // Decrease score by 5
                    }
                    else if (game->inGameDominoTiles[i][j].value != 0)
                    { // Zero is not incremented otherwise it loses its special ability
                        game->inGameDominoTiles[i][j].value++;
                        game->score++;
                    }
                }
            }
        }
        // Copy the value of the adjacent cell
        dominoTile->left = dominoTile->right = game->inGameDominoTiles[row][col].value;
        break;

    case 12: // Case tile [12|21]
        if (game->inGameDominoTiles[row][col].orientation == HORIZONTAL)
        {
            if (isLinkedTo == RIGHT)
            { // If tile in hand connects to the right of the one on the table
                dominoTile->left = game->inGameDominoTiles[row][col].value;
                dominoTile->right = game->inGameDominoTiles[row][col - 1].value;
            }
            else if (orientation == HORIZONTAL)
            { // If tile in hand connected to the left and horizontal
                dominoTile->left = game->inGameDominoTiles[row][col + 1].value;
                dominoTile->right = game->inGameDominoTiles[row][col].value;
            }
            else
            { // If tile in hand connected to the left and vertical
                dominoTile->left = game->inGameDominoTiles[row][col].value;
                dominoTile->right = game->inGameDominoTiles[row][col + 1].value;
            }
        }
        else
        { // If cell on board comes from a vertical tile
            if (orientation == HORIZONTAL)
            { // If tile in hand horizontal
                if (isLinkedTo == LEFT)
                { // If tile in hand connected to the left of the one on board
                    dominoTile->left = (game->inGameDominoTiles[row][col].side == TOP) ? game->inGameDominoTiles[row + 1][col].value : game->inGameDominoTiles[row - 1][col].value;
                    dominoTile->right = game->inGameDominoTiles[row][col].value;
                }
                else
                { // If tile in hand connected to the right of the one on board
                    dominoTile->left = game->inGameDominoTiles[row][col].value;
                    dominoTile->right = (game->inGameDominoTiles[row][col].side == TOP) ? game->inGameDominoTiles[row + 1][col].value : game->inGameDominoTiles[row - 1][col].value;
                }
            }
            else
            { // If tile in hand vertical
                dominoTile->left = game->inGameDominoTiles[row][col].value;
                dominoTile->right = game->inGameDominoTiles[row - 1][col].value;
            }
        }
        break;
    }
}

/**
 * Checks based on the adjacent tile if putting a tile from the hand,
 * the move is valid.
 * @param tileToPut tile in hand to put
 * @param game current game situation in which to insert the tile
 * @param rowTileInGame row of the tile to connect the one in hand to
 * @param colTileInGame column of the tile to connect the one in hand to
 * @param orientationTileToPut orientation of the tile to put
 * @param isLinkedTo specifies which side the tile in game is connected to the one to put
 * @return valid if the move is valid
 */
bool isValidMove(dominoTile tileToPut, gameSituation game, int rowTileInGame, int colTileInGame, char orientationTileToPut, char isLinkedTo)
{
    /** Side of the tile in play to which I am trying to connect */
    dominoTileSide adjacent = game.inGameDominoTiles[rowTileInGame][colTileInGame];
    // Check that the move involves special tiles because these can be connected regardless
    bool isSpecial = adjacent.value == 0 || tileToPut.left == 0 || tileToPut.left == 11 || tileToPut.left == 12;
    if (orientationTileToPut == HORIZONTAL)
    { // If I am putting it horizontally
        if (isLinkedTo == LEFT)
        { // If I am putting it to the left of the adjacent one
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.right == adjacent.value)
            {                                                                                                                  // If there is adjacency
                if (colTileInGame == 0                                                                                         // Edge
                    || (colTileInGame == 1 && game.inGameDominoTiles[rowTileInGame][colTileInGame - 1].value == NOT_VALID_INT) // If I am one cell before the edge
                    || (game.inGameDominoTiles[rowTileInGame][colTileInGame - 1].value == NOT_VALID_INT &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame - 2].value == NOT_VALID_INT))
                {
                    return true;
                }
            }
        }
        else if (isLinkedTo == RIGHT)
        { // If I am putting it to the right of the adjacent one
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.left == adjacent.value)
            {                                      // If there is adjacency
                if (colTileInGame == game.cols - 1 // Edge
                    || (colTileInGame == game.cols - 2 &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame + 1].value == NOT_VALID_INT) // If I am one cell before the edge
                    || (game.inGameDominoTiles[rowTileInGame][colTileInGame + 1].value == NOT_VALID_INT &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame + 2].value == NOT_VALID_INT))
                {
                    return true;
                }
            }
        }
    }
    else if (orientationTileToPut == VERTICAL)
    { // If I am putting it vertically
        if ((isLinkedTo == LEFT && colTileInGame == 0) || (colTileInGame == game.cols - 1 && isLinkedTo == RIGHT))
        { // Verticals can only be attached to the edges
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.left == adjacent.value)
            { // If there is adjacency or special
                // Verticals can be attached to horizontals or to other verticals on the bottom side
                if (adjacent.orientation == HORIZONTAL ||
                    (adjacent.orientation == VERTICAL && adjacent.side == BOTTOM))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Checks if the end of the game has been reached, meaning there are no more adjacencies
 * between the tiles in play (no specials) and those in hand.
 * @param numTotalTiles number of tiles in hand
 * @param tilesOnHand tiles in hand
 * @param game game situation
 * @return If the game is over
 */
bool endGame(int numTotalTiles, dominoTile tilesOnHand[], gameSituation game)
{
    // There are no more tiles in hand
    if (numTotalTiles == 0)
        return true;

    // If I haven't placed tiles yet so the game score is 0
    if (game.score == 0)
        return false;

    // Check if there are special tiles and if so then the game will surely not be over
    // because they can be attached anywhere
    for (int i = 0; i < numTotalTiles; i++)
    {
        if (tilesOnHand[i].left == 0 || tilesOnHand[i].left == 12 || tilesOnHand[i].left == 11)
            return false;
    }

    // Check if there are available combinations to continue the game
    for (int row = 0; row < game.rows; row++)
    {
        for (int cell = 0; cell < game.cols; cell++)
        {
            if (game.inGameDominoTiles[row][cell].value != NOT_VALID_INT)
            { // If there is a valid tile in play
                for (int onHandIndex = 0; onHandIndex < numTotalTiles; onHandIndex++)
                { // check if the tiles in hand have a number to put on the table
                    if (isValidMove(tilesOnHand[onHandIndex], game, row, cell, HORIZONTAL, LEFT) || isValidMove(tilesOnHand[onHandIndex], game, row, cell, HORIZONTAL, RIGHT) || isValidMove(tilesOnHand[onHandIndex], game, row, cell, VERTICAL, LEFT))
                    {
                        return false;
                    }
                    dominoTile tileReversed;
                    tileReversed.left = tilesOnHand[onHandIndex].right;
                    tileReversed.right = tilesOnHand[onHandIndex].left;
                    if (isValidMove(tileReversed, game, row, cell, HORIZONTAL, LEFT) || isValidMove(tileReversed, game, row, cell, HORIZONTAL, RIGHT) || isValidMove(tileReversed, game, row, cell, VERTICAL, LEFT))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
