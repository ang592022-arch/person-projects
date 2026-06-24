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
#include "../Include/boardGameTools.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {

        // ---------------- helper functions for saving and loading game state to/from file ----------------

        // a function used to print the step type, when saving the game to a file.
        const char *step_type_to_text(StepType t)
        {
            switch (t)
            {
            case StepType::MOVE_STONE:
                return "MOVE_STONE";
            case StepType::PUT_STONE:
                return "PUT_STONE";
            case StepType::OTHER:
                return "OTHER";
            }
            return "OTHER";
        }

        // a function used to parse the step type from text, when loading the game from a file.
        StepType step_type_from_text(const std::string &text)
        {
            if (text == "MOVE_STONE")
            {
                return StepType::MOVE_STONE;
            }
            if (text == "PUT_STONE")
            {
                return StepType::PUT_STONE;
            }
            if (text == "OTHER")
            {
                return StepType::OTHER;
            }
            throw std::runtime_error("Unknown step type in text file found: " + text);
        }

        // A function used to print the game stage, when saving the game to a file.
        const char *game_stage_to_text(GameStage s)
        {
            switch (s)
            {
            case GameStage::NOT_STARTED:
                return "NOT_STARTED";
            case GameStage::IN_PROGRESS:
                return "IN_PROGRESS";
            case GameStage::FINISHED:
                return "FINISHED";
            }
            return "NOT_STARTED";
        }

        // A function used to parse the game stage from text, when loading the game from a file.
        GameStage game_stage_from_text(const std::string &text)
        {
            if (text == "NOT_STARTED")
            {
                return GameStage::NOT_STARTED;
            }
            if (text == "IN_PROGRESS")
            {
                return GameStage::IN_PROGRESS;
            }
            if (text == "FINISHED")
            {
                return GameStage::FINISHED;
            }
            throw std::runtime_error("Unknown game stage in save file: " + text);
        }

        // A helper function to get the name of a stone, used when saving the game to a file.
        std::string stone_name(const Stone &stone)
        {
            return (stone.symbol.name == nullptr) ? std::string("UNKNOWN") : std::string(stone.symbol.name);
        }

        // A helper function to find a stone by its name, used when loading the game from a file.  When reading a history of steps, we need to find the stone quickly according to name of the stone in the step description. When this funciton is called, we assume the map from stone name to stone object has already been built.
        Stone find_stone_by_name_or_throw(const std::unordered_map<std::string, Stone> &stonesByName,
                                          const std::string &name)
        {
            const auto it = stonesByName.find(name);
            if (it == stonesByName.end())
            {
                throw std::runtime_error("Unknown stone name in saved file: " + name);
            }
            return it->second;
        }

        // A helper function to check if a stone is an empty stone, used when saving the game to a file. When saving the initial board state, we only save the non-empty stones, so we need to check if a stone is empty or not. Here, we consider a stone to be empty if its symbol is a space character or if its symbol color is Color::EMPTY.
        bool is_empty_stone(const Stone &stone)
        {
            return stone.symbol.symbol == U' ' || stone.symbolColor == CS111::BOARD_GAME::Color::EMPTY;
        }

        // A helper function to parse a column token (which can be either a number or a letter, a token means a string without whitespace) into a column index, used when getting input from the player. For example, "D" will be parsed into 4, and "5" will be parsed into 5. The column index is 1-based, meaning that "A" corresponds to 1, "B" corresponds to 2, and so on. The function will throw an exception if the token is not a valid column token. Note that -1 or +2 are acceptible column tokens. Whether the column is meaningful will be decided by the game logic.
        // Currently, only terminal input from a player can accept letter as column token. When position is saved to a file, the column is always saved as a number. So when loading from a file, we only need to parse number column token, and letter column token will not appear in the file.

        int parse_column_token_or_throw(const std::string &token)
        {
            if (token.empty())
            {
                throw std::runtime_error("Empty column token.");
            }

            bool numeric = true;
            size_t start = 0;
            if (token[0] == '+' || token[0] == '-') // important to accept -1 .
            {
                start = 1;
            }
            if (start >= token.size())
            {
                numeric = false;
            }
            for (size_t i = start; i < token.size() && numeric; ++i)
            {
                if (!std::isdigit(static_cast<unsigned char>(token[i])))
                {
                    numeric = false;
                }
            }

            if (numeric)
            {
                return std::stoi(token);
            }

            if (token.size() == 1)
            {
                const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(token[0])));
                if (upper < 'A' || upper > 'Z' || upper == 'I')
                {
                    throw std::runtime_error("Invalid column letter: " + token);
                }

                int col = upper - 'A' + 1;
                if (upper > 'I')
                {
                    --col;
                }
                return col;
            }

            throw std::runtime_error("Invalid column token: " + token);
        }

        // symbols is an array defined in stones.hpp.
        Symbol symbol_from_name(const std::string &name)
        {
            const int symbolCount = static_cast<int>(sizeof(symbols) / sizeof(Symbol));
            for (int i = 0; i < symbolCount; ++i)
            {
                if (name == symbols[i].name)
                {
                    return symbols[i];
                }
            }
            return symbols[symbolCount - 2]; // UNKNOWN
        }

        // parse some text  like ( 5 , 4 ).  This function is used when read step information from a file.
        BoardGame::Position parse_position_tokens_or_throw(const std::string &text)
        {
            std::istringstream posStream(text);
            char lp = '\0', comma = '\0', rp = '\0';
            int row = 0, col = 0; // 0  0 form an invalid position.
            // cin >> x for a char will always skip leading whitespace and read the next non-whitespace character.
            if (!(posStream >> lp >> row >> comma >> col >> rp) || lp != '(' || comma != ',' || rp != ')')
            {
                throw std::runtime_error("Invalid position text: '" + text + "'.");
            }
            // return BoardGame::Position{row - 1, col - 1};
            return BoardGame::Position{row, col};
        }

    } // end of namespace BOARD_GAME

} // end of namespace CS111