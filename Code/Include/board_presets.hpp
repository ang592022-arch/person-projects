/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 2
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * 
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

// This file is for declaring tools for making preset boards, such as Chinese chess, Western chess, and Go.

#ifndef BOARD_PRESETS_HPP
#define BOARD_PRESETS_HPP

#include "board.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        Stones make_chinese_chess_stones(); // a set of 15 Chinese chess pieces (7 black + 7 red + 1 empty)
        Stones make_western_chess_stones(); // a set of 13 Western chess pieces (6 white + 6 black + 1 empty)
        Stones make_go_stones(); // a set of 3 Go stones (1 black + 1 white + 1 empty)
        Board make_chinese_chess_board(); // a 9x10 board with the standard Chinese chess starting position, professional labels, and AtBlank stone position style
        Board make_western_chess_board(); // a 8x8 board with the standard Western chess starting position, professional labels, and AtBlank stone position style
        Board make_go_board(); // a 19x19 board with the standard empty Go starting position, professional labels, and AtCross stone position style
    }
}

#endif
