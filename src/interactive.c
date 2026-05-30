/**
 * @file interactive.c
 * @author Daniele
 * @brief Library containing functions of the interactive mode
 * @date 06/02/2024
 */

#include "interactive.h"

/**
 * Function for the interactive mode
 * @param numTiles number of tiles in hand
 * @param dominoTiles domino tiles in hand
 */
void playInteractive(int numTiles, dominoTile dominoTiles[])
{
    gameSituation game;
    int numTotalTiles = numTiles;
    game.inGameDominoTiles = createInGameDominoTiles(numTiles * 2, numTiles * 2); // Initialize the board with the maximum number of tiles it can contain
    game.rows = 1;
    game.cols = 2;
    game.score = 0;
    do
    { // Until I have no more cards to play
        printf("Your score is: %d\n", game.score);
        printf("Enter the position of the tile to put on the table\n");
        int onHandIndex = chooseTile(numTiles);
        bool isSpecial = (dominoTiles[onHandIndex].left == 0 || dominoTiles[onHandIndex].left == 11 || dominoTiles[onHandIndex].left == 12);
        if (game.score == 0)
        {                   // If first turn
            if (!isSpecial) // If not special tile
                insertTile(&game, &dominoTiles[onHandIndex], dominoTiles[numTiles - 1], 0, 0, HORIZONTAL, &numTiles);
            else
                printf("You cannot start with a special tile\n");
        }
        else // If it is not the first turn
        {
            int row, col;
            char orientation = NOT_VALID_CHAR, isLinkedTo = NOT_VALID_CHAR;
            do
            {
                printf("Enter the coordinates of the tile you want to connect the tile in your hand to (enter valid values)\n");
                printf("Row: ");
                scanf("%d", &row);
                printf("Column: ");
                scanf("%d", &col);
            } while ((row < 0 || row >= game.rows) || (col < 0 || col >= game.cols));
            // Rotate tile
            if (!isSpecial)
            { // If it's not a special tile ask if wants to rotate
                char rotate = NOT_VALID_CHAR;
                printf("Do you want to rotate the tile? y/n: ");
                while (rotate != 'y' && rotate != 'n')
                    scanf("%c", &rotate);
                if (rotate == 'y')
                    rotateDominoTile(&dominoTiles[onHandIndex]);
            }
            // Ask tile orientation
            printf("Choose tile orientation (h=horizontal v=vertical b=back): ");
            while (orientation != 'h' && orientation != 'v' && orientation != 'b')
                scanf("%c", &orientation);
            orientation = toupper(orientation);

            if (game.inGameDominoTiles[row][col].orientation == VERTICAL)
            {                                               // If the orientation of the adjacent tile is vertical
                if (orientation == VERTICAL)                // If the orientation of the tile I am putting is vertical
                    isLinkedTo = (col == 0) ? LEFT : RIGHT; // If I am on the first column it links to the left, if it's on the last it links to the right
                else
                { // If the orientation of the tile I am putting is horizontal
                    if (col + 1 < game.cols && game.inGameDominoTiles[row][col + 1].value != NOT_VALID_INT)
                        isLinkedTo = LEFT; // If col+1 exists and the next place is occupied, the tile to put is linked to the left
                    else if (col - 1 >= 0 && game.inGameDominoTiles[row][col - 1].value != NOT_VALID_INT)
                        isLinkedTo = RIGHT; // If col-1 exists and the next place is occupied, the tile to put is linked to the right
                    else
                    { // If I can choose which side to link it to
                        printf("Do you want to connect the tile to the left or right? (l=left r=right): ");
                        while (isLinkedTo != 'l' && isLinkedTo != 'r')
                            scanf("%c", &isLinkedTo);
                        isLinkedTo = toupper(isLinkedTo);
                    }
                }
            }
            else                                                                             // If the orientation of the adjacent tile is horizontal
                isLinkedTo = (game.inGameDominoTiles[row][col].side == LEFT) ? LEFT : RIGHT; // If the part taken on the board is the left one then it will connect to the left, otherwise vice versa

            if (isValidMove(dominoTiles[onHandIndex], game, row, col, orientation, isLinkedTo))
            { // If the move is valid
                if (dominoTiles[onHandIndex].left == 12 || dominoTiles[onHandIndex].left == 11)
                {
                    fixSpecialTiles(dominoTiles + onHandIndex, &game, row, col, orientation, isLinkedTo);
                }

                if (orientation == HORIZONTAL)
                {
                    if (isLinkedTo == LEFT)
                    {
                        if (col == 0 || col == 1)
                        {
                            game.cols += 2 - col;      // If col==1 cols++, if col==0 cols=cols+2
                            moveRight(&game, 2 - col); // If col==0 shift by 2, otherwise shift by 1
                            col = 0;                   // the tile is placed in the first position of the column
                        }
                        else
                            col -= 2; // If the first two columns are not used, the position of the tile to put will be col-2
                    }
                    else if (isLinkedTo == RIGHT)
                    {
                        if (col == game.cols - 1)
                            game.cols += 2; // If the last column is used, the field must be increased by 2
                        else if (col == game.cols - 2)
                            game.cols++; // If the penultimate column is used, the field must be increased by 1
                        col++;           // The tile will be put one column later
                    }
                }
                else if (orientation == VERTICAL)
                {
                    game.cols++; // It will always increase the board column by 1
                    if (col == 0)
                        moveRight(&game, 1); // If I use the first column, shift the board by 1
                    else
                        col = (isLinkedTo == LEFT) ? col - 1 : col + 1; // If linked to the left position at col-1, otherwise at col+1
                    if (row == game.rows - 1)
                        game.rows++; // If I use the last row, add a row to the board
                }
                insertTile(&game, &dominoTiles[onHandIndex], dominoTiles[numTiles - 1], row, col, orientation, &numTiles);
            }
            else
                printf("You cannot make this move\n");
        }
        printTiles(numTiles, dominoTiles);
        printTilesInGame(game);
    } while (!endGame(numTiles, dominoTiles, game));
    // Free memory and show score
    // free(dominoTiles);
    freeInGameDominoTiles(game.inGameDominoTiles, (numTotalTiles * 2));
    printf("Game over! Your score is %d", game.score);
}

/**
 * Checks if there are specials among the present tiles
 * @param tilesOnHand tiles in hand
 * @param numTotalTiles number of tiles in hand
 */
bool hasSpecials(dominoTile tilesOnHand[], int numTotalTiles)
{
    // Check if there are special tiles and if so then the game will surely not be over because they can be attached anywhere
    for (int i = 0; i < numTotalTiles; i++)
    {
        if (tilesOnHand[i].left == 0 || tilesOnHand[i].left == 12 || tilesOnHand[i].left == 11)
            return true;
    }
    return false;
}

/**
 * allows the user to choose one of the tiles in hand
 * @param numTiles number of tiles in hand
 */
int chooseTile(int numTiles)
{
    int onHandIndex;
    do
    {
        printf("Enter a valid position: ");
        scanf("%d", &onHandIndex);
    } while (onHandIndex > numTiles || onHandIndex < 1);
    return (onHandIndex - 1);
}

/**
 * inserts a tile in the game board
 * @param game game situation containing score, rows, columns and tiles in play
 * @param dominoTileOnHand domino tile to put on the board
 * @param lastDominoTile last tile in hand
 * @param row row of the position where to put the tile
 * @param col column of the position where to put the tile
 * @param orientation orientation of the tile to be placed (horizontal or vertical)
 * @param numTiles number of tiles in hand
 */

void insertTile(gameSituation *game, dominoTile *dominoTileOnHand, dominoTile lastDominoTile, int row, int col, char orientation, int *numTiles)
{
    if (orientation == HORIZONTAL)
    {
        game->inGameDominoTiles[row][col].value = dominoTileOnHand->left;
        game->inGameDominoTiles[row][col].side = LEFT;
        game->inGameDominoTiles[row][col + 1].value = dominoTileOnHand->right;
        game->inGameDominoTiles[row][col + 1].side = RIGHT;
        game->inGameDominoTiles[row][col].orientation = game->inGameDominoTiles[row][col + 1].orientation = HORIZONTAL;
    }
    else
    {
        game->inGameDominoTiles[row][col].value = dominoTileOnHand->left;
        game->inGameDominoTiles[row][col].side = TOP;
        game->inGameDominoTiles[row + 1][col].value = dominoTileOnHand->right;
        game->inGameDominoTiles[row + 1][col].side = BOTTOM;
        game->inGameDominoTiles[row][col].orientation = game->inGameDominoTiles[row + 1][col].orientation = VERTICAL;
    }
    game->score += dominoTileOnHand->left + dominoTileOnHand->right;
    (*numTiles)--;
    *dominoTileOnHand = lastDominoTile;
}

/**
 * Prints the game board
 * @param game game situation containing score, rows, columns and tiles in play
 */

void printTilesInGame(gameSituation game)
{
    printf("tiles on the table:\n");
    printf("   ");
    for (int col = 0; col < game.cols; col++)
        printf("%d  ", col);
    printf("\n");
    for (int row = 0; row < game.rows; row++)
    {
        printf("%d ", row);
        for (int col = 0; col < game.cols; col++)
        {
            if (game.inGameDominoTiles[row][col].value != NOT_VALID_INT)
            {
                if (game.inGameDominoTiles[row][col].side == LEFT)
                    printf("[%d|", game.inGameDominoTiles[row][col].value);
                else if (game.inGameDominoTiles[row][col].side == RIGHT)
                    printf("|%d]", game.inGameDominoTiles[row][col].value);
                else if (game.inGameDominoTiles[row][col].side == TOP)
                    printf("[%d:", game.inGameDominoTiles[row][col].value);
                else if (game.inGameDominoTiles[row][col].side == BOTTOM)
                    printf(":%d]", game.inGameDominoTiles[row][col].value);
            }
            else
                printf("   ");
        }
        printf("\n");
    }
}
