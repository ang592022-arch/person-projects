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

// nQueen.hpp
//
// NQueen Game (Two-Player Interactive Version)
// ------------------------------------------------------------
// Rules: given a board of N rows and N colmumns, where N is an even number.
// 1. Two players take turns placing their queen stones (e.g., Black and White) on an N x N chessboard.
// 2. A step is valid if the chosen square is empty and no previously placed queen (of either player) threatens the new queen (no same row, column, or diagonal). In other word, the new queen should not appear in the same line (row, column, diagonal) with any existing queen.
// 3. If a player attempts to place a queen in a position that is threatened (conflict), the step fails and the other player immediately wins.
// 4. Obviously, there are at most N queen stones can appear on a boards. If a player successfully places the Nth queen stone,  that player wins and the other player loses.
// 5. The game ends when either a conflict occurs or the nth queen is placed.
// 6. The board and move history can be saved/loaded, and the game can be replayed (inherited from BoardGame).

/*
Other interesting rules, but they are not implemented in this version, just for fun and future extension:
a)  after putting the queen in the last row, the board becomes larger, and the game continues until a conflict condition is met. But we take the simple appraoch here, as shown by rule 4.
b)  The players need to place the a stone in the first row, then the second rone, till the nth row. This will still make the game challenging, and can make the understanding of the algorithm easier. But this is not necesssary. 
*/

// This class is designed to be minimal and consistent with GoBang: override only the necessary methods.
// Author: (Your Name)
// Date: 2026-05-28

#ifndef CS111_BOARD_GAME_NQUEEN_HPP
#define CS111_BOARD_GAME_NQUEEN_HPP

#include "boardGame.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {

        class NQueen : public BoardGame
        {
        public:
            // Construct an NQueen game with given board size and player names.
            // n is the boardSize. The board will be n x n. n must be an even number, otherwise the game will be trivial and not interesting, as the first player can always win by placing queens in the first row. We can check this in the constructor and throw an exception if n is not even.
            NQueen(int n, const std::string &gameName, const std::vector<std::string> &playerNames);
            ~NQueen() override = default; 

            // Check if a step is valid: empty square, no queen threatens the position.
            bool is_step_valid(const Step &step) const override;

            // Apply a step, update the board and game state, check for win/loss.
            void do_step_and_update(const Step &step) override;

            // Clone the current game state (for branching/replay).
            std::unique_ptr<BoardGame> branch_the_current_game() const override;



            // (Optional) Add any additional public methods if needed.

        //private:
        protected:
            // Helper: Check if placing a queen at (row, col) is threatened by any existing queen.
            //  Note row and col is 1-based. 
            bool is_queen_threatened(int row, int col) const;

            // Helper: Check if the step places a queen in the last row. But this is not really needed. 
            // bool is_last_row(int row) const;

            // (Optional) Add any additional private or protected helpers if needed.

        public:
            // helpful to produce a Stone catalog with only three stones, the empty stone, the black queen stone, and the white queen stone. It could be useful for some cases.
            static Stones make_queen_stones();

            static Board make_n_queen_board(int n); // a helper function to make an n x n board for n queen game. It can be used in the constructor of NQueen to initialize the board.

            // Friend test function for white-box testing
            // Friend is not a member. It is irrelevent to be public or protected. It can be declared in either section. Here it is declared in the public section, just for no particular reason.
            friend void test_NQueen();
        }; // end of class NQueen

    void test_NQueen(); // better to declare this function in the related namespace. 

    } // namespace BOARD_GAME
} // namespace CS111

#endif // CS111_BOARD_GAME_NQUEEN_HPP
