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

#ifndef NQUEEN_ALG_HPP
#define NQUEEN_ALG_HPP

#include "nQueen.hpp"
#include <vector>


/**
 * @brief Finds one solution to the N-Queen problem using backtracking.
 * 
 * This function attempts to extend a partial solution to a complete one by placing queens
 * row by row, ensuring no two queens attack each other. If the board already contains
 * pre-placed queens, they are treated as fixed and cannot be moved.
 * 
 * @param board A 2D container representing the chessboard.   The container supports index operator of two levels, holding stones. For example, when board is a 2D char array, board[0] represent the first row, board[2][5] represents the stone (a character) at the 3rd row and 6th column (array index is 0-based). In the 2D array, a 'Q' represents a queen, other chars represent empty squares.
 * @param n A positive integer. The size of the board is (n x n). So, board can be a n x n 2D array. 
 * @param sol A sequence (e.g., vector<int>). It represents a partial solution where sol[i] is the column of the queen in row i. The size of sol is less than or equal to n. For example, when sol.size() == 3, it means the first 3 rows have queens successfully placed and the positions of the three queens are recorded in sol. If sol.size() == n, it means sol is already a complete solution. 
 * @return true, if a complete solution is found and stored in 'sol', false otherwise.
 * @note The type of board can be Board as declared in boardGame.hpp, as long as it can support 2D indexing, it is fine, the algorithm will work correctly with some tiny modifications.
 * @note sol can be designed with other type, like a simple array of int with size n, and adding another parameter len to indicate how many rows of the solution is found so far. like:
 * @note bool find_one_solution(Char ** board, int n, int * sol, int len);
 */
bool find_one_solution(char ** board, int n, std::vector<int> & sol) {
    // ... implementation ...
}

/*
<Algorithm Explanation>
number convention: We describe a row number from 1 to n, while the array index of the row is from 0 to n-1. So, for example, when we say row r, it means the row with array index r-1. Similarly, for column c, it means the column with array index c-1.
1.  **Problem Definition**:
    - We are given an n x n chessboard with some queen stones possibly already placed. These placed stones are fixed and cannot be moved. 
    - We have a partial solution 'sol', where sol[i] is the column of the queen in row i+1. The size of 'sol' is less than or equal to n.
    - The goal is to extend 'sol' to a complete solution (size n) that respects all constraints.

2.  **Base Case**:
    - If the partial solution 'sol' already has n elements (sol.size() == n), we have found a complete solution. Return true.

3.  **Recursive Step**:
    - Determine the next row `r` to place a queen. This is simply sol.size(), as 'sol' contains the queens for rows 0 to r-1.
    - For each position of column `c` in row `r` (from 0 to n-1):
        a. **Check for Fixed Queen**: If board[r][c] is already 'Q', this queen is fixed and must be used.
        b. **Check for Conflicts**: Check if placing a queen at (r, c) would conflict with any existing queen (a stone described in `sol` or fixed on the board).
        c. **Conflict Handling**:
            - If if the queen fixed at (r, c) as described by the`board`,  and there is a conflict , no solution exists. Return false.
            - If no queen is fixed at (r, c), and there is a conflict, skip to the next column，i.e, continue to try (r, c+1). 
        d. **No Conflict**: If placing a queen at (r, c) is safe (no conflict):
            i. **Make Move**: Append integer c to 'sol', remember the original stone (the character) at board[r][c] as oldStone, and mark board[r][c] as 'Q' .
            ii. **Recurse**: Recursively call find_one_solution (with same arguments, where sol is updated) to solve the rest of the board.
            iii. **Check Result of the recursive call**:
                - If the recursive call returns true, a solution is found. Return true.
                - If the recursive call returns false, this path is invalid. **Undo the move**:
                    * Remove 'c' from 'sol'.
                    * Restore board[r][c] to its original character (oldStone). 
                    * If a queen was fixed at (r, c), return false (no solution possible).
                    * Otherwise, continue to the next column.

4.  **Backtracking**:
    - Now, after the loop, all positions in the row r has been tried and none work, return false. (the caller to this function call will backtrack to the previous row upon receiving false).  
*/



/* <program design> 
Designing a derived class of NQueen, called NQueenAlg, has several advantages: 
- NQueenAlg object has a board built, and can be used print the board and show the solution. 
- The method `is_queen_threatened(int row, int col) const` can be directly used for the computation of the algorithm.
- If the function `find_one_solution` still wants to use a char** to represent the board, then design some helper functions for converting a Board to or from a 2D array will be helpful, like:

//array is returned, size is saved in n.  
char ** n_queen_board_to_char_array(const Board &board, in * n); 

Board char_array_to_n_queen_board(char ** arr, int n);

*/

typedef std::vector<int> Seq;
typedef std::vector<std::vector<int>> SeqSeq;

/**
< The algorithm of finding all solutions >
 bool find_all_sols(char **board, int n, Seq &sol, SeqSeq &sols);

 [The idea] 
 It is very similar to the algorithm of find_one_sol, only with the following differences
 - Recursion is changed to calling fd_all_sols()
 - The handling of return cases is changed.
    - When all rows are tried, push back sol into sols, before return true.
 - After trying a position in a row, no matter it is succsssful or not, always resumes back to the status (sol and board) before.
 - During the process of trying each position in a row. Do not return true when a good position is found, wait, until all positions in the rows are tried. If at least one good position is found, return true, otherwise, return false.
    - But, an exceptional case, if a position in the row has a fixed Queen (it is there in the given board), then if the position is good (find a solution including the position), or bad, then return true or false can be done immediately, no need wait. But before return, still need to resume the previous content of board and sol.
 */
bool find_all_sols(char **board, int n, Seq &sol, SeqSeq &sols);

/**
 * @brief Finds all solutions to the N-Queen problem using backtracking.
 *
 * This function extends a partial solution to all possible complete solutions by placing queens
 * row by row, ensuring no two queens attack each other. Pre-placed queens on the board are treated as fixed.
 *
 * @param board A 2D container representing the chessboard (e.g., char** or a class supporting 2D indexing).
 *              'Q' represents a queen; other chars represent empty squares.
 * @param n     The size of the board (n x n).
 * @param sol   A sequence (e.g., vector<int>) representing the current partial solution.
 *              sol[i] is the column of the queen in row i.
 * @param sols  A sequence of solutions (e.g., vector<vector<int>>), where each element is a complete solution.
 * @return      true if at least one solution is found, false otherwise.
 *
 * <Algorithm Explanation>
 * 1. Base Case:
 *    - If sol.size() == n, a complete solution is found. Push sol into sols and return true.
 *
 * 2. Recursive Step:
 *    - Let r = sol.size() (the next row to fill).
 *    - For each column c in row r (from 0 to n-1):
 *        a. If board[r][c] is a fixed queen ('Q'), only try this column.
 *        b. Check if placing a queen at (r, c) conflicts with any existing queens (in sol or fixed).
 *        c. If there is a conflict and a queen is fixed at (r, c), return false immediately.
 *        d. If there is a conflict and no queen is fixed, skip to the next column.
 *        e. If no conflict:
 *            i. Append c to sol, remember the original character at board[r][c], and set board[r][c] = 'Q'.
 *            ii. Recursively call find_all_sols for the next row.
 *            iii. After recursion, remove c from sol and restore board[r][c] to its original value.
 *            iv. If a queen was fixed at (r, c), return the result of the recursive call immediately.
 *    - After all columns are tried, return true if at least one solution was found, otherwise false.
 *
 * 3. Backtracking:
 *    - Always restore sol and board to their previous state after each attempt, regardless of success.
 *    - If a fixed queen is present in the row, only try that column and return immediately after.
 */
bool find_all_sols(char **board, int n, Seq &sol, SeqSeq &sols);



namespace CS111
{
    namespace BOARD_GAME
    {using Seq = std::vector<int>;
    using SeqSeq = std::vector<std::vector<int>>;

    bool find_one_sol(char** board, int n, Seq& sol);
    bool find_all_sols(char** board, int n, Seq& sol, SeqSeq& sols);
        class NQueenAlg : public NQueen
        { // this class has at least two methods:
          // 1) print one solution
          // 2) print all solutions.
          
        // !!!!!!!!!!!!!! < Hmk3 Task 5 > !!!!!!!!!!!!!!
        // Provide the missing code of this file. You can design 
        // this class. Some code outside this class is also possible
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
public:
        explicit NQueenAlg(int size);
        void print_one_solution();
        void print_all_solutions();
    private:
        int size_;

        };
    }
}

#endif // NQUEEN_ALG_HPP    