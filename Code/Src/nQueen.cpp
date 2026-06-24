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

// nQueen.cpp
// Implementation of the NQueen game logic (two-player interactive version)
// Consistent with GoBang style: minimal overrides, clear comments, teaching-oriented.

#include "../Include/nQueen.hpp"
#include "../Include/board_presets.hpp" // For queen stones, if needed

namespace CS111
{
    namespace BOARD_GAME
    {

        Stones NQueen::make_queen_stones()
        {
            Stones chessStones = make_western_chess_stones(); // Get the standard chess stones, which include queens.
            Stone *stoneArr = new Stone[3];                   // 2 queen stones + 1 empty stone
            // by convention, the last element in a stones catalog is the empty stone, so we can directly use the empty stone from the chess stones catalog. although it is not a good convention.
            stoneArr[2] = get_empty_stone();       // Assuming get_empty_stone() returns the empty stone from the catalog
            stoneArr[0] = chessStones.getStone(7); // By reading board_presets.cpp, we know the black queen has index 7 in the chess stones catalog..
            stoneArr[1] = chessStones.getStone(1); // By reading board_presets.cpp, we know the white queen has index 1 in the chess stones catalog.
            Stones queenStones(3, stoneArr);
            return queenStones;
        }

        // CS111::BOARD_GAME::Board" has no member "set_stones_catalog". But we do not want to change the Board class. One way to set the stones catalog is to use the constructor of BoardGame, which takes a Stones object as a parameter. Then, to change the stones of an existing board, can assign an expected board to the existing board.

        Board NQueen::make_n_queen_board(int n)
        {
            Stones queenStones = make_queen_stones();
            // Now, remove all the stones in the catalog except the two queen stones (the first two stones in the catalog), and set the rest to empty stone. This way, we can reuse the existing queen stones in the catalog, and we can also ensure that the empty stone is properly set in the catalog, which is important for checking if a square is empty or not.

            Board board(n, n, queenStones); // Create an n x n board with the queen stones catalog. The board constructor will set the stones catalog to the given queenStones, which has only two queen stones and one empty stone.
            board.set_mark_style(MarkStyle::Professional); // NQueen prefers professional style for better readability.
            board.set_grid_style(GridStyle::WithLines); // NQueen prefers grid lines for better visibility of the board.
            board.set_stone_position_style(StonePositionStyle::AtBlank);
            return board;
        }

        NQueen::NQueen(int boardSize, const std::string &gameName, const std::vector<std::string> &playerNames)
            : // BoardGame(boardSize, gameName, playerNames)
              BoardGame(CS111::BOARD_GAME::Board(boardSize, boardSize), gameName, GameStage::NOT_STARTED, 2, playerNames, 1, -1, -1, StepType::PUT_STONE, {})  // 1 is the current player, -1 is the winner (unknown), -1 is the loser (unknown), empty step history.
        {
            // Set up the board and stones for NQueen.
            // Use BoardPreset to get queen stones (white and black) if available.
            // (Assume BoardPreset::make_queen_stones() returns a Stones object with two queen stones.)
            if (boardSize < 1 || boardSize % 2 != 0)
            {
                throw std::invalid_argument("Board size must be a positive even number");
            }
            
            // Set up queen stones (optional, if not already set by BoardGame)
            initialGameState_.board_ = make_n_queen_board(boardSize);
            currentGameState_.board_ = make_n_queen_board(boardSize);
        }

        /* Check if a step is valid. his function does not check if threatened. Because when a step put a stone at a threatened position, the game is over. These conditions must be satisified:
        - the position is within the bounds of the board.
        - the position is empty (an empty stone is there), correct player. position in the board.
        - the player of the step is the current player.
        - the type of the step is PUT_STONE.
        */
        bool NQueen::is_step_valid(const Step &step) const
        {

            if (step.type != StepType::PUT_STONE)
            {
                // debug: print an error message to cerr, which will be helpful for debugging when a test fails.
                std::cerr << "Invalid step type: expected PUT_STONE, got " << static_cast<int>(step.type) << std::endl;
                return false;
            }
            int x = step.to.col;
            int y = step.to.row;
            // Check bounds
            if (x < 1 || x > currentGameState_.board_.get_width() || y < 1 || y > currentGameState_.board_.get_height())
            {
                std::cerr << "Invalid step: position (" << y << ", " << x << ") is out of bounds" << std::endl;
                return false;
            }
            // Check if square is empty
            // if (currentGameState_.board_.get_stone(x, y).symbol.symbol != U' ')
            if (currentGameState_.board_.get_stone(x, y).symbol.symbol != get_empty_stone().symbol.symbol) //
            {
                std::cerr << "Invalid step: expected empty square at (" << y << ", " << x << ")" << std::endl;
                return false;
            }
            // Check if player is correct
            if (step.player != currentGameState_.currentPlayerIndex_)
            {
                std::cerr << "Invalid step: expected player " << currentGameState_.currentPlayerIndex_ << ", got " << step.player << std::endl;
                return false;
            }
            // Do not check if any queen threatens this position
            /*
            if (is_queen_threatened(y, x))
            {
                return false;
            }
            */
            return true;
        }

        // Apply a step, update the board and game state, It is already known that the step is valid.
        // If checks if after step, the game is over or not.
        // Assuming the step is already saved in the history.
        void NQueen::do_step_and_update(const Step &step)
        {
            int x = step.to.col;
            int y = step.to.row;
            Stones stones_catalog = currentGameState_.board_.get_stones_catalog();
            Stone queen_to_put = step.player == 1 ? stones_catalog.getStone(0) : stones_catalog.getStone(1);
            currentGameState_.board_.set_stone(x, y, queen_to_put);

            // if the stone threatens any existing queen, the player loses, and the other player wins. So the game is over.
            if (is_queen_threatened(y, x))
            {
                std::cerr << "Player " << step.player << " loses by placing a queen at (" << y << ", " << x << ") which is threatened by an existing queen." << std::endl;
                currentGameState_.gameStage_ = GameStage::FINISHED;
                currentGameState_.winnerPlayerIndex_ = (step.player % currentGameState_.playerNumber_) + 1; // the other player wins
                currentGameState_.loserPlayerIndex_ = step.player; // the current player loses
                return;
            }

            // Now the stone of the step does not threaten any existing queen. If it is the last stone (Nth) stone, then the player wins.
            // Check how many steps are saved in the history. The step should be last one saved in history.
            int stepsCount = currentGameState_.stepHistory_.size();
            // If the step is the Nth step, then the player wins. N is found by checking the board size.
            int boardSize = currentGameState_.board_.get_width(); // the board is square, so width and height are the same.
            if (stepsCount == boardSize)                          // The last stone is placed, so the player is the winner. This
            {
                std::cout << "Player " << step.player << " wins by placing the last queen at (" << y << ", " << x << ")." << std::endl;
                currentGameState_.gameStage_ = GameStage::FINISHED;
                currentGameState_.winnerPlayerIndex_ = step.player; // the current player wins
                currentGameState_.loserPlayerIndex_ = (step.player % currentGameState_.playerNumber_) + 1; // the other player loses
                return;
            }

            // If the move is valid but not a win, switch to the next player.
            // debug 
            int oldPlayerIndex = currentGameState_.currentPlayerIndex_;
            
            //Bug  (1+1)%2 + 1 = 1. So the player does not change when it is 1. 
            // currentGameState_.currentPlayerIndex_ = (currentGameState_.currentPlayerIndex_ + 1) % currentGameState_.playerNumber_ + 1;
            currentGameState_.currentPlayerIndex_ = (currentGameState_.currentPlayerIndex_ % currentGameState_.playerNumber_) + 1;

            // for debugging. 
            // std::cerr << "Switching from player " << oldPlayerIndex << " to player " << currentGameState_.currentPlayerIndex_ << std::endl; 
        }

        // Helper: Check if placing a queen at (row, col) is threatened by any existing queen on the board. 
        // When there is a queen on the board, that share the same row, column, or diagonal with (row, col), then the position (row, col) is threatened. Note that row and col are 1-based.
        // return true if so, otherwise return false.
        bool NQueen::is_queen_threatened(int row, int col) const
        {
            // !!!!!!!!!!!!!! < Hmk3 Task 4 > !!!!!!!!!!!!!!
            // Implement the function according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int dir[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    int w = currentGameState_.board_.get_width();
    int h = currentGameState_.board_.get_height();

    for(int d = 0; d < 4; d++)
    {
        int nr = row + dir[d][0];
        int nc = col + dir[d][1];
        while(nr >= 1 && nr <= h && nc >= 1 && nc <= w)
        {
            Stone s = currentGameState_.board_.get_stone(nc, nr);
            if(s.symbol.symbol != get_empty_stone().symbol.symbol)
                return true;
            nr += dir[d][0];
            nc += dir[d][1];
        }
    }
    return false;
            
        }

        // Clone the current game state (for branching/replay)
        std::unique_ptr<BoardGame> NQueen::branch_the_current_game() const
        {
            return std::make_unique<NQueen>(*this);
        }

    } // namespace BOARD_GAME
} // namespace CS111
