2D Domino Game
Description

This project is an advanced 2D implementation of a Domino game developed in C. It allows players to place tiles on a 2D grid matrix to maximize their score and features two main gameplay modes: Interactive and AI. The project was built with a strong focus on modularity, utilizing appropriate data structures and programming paradigms.
Project Structure

The repository is organized into specific directories to maintain clear modularity:

    build/: Contains the compiled object files.

    includes/: Stores the header files.

    src/: Contains the source code implementations, logically divided into:

        ai.c: Implementation of the AI mode.  

        interactive.c: Implementation of the Interactive mode.  

        common_functions.c: Shared functions utilized by both game modes.  

    tools/: Contains the main execution files.


Gameplay & 2D Rules

    The game is played on a 2D matrix grid, which facilitates adjacency checks for tile placement.

    The player receives a set of N tiles (each containing two digits from 1 to 6) and aims to place them on the board to accumulate the highest possible score. The score is the sum of all digits on the board.

    While tiles can be connected horizontally, the 2D expansion allows tiles to be placed vertically at the extreme ends of the current board.

    Vertical tiles must grow downwards, creating new horizontal branches and allowing the game grid to expand in multiple directions.

    In Interactive mode, the user is prompted to choose which existing tile on the board they want to connect their new tile to (rather than selecting an empty cell), which simplifies compatibility checking.

Special Tiles

The game includes unique challenge tiles that alter the board state:

    [0/0]: A wildcard tile that can connect to any number.

    [11/11]: Adds 1 to all digits currently on the board (6s become 1s) and copies the adjacent digit increased by 1.

    [12/21]: A mirror tile that reverses the digits of the adjacent tile it connects to.

Game Modes

    Interactive Mode: The user manually inputs their moves to place tiles, and the board and score update accordingly until no valid moves remain.

    AI Mode: An algorithm automatically determines the best possible game from a given set of generated tiles. The AI utilizes a recursive algorithm to exhaustively check for the highest-scoring placement. It dynamically allocates memory to save the best game state and pass it back to the initial recursive call, carefully managing memory deallocation to prevent leaks.

Development & Challenges

The workload was evenly distributed between two developers, with each taking ownership of one specific game mode, while common functions were brainstormed and developed collaboratively. The main technical challenges involved:

    Adapting the matrix data structure to check adjacencies one side at a time.

    Deciding on the most efficient user input method for tile placement.

    Managing the dynamic memory for the recursive AI's best-state tracking while successfully avoiding memory leaks.
