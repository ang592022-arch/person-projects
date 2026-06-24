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

 #include "../Include/connect.hpp"
 #include <iostream>
 // !!!!!!!!!!!!!! < Hmk3 Task 10 > !!!!!!!!!!!!!!
// Provide the missing code of this file. You can design 
// the content of this file. 

int main()
{
  
    std::unique_ptr<CS111::BOARD_GAME::Connect> game
        = std::make_unique<CS111::BOARD_GAME::Connect>(9, "Connect Four", std::vector<std::string>{"Player1", "Player2"});
    game->play();
    return 0;
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
As long as playing the Connect game works, showing correct game logic, and showing the board after each step, it is good enough. You can design the content of this file as you like. For example, you can design some test cases, or you can design a simple interactive interface to play the game with user input.
*/