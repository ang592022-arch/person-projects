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


/*
====================
  Connect Game Rules
====================

Connect is a two-player game played on a square Go-like board of odd size (e.g., 9x9, 11x11, etc.).

Game Setup:
- The board is divided into two parts: the top half (rows 1 to mid-1) and the bottom half (rows mid+1 to n), where mid = (n+1)/2.
- The middle row (row mid) is left empty and no stones may be placed or moved there.
- Each player has a set of stones (e.g., X for Black, O for White).
- At the start, Black's stones are placed only in the top half, White's stones only in the bottom half. The initial distribution can be random or predefined.

Gameplay:
- Players take turns. On each turn, a player moves one of their own stones.
- A stone may move one unit horizontally or vertically (not diagonally) to an adjacent empty square.
- Each player can only move their stones within their own part of the board (top or bottom half). Stones cannot cross into the middle row or the opponent's half.

Connection Definition:
- Two stones are considered connected if they are adjacent horizontally or vertically (i.e., their positions differ by exactly one in either row or column, but not both).
- A group of stones is connected if every stone in the group can be reached from any other stone in the group by a sequence of such connections.
- Diagonal adjacency does NOT count as connected.

Winning Condition:
- The first player to connect all of their own stones into a single connected group (as defined above), with less number of steps, wins the game.
- If both player can achieve this, by the same number of steps, the game is a draw.

Example Board (X = Black, O = White, · = empty):

   A   B   C   D   E   F   G   H   J
 1 ·   ·   ·   ·   ·   ·   ·   ·   ·
 2 ·   ·   ·   X   ·   ·   ·   ·   ·
 3 ·   ·   ·   ·   ·   X   ·   ·   ·
 4 ·   ·   ·   ·   X   ·   X   ·   ·
 5 ·   ·   ·   ·   ·   ·   ·   ·   ·
 6 ·   ·   O   ·   ·   ·   ·   ·   ·
 7 ·   ·   ·   ·   O   ·   ·   ·   ·
 8 ·   ·   ·   ·   ·   O   ·   O   ·
 9 ·   ·   ·   ·   ·   ·   ·   ·   ·

After three steps, Black (X) wins:

   A   B   C   D   E   F   G   H   J
 1 ·   ·   ·   ·   ·   ·   ·   ·   ·
 2 ·   ·   ·   ·   X   ·   ·   ·   ·
 3 ·   ·   ·   ·   X   X   X   ·   ·
 4 ·   ·   ·   ·   ·   ·   ·   ·   ·
 5 ·   ·   ·   ·   ·   ·   ·   ·   ·
 6 ·   ·   ·   ·   ·   ·   ·   ·   ·
 7 ·   ·   ·   O   ·   O   ·   ·   ·
 8 ·   ·   ·   ·   ·   O   O   ·   ·
 9 ·   ·   ·   ·   ·   ·   ·   ·   ·

Note: For best compatibility in all editors, X and O are used for stones. Unicode symbols may not align in all environments.
*/

/* Note that when Connect is a derived a class of GoLike, the board can be displayed using pretty unicode character. However, when unicode are not displayed as expected with same width as other characters in a text editor. So，in the above,  the board will use X or O for easier readability. */

#ifndef CS111_BOARD_GAME_CONNECT_HPP
#define CS111_BOARD_GAME_CONNECT_HPP

#include "boardGame.hpp"
#include "goLike.hpp"
#include <vector>
#include <string>
namespace CS111
{
    namespace BOARD_GAME
    {
        class Connect : public GoLike
        {
        // !!!!!!!!!!!!!! < Hmk3 Task 8 > !!!!!!!!!!!!!!
        // Provide the missing code of this class.. You can design 
        // the content of this file. Some code outside this class is also possi


public:
    Connect(int boardSize,
            const std::string& gameName = "Connect",
            const std::vector<std::string>& playerNames = {"Black(X)", "White(O)"});

    bool check_and_handle_no_more_step() override;
    void end() override;

private:
    bool all_stone_linked(int playerID) const;
    static Board make_connect_board(int rows, int cols);
};

} // BOARD_GAME
} // CS111

#endif
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
