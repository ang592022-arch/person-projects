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

/* GoBang, a concrete class, a derived class of GoLike */

#ifndef GOBANG_HPP
#define GOBANG_HPP

#include "../Include/goLike.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        class GoBang : public GoLike
        {
        public:
            // <method 1> constructor. Need to implemented.
            GoBang(
                int boardSize = 19,
                const std::string gameName = "GoBang",
                const std::vector<std::string> playerNames = {"playerA", "playerB"});

            // since GoBang is a concrete class, all the pure virtual methods of BoardGame should be implemented.
            // also the methods with  game specific logic should be implement.
            // Therefore, the following methods need to be implemented, and are declared here.

            // void prepare() override; // <method 2> prepare the game, Just use the BoardGame's version.

            // get_step_from_player(). <method 3> Just use the BoardGame's version.

            bool is_step_valid(const Step &step) const override; // <method 4> check if a step is valid according to the rules of GoBang.

            // void print_step_info(const Step &step) const override; // <method 5> print the a step information.  just use the BoardGame's version,

            //  read_step_info, <method 5.5>, just use the BoardGame's version.

            void do_step_and_update(const Step &step) override; // <method 6> This function has GoBang logic

            // void handle_abnormal_step(const Step &step) override; // <method 7>, just use the BoardGame's version.

            // void end() override; // <method 8> just use the BoardGame's version, which is simple and general enough.

            // void play() override; // <method 9> just use the BoardGame's version, which is simple and general enough.

            // void replay() override; // <method 10> just use the BoardGame's version, which is simple and general enough.

            // destructor, <method 11> just use the default version, which is sufficient for this class, as it does not manage any resources directly.
            virtual ~GoBang() = default;

            // save_game_to_file() <method 12>  just use the BoardGame's version

            // load_game_from_file() <method 13> just use the BoardGame's version

            // reset_to_initial_state() <method 14> just use the BoardGame's version, which is simple and general enough.

            std::unique_ptr<BoardGame> branch_the_current_game() const override; // <method 15> must be implemented by a derived class.

            // <method 16>, check_and_handle_no_more_step() just use the BoardGame's version, which is simple and general enough.

        protected:
            bool check_win_condition(int x, int y, const Stone &stone_to_put) const;

        
        // Friend test function for white-box testing
        // Friend is not a member. It is irrelevent to be public or protected. It can be declared in either section. Here it is declared in the protected section, just for no particular reason.
        friend void test_GoBang();


        }; // end of class GoBang

        // better to declare test_GoBang() in side the namespace BOARD_GAME.
        void test_GoBang(); //
    } // end of namespace BOARD_GAME
} // end of namespace CS111

#endif