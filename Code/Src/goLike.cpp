/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 3
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * 
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

#include "../Include/goLike.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        GoLike::GoLike(
            int boardSize,
            const std::string gameName,
            const std::vector<std::string> playerNames)
            : BoardGame(CS111::BOARD_GAME::Board(boardSize, boardSize), gameName, GameStage::NOT_STARTED, 2, playerNames, 1, -1, -1, StepType::PUT_STONE, {})
        // explain 1 -1 -1: currentPlayerIndex is 1, meaning the first player (player 1) will start the game. winnerPlayerIndex is -1, meaning no winner yet. loserPlayerIndex is -1, meaning no loser yet.
        // bug fixed, current player changed from 0,   1,  Since player index is 1-based, the first player should be 1, not 0.
        {
            // throw exception if the board size is not a valid Go board size (odd number between 3 and 27)
            if (boardSize < 3 || boardSize > 27 || boardSize % 2 == 0)
            {
                throw std::invalid_argument("Invalid board size for GoLike game. Board size must be an odd number between 3 and 27.");
            }
            // throw exception if the number of player names is not equal to 2
            if (playerNames.size() != 2)
            {
                throw std::invalid_argument("GoLike game must have exactly 2 players. The number of player names must be equal to 2.");
            }

            // need to make sure that the stones of the board contains only Go stones: empty, black, white.  Since the Board constructor will not set the stones by default with the Go stones, we need to explicitly set the stones.

            Stones goStones = make_go_stones(); // declared in board_presets.hpp, defined in board_presets.cpp. It returns a set of 3 Go stones (1 black + 1 white + 1 empty).

            Board wanted_board = Board(boardSize, boardSize, goStones);
            this->initialGameState_.board_ = wanted_board;
            this->currentGameState_.board_ = wanted_board;
        }

    }
}