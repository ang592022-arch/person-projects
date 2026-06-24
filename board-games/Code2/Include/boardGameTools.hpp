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


// This file declares some helper functions to implement Board Games. They are provided openly for possile design and implementation of different board games.

#ifndef BOARD_GAME_TOOLS_HPP
#define BOARD_GAME_TOOLS_HPP

#include "board.hpp"
#include "stones.hpp"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "boardGame.hpp"
// a function used to print the step type, when saving the game to a file.

namespace CS111
{
   
    namespace BOARD_GAME
    {
        using StepType = BoardGame::StepType;
        using GameStage = BoardGame::GameStage;

        const char *step_type_to_text(StepType t);

        // a function used to parse the step type from text, when loading the game from a file.
        StepType step_type_from_text(const std::string &text);

        // A function used to print the game stage, when saving the game to a file.
        const char *game_stage_to_text(GameStage s);

        // A function used to parse the game stage from text, when loading the game from a file.
        GameStage game_stage_from_text(const std::string &text);

        // A helper function to get the name of a stone, used when saving the game to a file.
        std::string stone_name(const Stone &stone);

        // A helper function to find a stone by its name, used when loading the game from a file.  When reading a history of steps, we need to find the stone quickly according to name of the stone in the step description. When this funciton is called, we assume the map from stone name to stone object has already been built.
        Stone find_stone_by_name_or_throw(const std::unordered_map<std::string, Stone> &stonesByName,
                                          const std::string &name);

        // A helper function to check if a stone is an empty stone, used when saving the game to a file. When saving the initial board state, we only save the non-empty stones, so we need to check if a stone is empty or not. Here, we consider a stone to be empty if its symbol is a space character or if its symbol color is Color::EMPTY.
        bool is_empty_stone(const Stone &stone);

        // A helper function to parse a column token (which can be either a number or a letter, a token means a string without whitespace) into a column index, used when getting input from the player. For example, "D" will be parsed into 4, and "5" will be parsed into 5. The column index is 1-based, meaning that "A" corresponds to 1, "B" corresponds to 2, and so on. The function will throw an exception if the token is not a valid column token. Note that -1 or +2 are acceptible column tokens. Whether the column is meaningful will be decided by the game logic.
        // Currently, only terminal input from a player can accept letter as column token. When position is saved to a file, the column is always saved as a number. So when loading from a file, we only need to parse number column token, and letter column token will not appear in the file.

        int parse_column_token_or_throw(const std::string &token);

        // symbols is an array defined in stones.hpp.
        Symbol symbol_from_name(const std::string &name);

        // parse some text  like ( 5 , 4 ).  This function is used when read step information from a file.
        BoardGame::Position parse_position_tokens_or_throw(const std::string &text);

    } // end of namespace BOARD_GAME
}// end of namespace CS111

#endif // BOARD_GAME_TOOLS_HPP