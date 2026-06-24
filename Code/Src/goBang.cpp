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

#include "../Include/goBang.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        GoBang::GoBang(
            int boardSize,
            const std::string gameName,
            const std::vector<std::string> playerNames)
            : GoLike(boardSize, gameName, playerNames) // GoLike will check the validity of go like board. Anything wrong, exception will be thrown.
        {
            // GoBang prefers visible board grid lines during play/replay output.
            initialGameState_.board_.set_grid_style(GridStyle::WithLines);
            currentGameState_.board_.set_grid_style(GridStyle::WithLines);
        }

        // only need to define two methods.

        /* A step is not valid if
        - it is not a put stone step, or
        - the position to put the stone is not empty, or
        - the position to put the stone is out of the board.
        - the player of the step is not the current player.
        */
        bool GoBang::is_step_valid(const Step &step) const
        {
            // a step is valid if it is a put stone step, and the position to put the stone is empty, and the position is within the board.
            if (step.type != StepType::PUT_STONE)
            {
                return false;
            }
            int x = step.to.col;
            int y = step.to.row;
            if (x < 1 || x > currentGameState_.board_.get_width() || y < 1 || y > currentGameState_.board_.get_height())
            {
                return false;
            }
            if (currentGameState_.board_.get_stone(x, y).symbol.symbol != U' ') // the position is not empty, not allowed.
            {
                return false;
            }
            if (step.player != currentGameState_.currentPlayerIndex_)
            {
                return false;
            }
            return true;
        }


        // This function is important. It contains the logic to check if a game has a winner.
        void GoBang::do_step_and_update(const Step &step)
        {
            // since the step is already known to be valid, we can directly do the step and update the game state.
            int x = step.to.col;
            int y = step.to.row;
            Stones stones_catalog = currentGameState_.board_.get_stones_catalog();
            Stone stone_to_put = step.player == 1 ? stones_catalog.getStone(0) : stones_catalog.getStone(1); // player 1 uses the first stone in the catalog, player 2 uses the second stone in the catalog.
            currentGameState_.board_.set_stone(x, y, stone_to_put);
            // set_stone will put the col position x first, then the row position y. 

            // after doing the step, check if the game is over. If so, update the game stage and winner player index.
            if (check_win_condition(x, y, stone_to_put)) // check if the current player wins after putting the stone at (x, y).
            {
                currentGameState_.gameStage_ = GameStage::FINISHED;
                currentGameState_.winnerPlayerIndex_ = step.player;
                currentGameState_.loserPlayerIndex_ = (step.player % currentGameState_.playerNumber_) + 1; // set the loser player index
            }
            else
            {
                // if not win, then switch to the next player.
                currentGameState_.currentPlayerIndex_ = (currentGameState_.currentPlayerIndex_ % currentGameState_.playerNumber_) + 1; 
                // switch to the next player, and make it 1-based index. For example, if there are 2 players, and the current player index is 1, then the next player index will be (1 ) % 2 + 1 = 2. If the current player index is 2, then the next player index will be (2 ) % 2 + 1 = 1. This way, we can switch between player 1 and player 2.
            }
        }

        // 
        // the rule is simple, after puting the stone as position at (x, y), that is row y and column x, if there are 5 or more same stones in a line (horizontally, vertically, or diagonally), including the posiiton (x, y),  then the player wins.

        /**
         * @brief Helper for checking win condition after a move. Returns true if a player putting the stone at (x, y) with the given stone wins the game. Otherwise return false.  
         * 
         * @param x The column position of the move. x is 1 based, ranging from 1 to board width, from left to right.
         * @param y The row position of the move. y is 1 based, ranging from 1 to board height, from top to bottom.
         * @param stone The stone placed at the position (x, y).
         * @note The rule is simple, after putting the stone at position (x, y), that is row y and column x, if there are 5 or more same stones in a line (horizontally, vertically, or diagonally), including the position (x, y), then the player wins.
         * @note The way to tell if a position (i, j) on the board has the same stone can be checked by :
         * @note currentGameState_.board_.get_stone(i, j).symbol.symbol == stone.symbol.symbol
         */
        bool GoBang::check_win_condition(int x, int y, const Stone &stone) const
        {
        // !!!!!!!!!!!!!! < Hmk3 Task 3 > !!!!!!!!!!!!!!
        // Implement the function according to the specification.
 
    const int dir[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    int boardW = currentGameState_.board_.get_width();
    int boardH = currentGameState_.board_.get_height();

    for(int d = 0; d < 4; ++d)
    {
        int count = 1;
        int nx = x + dir[d][0];
        int ny = y + dir[d][1];
        while(nx >= 1 && nx <= boardW && ny >= 1 && ny <= boardH)
        {
            Stone curSt = currentGameState_.board_.get_stone(nx, ny);
            if(curSt.symbol.symbol == stone.symbol.symbol)
                count++;
            else
                break;
            nx += dir[d][0];
            ny += dir[d][1];
        }

        int rx = x - dir[d][0];
        int ry = y - dir[d][1];
        while(rx >= 1 && rx <= boardW && ry >= 1 && ry <= boardH)
        {
            Stone curSt = currentGameState_.board_.get_stone(rx, ry);
            if(curSt.symbol.symbol == stone.symbol.symbol)
                count++;
            else
                break;
            rx -= dir[d][0];
            ry -= dir[d][1];
        }

        if(count >= 5)
            return true;
    }
    return false;
}
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            
         // end of function check_win_condition

        // do not mention `override` at the definition. Only mention it at the declaration in the header file. Otherwise, it cannot compile. 

        std::unique_ptr<BoardGame> GoBang::branch_the_current_game() const   
        { 
            // create a new GoBang game object, and initialize it with the current game state. The initial game state of the new game is the same as the initial game state of the current game, which is stored in currentGameState_. The current game state of the new game is initialized to be the same as the current game state of the current game, which is stored in currentGameState_. Therefore, the new game starts from the current game state, and has the same initial game state as the current game.
            std::unique_ptr<GoBang> new_game = std::make_unique<GoBang>(*this);

            // another way:
            /* std::unique_ptr<GoBang> new_game = std::make_unique<GoBang>(currentGameState_.board_.get_width(), currentGameState_.gameName_, currentGameState_.playerNames_);
            new_game->currentGameState_ = currentGameState_; // copy the current game state to the new game. This will make the new game start from the current game state, and have the same initial game state as the current game.
            */
            
            return new_game; // return a unique pointer to the new game.
    
        }
    }
}