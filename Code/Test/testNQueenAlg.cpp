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

#include "../Include/nQueenAlg.hpp"

// !!!!!!!!!!!!!! < Hmk3 Task 7 > !!!!!!!!!!!!!!
// Provide the missing code of this file. You can design 
// the content of this file. 

#include <iostream>
int main()
{
    CS111::BOARD_GAME::NQueenAlg alg(4);
    std::cout << "===== Single Solution of 4-Queen =====\n";
    alg.print_one_solution();

    std::cout << "\n===== All Solutions of 4-Queen =====\n";
    alg.print_all_solutions();

    return 0;
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/* 
The requirement is simple. 
As long as the testing can show one solution, and show all solutions, it is good enough . 
*/