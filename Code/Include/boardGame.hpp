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

#ifndef BOARD_GAME_HPP
#define BOARD_GAME_HPP

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <functional> // for std::function.
#include "board.hpp"
#include "board_presets.hpp"

/* ....[technical note]......
It is a very bad practice to put "using namespace someNS;" or "using someNS::something" in a header file, or in a class declaration. Because it FORCE all the code that includes this header file, or that uses this class, to also use the namespace someNS. Which means the power of namespace to solve name conflict are forced to be lost.  which may cause name conflicts and other issues. It is better to use the fully qualified name (e.g., CS111::BOARD_GAME::Board) in the header file, and let the client code decide whether to use "using namespace someNS;" or not.
*/
// using namespace CS111::BOARD_GAME;  // very bad style.
namespace CS111
{
    // you can add another namespace BOARD_GAME inside CS111, if you like.
    namespace BOARD_GAME
    {
        class BoardGame; // forward declaration of BoardGame, because it is used in the declaration of some functions in this namespace, e.g., print_step_info, which is used in the implementation of BoardGame. This is a common technique to solve circular dependency between classes and functions.

        /* .....[design decision].....
        The integer data are all 1-based. for example:
        position (1, 1) is the top-left corner, first row, and first column.
        player id is also 1 based. player 1, player 2, etc.
        */

        /*
        An abstract base class for board games.
        */
        class BoardGame
        {
            // some types are publicly available for external use, e.g., in derived classes, or in client code.
        public:
            enum class StepType
            {
                MOVE_STONE,
                PUT_STONE,
                OTHER // need this for some special game.
            };

            enum class GameStage // do not call it GameState, which could be understood as a snapshot. Call it stage to avoid confusion.
            {
                NOT_STARTED,
                IN_PROGRESS,
                FINISHED
            };

            /* [.....technical note ......]
            how to simplify a qualified name?  We can use the following technique:
            using simpleName = some_qualified_name;
            */
            using Stone = CS111::BOARD_GAME::Stone; // for convenience, so that we can directly use Stone instead of CS111::BOARD_GAME::Stone in the code of this class and its derived classes.

            struct Position
            {
                int row; // the y coordinate of the position, starting from 1, from top to bottom. For example, in a 19x19 board, the y coordinate can be from 1 to 19.
                int col; // the x coordinate of the position, starting from 1, from left to right. For example, in a 19x19 board, the x coordinate can be from 1 to 19.
            }; // note that be carefule to translate an algebraic notation like <5, D> into the x and y coordinates. In this case, the x coordinate is 4 (D is the 4th column), and the y coordinate is 5 (the row number).

            /* How to say in a step a player resignes?
            - Choice A: each step of playing, display a menu where a resigning choice is provided.
            - Choice B: use a special step to indicate resigning, e.g., a step where the to position is (-1, -1).
            Choice B seems to be simple. We choose it in this program.
            */

            // a constant position indicating resigning, which can be used in a step to indicate that the player resigns.
            static constexpr Position RESIGN_POSITION = {-1, -1};

            static constexpr int MAX_FAILED_STEPS = 3; // the maximum number of times a player can fail to make a valid step before losing the game. This is to prevent a player from being stuck in an infinite loop of making invalid steps.

            /* [.....technical note ......]
            Can we use const instead of constexpr for RESIGN_POSITION? No
            cannot compile. In class initialization  of const data member need to have integral type (integer, bool, enum), but Position is a struct, a congregate type, not allowed.

            In contrast, constexpr can be used for any literal type, including struct, as long as it can be initialized at compile time. Since Position is a literal type (it has a constexpr constructor and all its members are literal types, literal means can be evaluated at compile time), we can use constexpr to initialize RESIGN_POSITION at compile time. Therefore, we must use constexpr for RESIGN_POSITION, and cannot use const.
            */
            // static const Position RESIGN_POSITION2 = {-1, -1};  // cannot compile

            /* A single structure to represent two kinds of steps:
             - For a put_stone step, need two pieces of information: the stone to be put, and the position to put the stone (toX and toY).
             - For a move_stone step, need three pieces of information: the position of the stone to be moved (fromX and fromY), and the position to move the stone to.
             They both need a position (the toX and toY), but the fromX and fromY  and to_y, while something is not shared. To demonstrate the use of union, the not shared part (fromX and fromY, and stone) is put in a union, and the shared part (toX and toY) is put outside the union. The type of the step is indicated by the StepType enum.
            */
            struct Step
            {
                int player;    // 1-based index of the player who makes this step.
                StepType type; // the type of the step, e.g., MOVE_STONE or PUT_STONE, or OTHER. This will be set when a step is made.
                Position to;   // the position to put the stone (for put_stone step) or to move the stone to (for move_stone step). This is shared by both kinds of steps, so it is not put in the union.
                union          // anonnymous union, because we don't need to give it a name, and we can directly access its members.
                {
                    Position from; // only needed for move_stone step.
                    Stone stone;   // the stone that is to be put on the board.   only needed for put_stone step, not needed  for move_stone step.
                };
            };

            /* Example:
            if(step.type == StepType::PUT_STONE){
                print_stone(step.stone);
                print_position(step.to);
            }
            else{
                print_position(step.from);
                print_position(step.to);
            }
            */

            // .....[technical note].....
            // it is helpful to have a single structure containing all data of a game or a system.

            // a structure holding all relevant game information (all data members). This will make it easier to save and load the game state to/from a file, as we can directly save and load this structure. Also, make a concept of the "snapshot" of the game clarified. The program will be easier to understand.

            struct GameSnapshot
            {
                CS111::BOARD_GAME::Board board_;       // 1) the current state of the board
                std::string gameName_;                 // 2) the name of the game
                GameStage gameStage_;                  // 3) the current stage of the game (NOT_STARTED, IN_PROGRESS, FINISHED)
                int playerNumber_;                     // 4) the number of players in the game, since it can be read from a file, it should be const.
                std::vector<std::string> playerNames_; // 5) the names of the players

                int currentPlayerIndex_; // 6) the index of the current player (0-based)
                // int currentPlayerIndex = 0;

                /*. [..... technical note .....]
        The technique like
        int x = 5;
        in a class is called a non-static data member initializer (NSDMI).  it is like a default value for th data member. When a constructor does not assign value to member x, then x's value will be initialized to 5. If a constructor does assign value to member x, then x's value will be initialized to that value instead of 5. NSDMI ca make constructors simpler.
        */
                int winnerPlayerIndex_;             // 7) the index (1-based) of the winning player (-1 if no winner yet),
                int loserPlayerIndex_;              // 7.5) the index (1-based) of the losing player (-1 if no loser yet), used for some special games where there is a loser, e.g., in a game of 3 or more players, when one player wins, the other players are all losers. In this case, we can set the winnerPlayerIndex_ to the index of the winning player, and set the loserPlayerIndex_ to -1 to indicate that there is no single loser, but all other players are losers.
                StepType stepType_;                 // 8) the type of the current step (MOVE_STONE, PUT_STONE, OTHER)
                std::vector<Step> stepHistory_;     // 9) the history of steps made in the game
                std::vector<int> failedStepsCount_; // 10) the number of failed steps for each player, used to determine if a player should lose the game after too many failed steps.

                // a constructor, it has 9 parameters for the 9 data members.
                GameSnapshot(const CS111::BOARD_GAME::Board &board, const std::string &gameName, GameStage gameStage, int playerNumber, const std::vector<std::string> &playerNames, int currentPlayerIndex, int winnerPlayerIndex, int loserPlayerIndex, StepType stepType, const std::vector<Step> &stepHistory) : board_(board), gameName_(gameName), gameStage_(gameStage), playerNumber_(playerNumber), playerNames_(playerNames), currentPlayerIndex_(currentPlayerIndex), winnerPlayerIndex_(winnerPlayerIndex), loserPlayerIndex_(loserPlayerIndex), stepType_(stepType), stepHistory_(stepHistory),
                                                                                                                                                                                                                                                                                               failedStepsCount_(playerNumber, 0)
                // initialize the failedStepsCount_ vector with playerNumber elements, all initialized to 0.
                {
                    // throw exception if the number of elemetns in playerNames is not equal to playerNumber
                    if (playerNames.size() != playerNumber)
                    {
                        throw std::invalid_argument("The number of player names must be equal to the number of players.");
                    }

                    if (playerNames.size() != playerNumber)
                    {
                        throw std::invalid_argument("The number of player names must be equal to the number of players.");
                    }
                }
            };

            // data members are protected, instead of private, so that derived classes can direct access them. One advantage is that there is no need to write getter and setter functions for derived classes. One disadvantage is that it may break encapsulation, as derived classes can directly access and modify the data members. However, in this case, we assume that the derived classes are well-designed and will not misuse the data members.
        protected:
            /* ----------- data members -----------
            They are numbered, for ease of description, and to prevent forgetting handling them*/

            GameSnapshot initialGameState_; // a snapshot of the initial board state.

            GameSnapshot currentGameState_; // a snapshot of the current game state, which can be used for saving and loading the game state to/from a file.

        public:
            // the methods are numbered in the comment, for ease of description, and to prevent forgetting handling them.

            // <method 1>  a constructor that can initialize all members:

            /* make sure the 9 member of s initialGameState is initialized.
            1) board_; 2) gameName_; 3) gameStage_; 4) playerNumber_; 5) playerNames_; 6) currentPlayerIndex_; 7) winnerPlayerIndex_; 8) stepType_; 9) stepHistory_.

            Do not use reference parameter, because we need to make the game independent from outside objects. For example,
            instead of saying:
            const std::string &gameName,
            we say:
            const std::string gameName,
            */
            BoardGame(const CS111::BOARD_GAME::Board board,                                // 1)
                      const std::string gameName = "ANY",                                  // 2)
                      GameStage gameStage = GameStage::NOT_STARTED,                        // 3)
                      int playerNumber = 2,                                                // 4)
                      const std::vector<std::string> playerNames = {"playerA", "playerB"}, // 5)
                      int currentPlayerIndex = 1,                                          // 6) // player 1, 2, ... 1-based index for player, to be consistent with the display of player information.
                      int winnerPlayerIndex = -1,                                          // 7) // 1-based index for player, -1 if winner unknown, 0 means more than one winner, > 0 means a single winner. 
                      int loserPlayerIndex = -1,                                           // 7.5) // 1-based index for player, -1 if loser unknown, 0 means more than one loser, > 0 means a single loser.
                      StepType stepType = StepType::PUT_STONE,                             // 8)
                      const std::vector<Step> stepHistory = {})                            // 9)  empty vector can be described by {}.
                : initialGameState_(board, gameName, gameStage, playerNumber, playerNames, currentPlayerIndex, winnerPlayerIndex, loserPlayerIndex, stepType, stepHistory),
                  currentGameState_(initialGameState_) // initialGameState_ is initialized first.
            {
            }

            /* ......[technical note]......
            For the items in the initializer list list of a constructor, what are their time order of initialization?
            Answer: they are initialized in the order of their declaration in the class, not the order in the initializer list.
            Therefore, even if we write currentGameState_(initialGameState_) before initialGameState_(...), the initialGameState_ will be initialized first, and then currentGameState_ will be initialized with the value of initialGameState_. This is important to ensure that currentGameState_ is properly initialized with the value of initialGameState_.
            */

            /* a default constructor is not provided by this base class, because no obvious reason to pick which board as the default board.
            A derived class will provide a default constructor, when a board of choice is clear.

            Since BoardGame is an abstract base class and its correct initialization requires specific parameters (like board, game name, player info, step type), a default constructor would not make sense and could lead to incomplete or invalid objects.
            */

            // all the following functions are used to build the play() function.

            // <method 2> prepare the game for play. Possibly put some stones on the board before the game starts. Show some welcome message, do some checking.
            virtual void prepare(); //  Can  be overridden by derived classes for specific games.

            // <method 3> Ask for a step from the current player. This function does not do validity checking. Only return a step according to player's input. Possibly an invalid step is returned, whose validity will be checked by other functions.
            // the parameter s is updated, and returned. the step type shold be set in the parameter s, so that the returned step can be properly handled by other functions.
            virtual Step &get_step_from_player(Step &s) const; // can be overridden by derived classes for specific games.

            // <method 4> A function to decide if a step is valid or not.
            virtual bool is_step_valid(const Step &step) const = 0;

            // <method 5> print a step text to an output stream (console/file/string stream).
            // The printed format should be consistent with read_step_info().
            virtual void print_step_info(const Step &step, std::ostream &out = std::cout) const; // can be overridden by derived classes for specific games, as the step information may be different for different games.

            // <method 5.5> read one step text from an input stream (file/string stream).
            // The format should match print_step_info(). Returns false when parsing fails.
            virtual bool read_step_info(std::istream &in, Step &step) const;

            /* <method 6> Do a step and update the the information of the game. Assume the step is a VALID step.
            This method will also decide if the game is over.
            */
            virtual void do_step_and_update(const Step &step) = 0; // to be overridden by derived classes for specific games.

            /* <method 7>  update the game due to a player's abnormal step choice, like:
            -  when the player quits (choose a special stone position (-1, -1)), there are two cases:
               a)  if a game can continue (still some place to put a stone) but a player quits, then the player lose the game.
               b)  if a game cannot continue (no place to put a stone) and a player quits, then the game is over, and who is the winner to be decided by the game logic.
            -  If max failed steps reached, not quiting the game, then current player loses.
            Since this function has some game-specific logic. it is left to be implemented by a derived class.
            */
            virtual void handle_abnormal_step(const Step &step);

            // <method 8> Do the actions to end the game. This function is called when it is already known that the game is over.
            virtual void end(); // to be overridden by derived classes for specific games.

            // <method 9> play the game.
            virtual void play();

            /* <method 10> replay the game.
            to be overridden by derived classes for specific games. This function can replay the game,
            - When quiet is false: display the board after each step. Show the initial game information, display the initial board (where the stones are on the board if there is some).  For each step, it can also ask the user to press a key to continue to the next step, so that the user can have time to see the board and the step information. Then after each step, the updated board is shown, until the game ends.
            - When quiet is true: do not display the board or any game information, just update the internal state. This can be used for testing, or for quickly replaying a game without displaying the board. quiet replay can be used when load a game from a file, to quickly apply all steps starting from the initial board.
            */
            virtual void replay(bool quiet);

            // <method 11> a virtual destructor, to ensure that the destructor of the derived class is called when a base class pointer is deleted. This is important for proper resource management and to avoid memory leaks.
            virtual ~BoardGame() = default; // using the default destructor, which is sufficient for this class, as it does not manage any resources directly. The derived classes can override the destructor if they need to manage resources.

            // <method 12> save history (initial board and step history) to a file.
            // Step text is produced by print_step_info() (which derived classes can override).
            virtual void save_game_to_file(const std::string &filename) const;

            // <method 13> load history step from a file to a game (set the initial board and step history).
            // Warning: board and step history will be replaced by loaded data.
            // Step text is parsed by read_step_info() (which derived classes can override).
            virtual void load_game_from_file(const std::string &filename);

            // <method 14> reset the game to the initial state, which is useful when we want to replay the game after replaying the game, or when we want to start a new game with the same initial board.  when this function is called, some warning should be given because without saving, the current game state (including step history will be lost.)
            virtual void reset_to_initial_state()
            {
                currentGameState_ = initialGameState_;
            }

            // <method 15> branch the current game, which can be used to start a new game from the current game state. This is useful for studying how to play at the current moment, or for trying different strategies from the current game state. The new game starts from the current game state, and has the same initial game state as the current game.

            // cannot compile:
            // BoardGame branch_the_current_game();
            // because BoardGame is an abstract class, and cannot be instantiated. Therefore, we cannot return a BoardGame object. We can only return a pointer or reference to a BoardGame object, which can point to a derived class object.

            // return a unique pointer to a BoardGame object, which can point to a derived class object. The derived class can override this function to return a unique pointer to its own type of game object. A unique pointer is used here to ensure that the ownership of the game object is clear, and to avoid memory leaks. The caller of this function will get a unique pointer to a new game object that is a copy of the current game state, and can use it to play a new game from the current state.

            // this function cannot be defined in the base class, do not know how to create a clone of the abstract base class. So it should be implemented in a derived class.
            virtual std::unique_ptr<BoardGame> branch_the_current_game() const = 0;

            // <method 16> a function check if it is possible to make a step. If no stp is possible, do some update of the game state. It has game-specific logic. Here a version is provided simply checking the board is full. A derived class can override it.
            // returns true when no more step is possible, false otherwise.
            // virtual bool check_and_handle_no_more_step() const;  // cannot be const, since some state is updated.
            virtual bool check_and_handle_no_more_step();

            //[.....technical note ......]
            // can add additional methods. These methods can be added in the derived class or here in the base class. But since a derived class can always add more fields and handle them, maybe it is better to let the derived class to decide and define additional methods.
            // optional extension points for derived classes with extra game-specific state.
            // these extra_fields handling methods are not required here. But the serve as some "reminder", some "hook", letting the derived class know that they can be overridden when needed.
            // virtual void save_extra_fields(std::ostream &) const {}
            // virtual void load_extra_fields(std::istream &) {}
            // return a new game object that is image of the current game. I.e., the new game starts from the current game state. This is useful for many purposes, like study how to play at the current moment.

        }; // end of class BoardGame

    } // end of namespace BOARD_GAME

} // end of namespace CS111


#endif