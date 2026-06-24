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

#include "../Include/utils.hpp"
#include <sstream>
#include <cctype>
#include "../Include/boardGame.hpp"
#include "../Include/boardGameTools.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {

        static constexpr const char *SAVE_MAGIC = "BOARD_GAME_SAVE_V1";

        using namespace CS111::BOARD_GAME; // for convenience, so that we can directly use the types defined in BOARD_GAME namespace, e.g., Stone, without needing to write CS111::BOARD_GAME::Stone.

        using StepType = BoardGame::StepType;

        using GameStage = BoardGame::GameStage;


        // ------------------- implementation of BoardGame member functions -------------------
        // <method 1>  the constructor. already implemented in the header file.

        /* <method 2> prepare is a pure virtual method. Derived class can override it. Here only do:
        - some basic consistency checking, e.g., the number of player names must be equal to the number of players. step history is empty.
        This function is called at the beginning of play(), not at replay().
        */
        void BoardGame::prepare()
        {
            // check the consistency of the initial game state, e.g., the number of player names must be equal to the number of players. step history is empty.
            if (currentGameState_.playerNames_.size() != currentGameState_.playerNumber_)
            {
                throw std::runtime_error("Inconsistent initial game state: the number of player names must be equal to the number of players.");
            }
            if (!currentGameState_.stepHistory_.empty())
            {
                throw std::runtime_error("Inconsistent initial game state: step history must be empty at the beginning of the game.");
            }
            // if game stage is set to NOT_STARTED,  then change it to IN_PROGRESS, because the game is now ready to start.
            if (currentGameState_.gameStage_ == GameStage::NOT_STARTED)
            {
                currentGameState_.gameStage_ = GameStage::IN_PROGRESS;
            }

            // if the gameStage is  FINISHED, then it is not ready to start, throw an exception.
            if (currentGameState_.gameStage_ == GameStage::FINISHED)
            {
                throw std::runtime_error("The game stage is FINISHED, the game is not ready to start. Please check the initial game state.");
            }

            // if a winner or a loser is already set in the initial game state, then it is not ready to start, throw an exception.
            if (currentGameState_.winnerPlayerIndex_ >= 0)
            {
                throw std::runtime_error("A winner is already set in the initial game state, the game is not ready to start. Please check the initial game state.");
            }
            if (currentGameState_.loserPlayerIndex_ >= 0)
            {
                throw std::runtime_error("A loser is already set in the initial game state, the game is not ready to start. Please check the initial game state.");
            }

            // greeting:
            std::cout << "Welcome to play " << currentGameState_.gameName_ << "!" << std::endl;
            // show players, and whose turn it is to play first.
            std::cout << "Number of players: " << currentGameState_.playerNumber_ << std::endl;
            std::cout << "Player names: ";
            // int j = currentGameState_.currentPlayerIndex_; // this is the index of the player who will play first, it is set in the initial game state.
            int j = 1;
            for (const auto &name : currentGameState_.playerNames_)
            {
                // show name of player j
                std::cout << "Player " << j << ": " << name << "  ;  ";
                // j = (j + 1) % currentGameState_.playerNumber_;
                j++;
            }
            std::cout << std::endl;
            // since the player index is 1 based, while the array index is always 0 based, need to do the minus 1.
            std::cout << "First player to play: " << currentGameState_.playerNames_[currentGameState_.currentPlayerIndex_ - 1] << std::endl;
        }

        /* <method 3> get_step_from_player. This function does not do validity checking. Only return a step according to player's input. Possibly an invalid step is returned, whose validity will be checked by other functions.
        This function can be overridden by derived classes for specific games.
        The type and player information should be set in the parameter s.
        This function is used when interact with a player, and accepting input from terminal. There is another function read_step_info() which is used when reading step information from a file. The format of a step information on a file is a bit different from the format of a step information input from terminal. For user's input, we emphasize simplicity, while input from a file can have more decorations for structure clarity.

        For example for a move step, the user can input "5 D 6 E", while in a file, the step information is expected to be like "Player 1 : MOVE from (5, 4) to (6, 6)". The read_step_info() function will parse the step information from the file, and then call get_step_from_player() to get the step information in a structured way.
        */

        /*  [..... technical note ......]
        Be careful, for a step whose stone position is (5, 7), that is row 5 and column 7, when put the stone on board by the set_stone function, the parameters should be set_stone(7, 5, stone), because its first parameter is column number, and the second parameter is row number. This is a common source of confusion and error. When a program is built in different stages, history code connect with new code,  style compatibility can demand attention, like the order of parameters of functions.
        */

        BoardGame::Step &BoardGame::get_step_from_player(Step &s) const
        {
            switch (s.type)
            {
                // for a move step, we need to ask the player to input the from position and the to position. For example, "5 D 6 E" means move a stone from (5, D) to (6, E). We need to parse the column token D and E into column numbers.
            case StepType::MOVE_STONE:
            {
                std::cout << "Please enter the step information for MOVE_STONE (from_row from_col to_row to_col, col can be number or letter): ";
                const std::string line = extract_line(std::cin);
                std::istringstream iss(line);
                int fromRow = 0;
                int toRow = 0;
                std::string fromColToken;
                std::string toColToken;
                if (!(iss >> fromRow >> fromColToken >> toRow >> toColToken))
                {
                    s.from = {0, 0};
                    s.to = {0, 0};
                    break;
                }

                try
                {
                    s.from.row = fromRow;
                    s.from.col = parse_column_token_or_throw(fromColToken);
                    s.to.row = toRow;
                    s.to.col = parse_column_token_or_throw(toColToken);
                }
                catch (...)
                {
                    s.from = {0, 0};
                    s.to = {0, 0};
                }
                break;
            }
            case StepType::PUT_STONE:
            {
                std::cout << "Please enter the step information for PUT_STONE (row col, col can be number or letter): ";
                const std::string line = extract_line(std::cin);
                std::istringstream iss(line);
                int row = 0;
                std::string colToken;
                if (!(iss >> row >> colToken))
                {
                    s.to = {0, 0};
                    break;
                }

                try
                {
                    s.to.row = row;
                    s.to.col = parse_column_token_or_throw(colToken);
                }
                catch (...)
                {
                    s.to = {0, 0};
                }
                break;
            }
            default:
                std::cerr << "Other type and Unknown step type are not handled." << std::endl;
                throw std::runtime_error("Other type and Unknown step type are not handled in get_step_from_player. Please override this function in the derived class to handle the OTHER step type if needed.");
                break;
            }
            return s;
        }

        /* <method 4> is_step_valid. This is a pure virtual method. Derived class must override it. Not provided here */

        // <method 5> print the a step information. The printing can show on terminal, or saved in file. So, the text format will be consistent to what appear in a save file. For example, "Player 1: PUT "Black Circle" (5, 4)" or "Player 2: MOVE from (5, 4) to (6, 6)". Note that the column number is not converted to a letter when printing. For example, column 4 is not printed as D, column 5 is not printed as E, etc. The read_step_info() function can parse the step information in the file, and then call this function to print the step information in a consistent way. The derived class can override this function to print more detailed information for the OTHER step type if needed. Whether printing the newline will be decided by the caller.
        //
        void BoardGame::print_step_info(const Step &step, std::ostream &out) const
        {
            //  Display step information
            out << "Player " << step.player << " : ";
            if (step.type == StepType::PUT_STONE)
            { // space is put aside ( , and ), for easier parsing and reading.
                out << "PUT \"" << stone_name(step.stone) << "\" ( " << (step.to.row) << " , " << (step.to.col) << " ) ";
            }
            else if (step.type == StepType::MOVE_STONE)
            // Example: "MOVE from (5, 4) to (6, 6)". Note that the column number is not converted to a letter when printing. For example, column 4 is not printed as D, column 5 is not printed as E, etc.
            {
                out << "MOVE from ( " << (step.from.row) << " , " << (step.from.col) << " ) to ( " << (step.to.row) << " , " << (step.to.col) << " ) ";
            }
            else if (step.type == StepType::OTHER) // derived class can handle this by overidding this function.
            {
                out << "OTHER STEP INFORMATION";
            }
            // whether printing the newline will be decided by the caller.
            // out << std::endl;
        }

        /* for an input stream (aiming a saved file),  read the step from it.
        The text format in the input is expected to be like:
        "Player <player_index> : PUT "<stone_name>" ( <row> , <col> )"
        e.g. "Player 1 : PUT "Black Circle" ( 5 , D )"
        or
        "Player <player_index> : MOVE from ( <from_row> , <from_col> ) to ( <to_row> , <to_col> )"
        e.g. "Player 2 : MOVE from ( 5 , D ) to ( 6 , E )"
        */
        bool BoardGame::read_step_info(std::istream &in, Step &step) const
        {
            const std::string line = extract_line(in);
            const std::string prefix = "Player ";
            // x.rfind(y, n) will search from right to left in string x the appearance of string y, starting from index n (y's leftmost character position). if not found at index n, then check index n-1, then n-1, till index 0.  If n is 0, it means searching from the beginning of the string. So, x.rfind(y, 0) == 0 means that string x starts with string y.
            if (line.rfind(prefix, 0) != 0)
            {
                return false;
            }

            const size_t colonPos = line.find(':');
            if (colonPos == std::string::npos)
            {
                return false;
            }

            int playerIndex = -1;
            {
                // line.substr
                //  x.substr(a, b) returns a new string that is the substring of x, starting at index a and having length b.
                //  Find the substring after "Player " and before the colon, and parse it into an integer, build a istringstream for parsing,and read an integer from it. If parsing fails, return false.
                std::istringstream pidStream(trim(line.substr(prefix.size(), colonPos - prefix.size())));
                if (!(pidStream >> playerIndex))
                {
                    return false;
                }
            }

            // step.player = playerIndex - 1;
            step.player = playerIndex; // since player index is 1-based, we do not need to do the minus 1.

            // take the substring after the colon as the step description, and trim the leading and trailing whitespace.
            const std::string desc = trim(line.substr(colonPos + 1));

            // handle the part like `PUT "Black Circle" ( 5 , 4 )`.
            if (desc.rfind("PUT ", 0) == 0)
            {
                const size_t q1 = desc.find('"');
                const size_t q2 = (q1 == std::string::npos) ? std::string::npos : desc.find('"', q1 + 1);
                if (q1 == std::string::npos || q2 == std::string::npos)
                {
                    return false;
                }

                const std::string stoneName = desc.substr(q1 + 1, q2 - q1 - 1);
                const std::string posText = trim(desc.substr(q2 + 1));

                step.type = StepType::PUT_STONE;
                // a stone is determined by its symbol, symbol color, background color, and shape. For simplicity, we only use the symbol to determine the stone, and set the other attributes to some default values. The symbol is determined by the name of the stone. We can find the symbol from the name by looking up in the symbols array defined in stones.hpp. If not found, we can use a default unknown symbol.
                step.stone = Stone{symbol_from_name(stoneName), Color::BLACK, Color::WHITE, Shape::CIRCLE};

                step.to = parse_position_tokens_or_throw(posText);
                return true;
            }

            // the rfind("MOVE ", 0) method checks if the string starts with "MOVE "
            if (desc.rfind("MOVE ", 0) == 0)
            {
                step.type = StepType::MOVE_STONE;

                if (desc.find(" from ") != std::string::npos && desc.find(" to ") != std::string::npos)
                {
                    const size_t fromPos = desc.find(" from ");
                    const size_t toPos = desc.find(" to ");
                    if (fromPos == std::string::npos || toPos == std::string::npos || toPos <= fromPos + 6)
                    {
                        return false;
                    }
                    const std::string fromText = trim(desc.substr(fromPos + 6, toPos - (fromPos + 6)));
                    const std::string toText = trim(desc.substr(toPos + 4));
                    step.from = parse_position_tokens_or_throw(fromText);
                    step.to = parse_position_tokens_or_throw(toText);
                    return true;
                }

                /*  // the arrow format is not used , --> is replaced by "to" in the updated step format design.
                const std::string moveBody = trim(desc.substr(5));
                const size_t arrowPos = moveBody.find("--->");
                if (arrowPos == std::string::npos)
                {
                    return false;
                }
                const std::string fromText = trim(moveBody.substr(0, arrowPos));
                const std::string toText = trim(moveBody.substr(arrowPos + 4));
                step.from = parse_position_tokens_or_throw(fromText);
                step.to = parse_position_tokens_or_throw(toText);
                */
                return true;
            }

            step.type = StepType::OTHER;
            return desc == "OTHER STEP INFORMATION";
        }

        // <method 6> do a step and update the game state. This is a pure virtual method. Derived class must override it. Some game-specific logic is needed. Not provided here.

        // <method 7> handle abnormal step. Before calling this function the step is already known to be in valid. Here a definition is provided assuming there are two players.  Derived class cam override it.
        // Let printing the winner action done by the end() function, not here. Here only set the winner/loser index, and change the game stage to FINISHED.
        void BoardGame::handle_abnormal_step(const Step &step)
        {
            // if the step is a resigning step, then end the game and declare the other player as the winner.
            if (step.to.row == RESIGN_POSITION.row && step.to.col == RESIGN_POSITION.col)
            {
                // std::cout << "Player " << step.player + 1 << " resigns!" << std::endl;
                std::cout << "Player " << currentGameState_.playerNames_[step.player - 1] << " resigns!" << std::endl;
                goto SET_PLAYER_AS_LOSER_AND_END;
            }
            // if the step is invalid, increase the failed step count for the current player, and check if the max failed steps is reached. If max failed steps reached, not quiting the game, then current player loses.
            // currentGameState_.failedStepsCount_[step.player]++;
            currentGameState_.failedStepsCount_[step.player - 1]++; // since player index is 1-based, we need to do the minus 1 when accessing the array.
            std::cout << "Invalid step! Player " << step.player << " has " << currentGameState_.failedStepsCount_[step.player - 1] << " failed steps." << std::endl;
            if (currentGameState_.failedStepsCount_[step.player - 1] >= MAX_FAILED_STEPS)
            {
                std::cout << "Player " << step.player << " (" << currentGameState_.playerNames_[step.player - 1] << ")  has reached the maximum number of failed steps and loses the game!" << std::endl;
                goto SET_PLAYER_AS_LOSER_AND_END;
            }
            else
            {
                std::cout << "Player " << step.player << " (" << currentGameState_.playerNames_[step.player - 1] << ")  has " << MAX_FAILED_STEPS - currentGameState_.failedStepsCount_[step.player - 1] << " failed steps remaining, and can continue to play." << std::endl;
                return ; // !!! bug fixed, have to return here, Otherwise, the function will set the stage as FINISHED. 
            }
            // do not change the current player. so that in the loop of the play(), the player has a chance to provide the step information.
        SET_PLAYER_AS_LOSER_AND_END:
            // set the loser player index, and end the game.
            // currentGameState_.loserPlayerIndex_ = step.player;
            // currentGameState_.gameStage_ = GameStage::FINISHED;
            // set the loser player index.
            currentGameState_.loserPlayerIndex_ = step.player; // the resigning player is the loser.

            // if there is only two players, the other player wins.
            if (currentGameState_.playerNumber_ == 2)
            {
                currentGameState_.winnerPlayerIndex_ = (step.player % currentGameState_.playerNumber_) + 1; // the other player wins.
            }
            // else, more than 2 players, all the other players win.
            else
            {
                currentGameState_.winnerPlayerIndex_ = 0; // 0 means more than one winner, in this case, all other players except the resigning player are winners.
            }

            currentGameState_.gameStage_ = GameStage::FINISHED; // end the game.
            return;
        }

        // <method 8> end the game. This is a pure virtual method. Derived class must override it. Not provided here.
        void BoardGame::end()
        {
            // by default, just set the game stage to FINISHED, and print a message. Derived classes can override this function to provide more specific logic for ending the game, e.g., determining the winner, displaying the final board state, etc.
            currentGameState_.gameStage_ = GameStage::FINISHED; // this step is redundant, since before calling end(), the stage of the game should be FINISHED alread.

            // some some separator and visual effect to indicate the game is over.
            std::cout << " ~~~~~~~~~~~~~~~ Game over! ~~~~~~~~~~~~~~~~" << std::endl;
            // print the winner

            // since player index is 1-based, the winner player index should also be 1-based, and should be less than playerNumber.  
            int winnerIndex = currentGameState_.winnerPlayerIndex_;
            int loserIndex = currentGameState_.loserPlayerIndex_;
            
            // handle all the abnormal cases of winner and loser first, then handle the normal case. 
            // abnormal cases, throw different exceptions and messages. 

            // both winner and loser must be set, cannot unknown. 
            if(winnerIndex == -1 || loserIndex == -1 )
            {
                std::cerr << "Warning: winner or loser is unknown. Please check the game state." << std::endl;
                throw std::runtime_error("Warning: winner or loser is unknown. Please check the game state."); 
            }

            if(winnerIndex > currentGameState_.playerNumber_ || winnerIndex < -1)
            {
                std::cerr << "Warning: the winner player index is out of range. Please check the game state." << std::endl;
                throw std::runtime_error("The winner player index is out of range. Please check the game state."); 
            }

            if(loserIndex > currentGameState_.playerNumber_ || loserIndex < -1)
            {
                std::cerr << "Warning: the loser player index is out of range. Please check the game state." << std::endl;
                throw std::runtime_error("The loser player index is out of range. Please check the game state."); 
            }

            // if more than one winner and also more than one loser, it is wrong.
            if (winnerIndex == 0 && loserIndex == 0)
            {
                std::cerr << "Warning: more than one winner and more than one loser. Please check the game state." << std::endl;
                throw std::runtime_error("More than one winner and more than one loser. Please check the game state."); 
            }

            // if loser and winner are the same person, it is wrong.
            if (winnerIndex >= 1 && loserIndex >= 1 && winnerIndex == loserIndex)
            {
                std::cerr << "Warning: the winner and loser are the same player. Please check the game state." << std::endl;
                throw std::runtime_error("The winner and loser are the same player. Please check the game state."); 
            }

            // -------------  the normal cases --------------------

            if (winnerIndex >= 1 ) // a single winner 
            {
                std::cout << "The winner is: " << currentGameState_.playerNames_[winnerIndex - 1] << std::endl;  
                if(loserIndex == 0) 
                    std::cout << "All the other players" << " lose." << std::endl; // if there is a single winner, but no loser, then all the other players except the winner lose. This can happen when there are more than 2 players, and the winner is determined by some winning condition, rather than by other players' resigning or reaching max failed steps.        
                else if (loserIndex >= 1)
                {
                    std::cout << "The loser is: " << currentGameState_.playerNames_[loserIndex - 1] << std::endl;  
                }
            }
            else  // if(winnerIndex == 0) // more than one winner, in this case, all other players except the loser are winners.
            {
                // print the single loser. 
                // if(loserIndex >= 1) // should be the case. 
                {
                    std::cout << "The loser is: " << currentGameState_.playerNames_[loserIndex - 1] << std::endl;  
                    std::cout << "All the other players win." << std::endl; 
                }
            }
             
            // some some separator and visual effect to indicate the game is over.
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        }

        /* <method 9> play the game.  */
        void BoardGame::play()
        {
            try
            {
                prepare();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error in preparing the game: " << e.what() << std::endl;
                return; // exit the play function if preparation fails, as the game cannot be played with an inconsistent initial state.
            }

            // display the board, and ask the current player to make a step.
            std::cout << "Current board:" << std::endl;
            
            // void display_board(std::ostream &os, MarkStyle ms, StonePositionStyle sps, GridStyle gs) const;
            // board.display_board(std::cout, mark_style, board.get_stone_position_style(), grid_style);

            // to adjust the printing effect, can assign some default styles for mark_style, stone_position_style, and grid_style. For example, we can use MarkStyle::ARROW, StonePositionStyle::CENTER, GridStyle::SOLID.
            
            currentGameState_.board_.display_board(std::cout); // assuming the Board class has a display() member function to print the board state to the console.

            // the main game loop, which continues until the game stage is FINISHED.
            try // it is better to use try and catch for the loop of running steps. This can help debugging.
            {

                while (currentGameState_.gameStage_ != GameStage::FINISHED)
                {
                    // check if a step can be made
                    if (check_and_handle_no_more_step())
                    {
                        std::cout << "No more valid steps can be made. The game ends in a draw." << std::endl;
                        currentGameState_.gameStage_ = GameStage::FINISHED;
                        break;
                    }

                    // get the current player index, and the current player name.
                    int currentPlayerIndex = currentGameState_.currentPlayerIndex_;
                    // std::string currentPlayerName = currentGameState_.playerNames_[currentPlayerIndex];
                    std::string currentPlayerName = currentGameState_.playerNames_[currentPlayerIndex - 1];
                    // since player index is 1-based, we need to do the minus 1 when accessing the array.

                    // show some visual effect to indicate it is the current player's turn, e.g., print a message.
                    std::cout << "----------------------------------------" << std::endl;
                    std::cout << "It is " << currentPlayerName << "'s turn to play." << std::endl;

                    // get a step from the player, and check if it is valid. If it is not valid, ask the player to try again, until a valid step is made.
                    // prepare the step before calling the get_step function.
                    Step step;
                    step.player = currentPlayerIndex;        // set the player index in the step, so that the step can be properly handled by other functions. The type and other information of the step will be set in get_step_from_player() function.
                    step.type = initialGameState_.stepType_; //  assume each game has only one fixed step type. This can be changed by an overriden play function in a derived class.

                    // since each player only playes one kind of stone (for go, gobang, 8queen), the following two line  can be helpful. But for other games, like chess and xiangqi,  the stone for a step will be set differently.
                    // auto stones = currentGameState_.board_.get_stones_catalog();
                    // step.stone = stones.getStone(step.player-1);

                    bool validStep = false;

                    step = get_step_from_player(step); // get a step from the player, this is a pure virtual function, which will be implemented by derived classes for specific games.

                    // no matter the step is valid or not, it will be added into the step history. This is done before other processing of the step.
                    currentGameState_.stepHistory_.push_back(step);

                    if (is_step_valid(step))
                    {
                        validStep = true;
                    }
                    else
                    {
                        // handled by the function handle_abnormal_step(). Maybe a player quits, maybe some invalid step. Maybe the player has one more chance to input a step. The stage of the game can be changed.
                        handle_abnormal_step(step);
                        continue;
                    }

                    // now the step is valid, do the step and update the game state. This is a pure virtual function, which will be implemented by derived classes for specific games.
                    // this function natually pause the game, wait for user's input.
                    do_step_and_update(step);

                    // print the updated board.
                    std::cout << "Board after the step:" << std::endl;
                    currentGameState_.board_.display_board(std::cout); // assuming the Board class has

                } // check if the game is over after doing the step
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error during playing the game at some step: " << e.what() << std::endl;
            }

            end(); // do the actions to end the game, e.g., print the winner, display the final board state, etc.
        }

        // <method 10> replay the game. When `quiet` is true. do not dislay the board after each step. 
        
        /**
         * @brief Replay the game by applying the history of steps from the initial game state. When `quiet` is true, do not display the board after each step, just update the internal state. When `quiet` is false, display the board after each step, so that we can visually see how the game evolves step by step. Comparing to the play() function, the major difference is that no need to ask the user for input of a step. All the steps are known. 
         * @note Since the function do_step_and_update(step) assumes that the step is just appended to the step history, the step history of the currentGameState_ is handled as follows. Before replaying any step, copy the step history of the currentGameState_ to a vector, call it v. Then, make the step history of current game state empty. Then, repeat: take a step from v and append it to the step history of current game state, and then handle that step, just like the play() function. 
         * 
         * @param quiet when true, display the board after doing each step. When false, do not display the board after doing each step.
         */
        void BoardGame::replay(bool quiet)
        {
            // !!!!!!!!!!!!!! < Hmk3 Task 2 > !!!!!!!!!!!!!!
            // Implement the function according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    reset_to_initial_state();
    auto& snap = currentGameState_;

    if(!quiet)
    {
        std::cout << "\n===== Replay Start =====\n";
        std::cout << "Game Name: " << snap.gameName_ << "\n";
        std::cout << "Total steps to replay: " << snap.stepHistory_.size() << "\n";
        snap.board_.display_board(std::cout);
        pause_for_enter("Press Enter to start replay step by step...");
    }

  
    for(const Step& st : snap.stepHistory_)
    {
        if(!quiet)
        {
            std::cout << "\n-------- Next Step --------\n";
            print_step_info(st, std::cout);
            std::cout << "\n";
        }

   
        if(is_step_valid(st))
            do_step_and_update(st);
        else
            handle_abnormal_step(st);

    
        if(!quiet)
        {
            std::cout << "Board after step:\n";
            snap.board_.display_board(std::cout);
            pause_for_enter("Press Enter for next step...");
        }

    
        if(snap.gameStage_ == GameStage::FINISHED) break;
    }


    if(!quiet)
    {
        std::cout << "\n===== Replay Complete =====\n";
        end();
        pause_for_enter("Replay finished, press Enter to exit replay...");
    }
}

        // end of replay function.

        // <method 11> the destructor, which is already defined as the default version, as described in the header file.

        // <method 12> This function is designed according to the example saved files mentioned above.
        /* How to save a step of type OTHER to a file? There are two ways
        a) Let it has a parameter, a function to output a step of OTHER type, which is game-specific and cannot be determined in the base class.
        Then, the prototype of this function can be like:
        void BoardGame::save_game_to_file(const std::string &filename, std::function<void(const Step&, std::ostream&)> print_other_step_func) const;

        b) The derived class can provide its own overriding function to write game to file that can output the OTHER type step, and pass it as an argument when calling this function. This is a common technique to handle game-specific logic.

        For simplicity, we choose the way b) here, and there is no spactial function
        */
        void BoardGame::save_game_to_file(const std::string &filename) const
        {
            std::ofstream out(filename);
            if (!out)
            {
                throw std::runtime_error("Cannot open save file for writing: " + filename);
            }

            // make   short names for currentGameState_ and initialGameState_,
            const auto &current = currentGameState_;
            const auto &initial = initialGameState_;

            // ===Initial Game Info===
            out << "===Initial Game Info===" << std::endl;
            out << "GAME_NAME: \"" << initial.gameName_ << "\"" << std::endl;
            out << "STEP_TYPE: " << step_type_to_text(initial.stepType_) << std::endl;
            out << "GAME_STAGE: " << game_stage_to_text(initial.gameStage_) << std::endl;
            out << "BOARD_SIZE: " << initial.board_.get_height() /*rows*/ << " " << initial.board_.get_width() /*columns*/ << std::endl;

            out << "PLAYER_COUNT: " << initial.playerNumber_ << std::endl;
            // out << "CURRENT_PLAYER_ID: " << initial.currentPlayerIndex_ + 1 << std::endl; // +1 to make it 1-based index for better user-friendliness in the saved file.
            out << "CURRENT_PLAYER_ID: " << initial.currentPlayerIndex_ << std::endl; // since player index is 1-based, we do not need to do + 1.
            out << "WINNER_PLAYER_ID: " << initial.winnerPlayerIndex_ << std::endl;   // since player index is 1-based, we do not need to do the minus 1.

            // ---Stone Set---
            out << "---Stone Set---" << std::endl;
            auto stones = initial.board_.get_stones_catalog();
            int stoneNum = stones.getStoneNum();
            out << "STONE_COUNT: " << stoneNum << std::endl;
            for (int j = 0; j < stoneNum; ++j)
            {
                out << "\"" << stone_name(stones.getStone(j)) << "\"" << std::endl;
                // can find a way to write the stone symbols. But saving the stone names will be sufficient, since when load the game from file, from the stone names we can find the corresponding stone symbols.
            }

            // ---Player Names---
            out << "---Player IDs and Names---" << std::endl;
            // out << "PLAYER_ID_AND_NAME:" << std::endl;
            for (size_t i = 1; i <= initial.playerNames_.size(); ++i)
            {
                out << i << ": \"" << initial.playerNames_[i - 1] << "\"" << std::endl;
            }

            // ---Initial Stones---
            out << "---Initial Stones---" << std::endl;

            // std::tuple can have more than three elements. But here we only need three.

            // Only save non-empty stones
            std::vector<std::tuple<int, int, std::string>> initialStones;

            // Note that the Board::get_stone(r, c) function work with r and c as 1 based, for convenience of the user's understanding of the row position and column position.  It will internally convert to r-1 and c-1

            // bug fixed:  for (int r = 0; r < initial.board_.get_height(); ++r)
            for (int r = 1; r <= initial.board_.get_height(); ++r)
            {
                for (int c = 1; c <= initial.board_.get_width(); ++c)
                {
                    const auto &stone = initial.board_.get_stone(r, c);
                    if (!is_empty_stone(stone))
                    {
                        // the emplace_back function will create an element using its arguments, and put it at the end of vector. Unlike push_back, which need something already created.
                        // r, c, and stone name are used to make a tuple, which saved in the initialStones vector.
                        initialStones.emplace_back(r, c, stone_name(stone));
                        // positions saved are 1 based.  user-friendliness in the saved file.
                    }
                }
            }
            out << "INITIAL_STONE_COUNT: " << initialStones.size() << std::endl;
            /* the initial stones on the board are saved like:
            1: ( 5 , 4 ) "Black Circle"
            2: ( 6 , 6 ) "White Queen"
            */

            for (size_t i = 0; i < initialStones.size(); ++i)
            {
                // out << (i + 1) << ": (" << std::get<0>(initialStones[i]) << ", " << std::get<1>(initialStones[i]) << ") \"" << std::get<2>(initialStones[i]) << "\"" << std::endl;
                // add space besides ( ) and , for easier parsing and reading.
                out << (i + 1) << ": ( " << std::get<0>(initialStones[i]) << " , " << std::get<1>(initialStones[i]) << " ) \"" << std::get<2>(initialStones[i]) << "\"" << std::endl;
            }

            // ===Step History===
            out << "===Step History===" << std::endl;
            out << "STEP_COUNT: " << current.stepHistory_.size() << std::endl;
            for (size_t i = 1; i <= current.stepHistory_.size(); ++i)
            {
                const auto &step = current.stepHistory_[i - 1];
                out << i << ". ";
                print_step_info(step, out);
                out << std::endl;
            }

            out << "===End of Game Info===" << std::endl;
        }

        // <method 13>
        void BoardGame::load_game_from_file(const std::string &filename)
        {
            auto &current = currentGameState_;
            auto &initial = initialGameState_;

            std::ifstream in(filename);
            if (!in)
            {
                throw std::runtime_error("Cannot open save file for reading: " + filename);
            }

            /* .....[technical note].....
            Several lambda functions are defined. They enhance the tool functions defined in utils.cpp. They can throw exceptions, and combing some actions together.
            */

            auto require_token = [&](const std::string &tok)
            {
                if (!extract_required_token(in, tok))
                {
                    throw std::runtime_error("Expected token '" + tok + "' in save file.");
                }
            };

            // require a specific text in the input stream, and throw an exception if it is not found. This is useful for checking the structure of the save file, e.g., to ensure that certain sections are present and properly labeled. When sucessful the reading position in the input stream will be after the required text.
            auto require_text = [&](const std::string &txt)
            {
                if (!reach_non_whitespace(in) || !extract_required_text(in, txt))
                {
                    throw std::runtime_error("Expected text '" + txt + "' in save file.");
                }
            };

            // unquote a quoted text, and check if it is properly quoted. If not, throw an exception. The field parameter is used for error message.
            auto unquote = [&](const std::string &raw, const std::string &field) -> std::string
            {
                const std::string text = trim(raw);
                if (text.size() < 2 || text.front() != '"' || text.back() != '"')
                {
                    throw std::runtime_error("Expected quoted text for " + field + ", got '" + text + "'.");
                }
                return text.substr(1, text.size() - 2); // substr takes the starting index and length of the substring.
            };

            // ===Initial Game Info===
            require_text("===Initial Game Info===");

            require_token("GAME_NAME:");
            initial.gameName_ = unquote(extract_line(in), "GAME_NAME");

            require_token("STEP_TYPE:");
            std::string stepTypeText;
            in >> stepTypeText;
            initial.stepType_ = step_type_from_text(stepTypeText);

            require_token("GAME_STAGE:");
            std::string gameStageText;
            in >> gameStageText;
            initial.gameStage_ = game_stage_from_text(gameStageText);

            require_token("BOARD_SIZE:");
            int boardHeight = 0;
            int boardWidth = 0;
            in >> boardHeight >> boardWidth;
            initial.board_ = Board(boardWidth, boardHeight); // the defult print style is gridStyle.

            require_token("PLAYER_COUNT:");
            in >> initial.playerNumber_;
            require_token("CURRENT_PLAYER_ID:");
            in >> initial.currentPlayerIndex_;
            // initial.currentPlayerIndex_ -= 1; // since player index is 1-based, we do not need to do the minus 1.
            require_token("WINNER_PLAYER_ID:");
            in >> initial.winnerPlayerIndex_;
            // initial.winnerPlayerIndex_ -= 1;

            // ---Stone Set---
            require_text("---Stone Set---");
            require_token("STONE_COUNT:");
            int stoneNum = 0;
            in >> stoneNum;

            // stoneBuffer is a vector initially filled with empty stones, and its size is stoneNum.
            std::vector<Stone> stoneBuffer(static_cast<size_t>(stoneNum), get_empty_stone());

            Stones stonesCatalog(stoneNum, stoneBuffer.data());
            // the .data() function of vector returns a pointer to the underlying array, which can be used to initialize the Stones object. This way we can avoid dynamic memory allocation and manual memory management for the stones catalog. The stoneBuffer vector will manage the memory for the stones, and we can access and modify the stones through the stonesCatalog object.

            for (int j = 0; j < stoneNum; ++j)
            {
                const std::string stoneName = unquote(extract_line(in), "stone name");
                Symbol sym = symbol_from_name(stoneName);
                stonesCatalog.setStone(j, sym, Color::BLACK, Color::WHITE, Shape::CIRCLE);
            }

            Board boardWithCatalog(initial.board_.get_width(), initial.board_.get_height(), stonesCatalog);
            initial.board_ = boardWithCatalog;

            std::unordered_map<std::string, Stone> stonesByName;
            for (int j = 0; j < stoneNum; ++j)
            {
                const auto &stone = stonesCatalog.getStone(j);
                stonesByName[stone_name(stone)] = stone;
            }

            // ---Player Names---
            // require_text("---Player Names---");
            require_text("---Player IDs and Names---");

            // if (!extract_required_token(in, "PLAYER_ID_AND_NAME:") && !extract_required_token(in, "PLAYER_ID_AND_NAMES:"))
            // logical and with two identical expressions?
            // if (!extract_required_token(in, "PLAYER_ID_AND_NAME:") && !extract_required_token(in, "PLAYER_ID_AND_NAMES:"))
            // {
            //     throw std::runtime_error("Expected 'PLAYER_ID_AND_NAME:' in save file.");
            // }

            initial.playerNames_.resize(initial.playerNumber_);
            for (int i = 1; i <= initial.playerNumber_; ++i)
            {
                require_token(std::to_string(i) + ":");
                initial.playerNames_[i - 1] = unquote(extract_line(in), "player name");
            }

            // ---Initial Stones---
            require_text("---Initial Stones---");
            require_token("INITIAL_STONE_COUNT:");
            int initialStoneCount = 0;
            in >> initialStoneCount;

            /* The initial Stones in the file looks like:
            1: ( 5 , 4 ) "Black Circle"
            2: ( 6 , 6 ) "White Queen"
            */
            for (int i = 1; i <= initialStoneCount; ++i)
            {
                require_token(std::to_string(i) + ":");
                const std::string rest = extract_line(in);
                const size_t q1 = rest.find('"');
                const size_t q2 = (q1 == std::string::npos) ? std::string::npos : rest.find('"', q1 + 1);
                if (q1 == std::string::npos || q2 == std::string::npos)
                {
                    throw std::runtime_error("Invalid initial stone line: '" + rest + "'.");
                }

                const std::string posText = trim(rest.substr(0, q1));
                const Position pos = parse_position_tokens_or_throw(posText);
                const std::string stoneName = rest.substr(q1 + 1, q2 - q1 - 1); // the stone name is the part between the two quotes.
                const Stone stone = find_stone_by_name_or_throw(stonesByName, stoneName);
                initial.board_.set_stone(pos.col, pos.row, stone);
            }

            current = initial;

            // ===Step History===

            /* this part in the file looks like:
            ===Step History===
            STEP_COUNT: 5
            1. Player 1: PUT "BLACK" (5, 5)
            2. Player 2: PUT "WHITE" (5, 6)
            */

            require_text("===Step History===");
            require_token("STEP_COUNT:");
            int stepCount = 0;
            in >> stepCount;
            current.stepHistory_.resize(stepCount);

            for (int i = 1; i <= stepCount; ++i)
            {
                require_token(std::to_string(i) + ".");
                Step step{};
                if (!read_step_info(in, step))
                {
                    throw std::runtime_error("Invalid step text in save file.");
                }

                if (step.type == StepType::PUT_STONE)
                {
                    step.stone = find_stone_by_name_or_throw(stonesByName, stone_name(step.stone));
                }

                current.stepHistory_[i - 1] = step; // correct, i-1, vector index is 0-based.
            }

            require_text("===End of Game Info===");

            for (const auto &step : current.stepHistory_)
            {
                if (!is_step_valid(step))
                {
                    handle_abnormal_step(step);
                }
                else
                {
                    do_step_and_update(step);
                }
                if (current.gameStage_ == GameStage::FINISHED)
                {
                    break;
                }
            }
        } // end of load_game_from_file()

        // <method 15>  branch_the_current_game, must be implemented by a derived class.

        // <method 16>    simply check is the board is full.
        bool BoardGame::check_and_handle_no_more_step()
        {
            // Default implementation: check if the board is full
            for (int r = 1; r <= currentGameState_.board_.get_height(); ++r)
            {
                for (int c = 1; c <= currentGameState_.board_.get_width(); ++c)
                {
                    if (is_empty_stone(currentGameState_.board_.get_stone(r, c)))
                    {
                        return false; // there is at least one empty position, so a step can be made.
                    }
                }
            }
            currentGameState_.gameStage_ = GameStage::FINISHED; // end the game if there is no more step can be made.
            return true;                                        // no empty positions, so no step can be made.
        }

    } // end of namespace BOARD_GAME

} // end of namespace CS111
