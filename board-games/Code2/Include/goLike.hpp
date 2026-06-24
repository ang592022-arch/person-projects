/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 3
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

/*
A GoLike game has a board of Go. The is the only main feature as a derived class of BoardGame. More sepcificly, such a game features:
- Only two players
- Only three kind of stones: empty, black, white.
- rows and columns are the same (square) and should be odd number. (3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27)

*/

#ifndef GOLIKE_HPP
#define GOLIKE_HPP
#include "boardGame.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        class GoLike : public BoardGame
        {
            // the constructor. It is the only moethod in this class, just make sure the board is a Go board.
            // to make sure all the needed information are there, sufficient to pass to the bass class BoardGame, the parameters of BoardGame are shown below.
            /*
               const CS111::BOARD_GAME::Board board, // 1)
                 const std::string gameName = "ANY", //2)
                 GameStage gameStage = GameStage::NOT_STARTED, // 3)
                 int playerNumber = 2, // 4)
                 const std::vector<std::string> playerNames = {"playerA", "playerB"}, // 5)
                 int currentPlayerIndex = 0, // 6)
                 int winnerPlayerIndex = -1, // 7)
                 int loserPlayerIndex = -1, // 7.5)
                 StepType stepType = StepType::PUT_STONE, // 8)
                 const std::vector<Step> stepHistory = {}) // 9)
            */
            /* For simplicity,  for 1), we only need a size parameter which is used to construct a board. For the other parameters, only 2), 5) passed as a parameters, and rest can be the default values. In case some default values are not suitable, they can still be changed before a game starts, maybe by a prepare() function in derived class, since all the data members are protected.
             */
        public:
            GoLike(
                int boardSize,
                const std::string gameName = "GoLike",                              // 2)
                const std::vector<std::string> playerNames = {"playerA", "playerB"} // 5)
                )
                ;
        };

    }
}

#endif // GOLIKE_HPP