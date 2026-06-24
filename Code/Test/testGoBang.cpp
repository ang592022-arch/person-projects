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

// Test/testGoBang.cpp
// GoBang test file: covers construction, step validation, win condition, save/load, replay, and (optionally) play.

#include <iostream>
#include <fstream>
#include <sstream>
#include "../Include/goBang.hpp"
#include "../Include/tool.hpp"


using CS111::BOARD_GAME::GoBang;
using CS111::Tools::Test::TestContext;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

void CS111::BOARD_GAME::test_GoBang() {
    using Step = CS111::BOARD_GAME::BoardGame::Step;
    using StepType = CS111::BOARD_GAME::BoardGame::StepType; 
    using GameStage = CS111::BOARD_GAME::BoardGame::GameStage;

    TestContext ctx;
    print_test_banner("GOBANG FUNCTIONAL TEST PROGRAM");

    // Section 1: Constructor and Initial State
    print_test_separator("1. Constructor and Initial State");
    GoBang game(7, "GoBang", {"Alice", "Bob"});
    check_true(ctx, game.currentGameState_.board_.get_width() == 7, "Board width should be 7");
    check_true(ctx, game.currentGameState_.playerNames_[0] == "Alice", "First player should be Alice");
    check_true(ctx, game.currentGameState_.playerNames_[1] == "Bob", "Second player should be Bob");
    check_true(ctx, game.currentGameState_.gameStage_ == GameStage::NOT_STARTED, "Game should not be started");

    // Section 2: Step Validation
    print_test_separator("2. Step Validation");
    Step s1{1, StepType::PUT_STONE, {4, 4}}; // here 1 means player 1, that is Alice, and the position (4,4) is a valid position on the board. Note that the stone of a step is not needed for step validation and updating, so it can be left uninitialized. The stone information in a step is only needed for printing the step information, so it can be set when printing.
    // put s1 in the current game state step history. 
    game.currentGameState_.stepHistory_.push_back(s1);
    check_true(ctx, game.is_step_valid(s1), "Valid step should be accepted");
    game.do_step_and_update(s1); // Place black at (4,4)
    Step s2{2, StepType::PUT_STONE, {4, 4}}; // bob's turn. 
    game.currentGameState_.stepHistory_.push_back(s2);
    check_true(ctx, !game.is_step_valid(s2), "Occupied step should be rejected");
    Step s3{2, StepType::PUT_STONE, {8, 8}}; // still bob's turn, because s2 is invalid, the current player does not change. 
    game.currentGameState_.stepHistory_.push_back(s3);
    check_true(ctx, !game.is_step_valid(s3), "Out of bounds step should be rejected");
    Step s4{2, StepType::PUT_STONE, {4, 5}}; // still bob's turn, because s2 and s3 are invalid, the current player does not change.
    game.currentGameState_.stepHistory_.push_back(s4);
    check_true(ctx, game.is_step_valid(s4), "Valid step should be accepted");
    game.do_step_and_update(s4); // Place white at (4,5)

    // Section 3: Win Condition
    print_test_separator("3. Win Condition");
    // Black: (4,4), (5,4), (6,4) already placed

    // this function is needed for printing a step, when a symbol of the stone of a step is printed, so better include the right stone in the step. 
    // for the functions is_step_valid and do_step_and_update, the stone of a step is not needed. 

    auto bind_put_stone_from_player = [&](Step &step)
    {
        if (step.type != StepType::PUT_STONE)
        {
            return;
        }
        auto stones = game.currentGameState_.board_.get_stones_catalog();
        if (step.player >= 1 && step.player <= stones.getStoneNum())
        {
            step.stone = stones.getStone(step.player-1);
        }
    };

    std::cout << "Board before s5:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s5{1, StepType::PUT_STONE, {5, 4}}; // alice's turn, place black at (5,4), now 4 in a column (4..6, col 4)
    bind_put_stone_from_player(s5);
    game.currentGameState_.stepHistory_.push_back(s5);
    check_true(ctx, game.is_step_valid(s5), "Valid step should be accepted");
    game.do_step_and_update(s5); // Black at (5,4)

    std::cout << "Applied step s5: ";
    game.print_step_info(s5, std::cout);
    std::cout << "\n";
    std::cout << "Board after s5:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s6{2, StepType::PUT_STONE, {5, 5}}; // bob's turn, place white at (5,5)
    bind_put_stone_from_player(s6);
    game.currentGameState_.stepHistory_.push_back(s6);
    check_true(ctx, game.is_step_valid(s6), "Valid step should be accepted");
    game.do_step_and_update(s6); // White at (5,5)

    std::cout << "Applied step s6: ";
    game.print_step_info(s6, std::cout);
    std::cout << "\n";
    std::cout << "Board after s6:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s7{1, StepType::PUT_STONE, {6, 4}}; // alice's turn, place black at (6,4)
    bind_put_stone_from_player(s7);
    game.currentGameState_.stepHistory_.push_back(s7);
    check_true(ctx, game.is_step_valid(s7), "Valid step should be accepted");
    game.do_step_and_update(s7); // Black at (6,4)

    std::cout << "Applied step s7: ";
    game.print_step_info(s7, std::cout);
    std::cout << "\n";
    std::cout << "Board after s7:\n";
    game.currentGameState_.board_.display_board(std::cout);

    check_true(ctx, game.currentGameState_.gameStage_ != GameStage::FINISHED, "Game should not be finished yet (only 3 black stones in line)");
    check_true(ctx, game.currentGameState_.winnerPlayerIndex_ == -1, "There should be no winner yet");

    Step s8{2, StepType::PUT_STONE, {1, 1}};
    bind_put_stone_from_player(s8);
    check_true(ctx, game.is_step_valid(s8), "Valid step should be accepted");
    game.do_step_and_update(s8); // White filler step

    std::cout << "Applied step s8: ";
    game.print_step_info(s8, std::cout);
    std::cout << "\n";
    std::cout << "Board after s8:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s9{1, StepType::PUT_STONE, {7, 4}};
    bind_put_stone_from_player(s9);
    game.currentGameState_.stepHistory_.push_back(s9);
    check_true(ctx, game.is_step_valid(s9), "Valid step should be accepted");
    game.do_step_and_update(s9); // Black at (7,4)

    std::cout << "Applied step s9: ";
    game.print_step_info(s9, std::cout);
    std::cout << "\n";
    std::cout << "Board after s9:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s10{2, StepType::PUT_STONE, {1, 2}};
    bind_put_stone_from_player(s10);
    game.currentGameState_.stepHistory_.push_back(s10);
    check_true(ctx, game.is_step_valid(s10), "Valid step should be accepted");
    game.do_step_and_update(s10); // White filler step

    std::cout << "Applied step s10: ";
    game.print_step_info(s10, std::cout);
    std::cout << "\n";
    std::cout << "Board after s10:\n";
    game.currentGameState_.board_.display_board(std::cout);

    Step s11{1, StepType::PUT_STONE, {3, 4}};
    bind_put_stone_from_player(s11);
    game.currentGameState_.stepHistory_.push_back(s11);
    check_true(ctx, game.is_step_valid(s11), "Valid step should be accepted");
    game.do_step_and_update(s11); // Black at (3,4), now 5 in a column (3..7, col 4)

    std::cout << "Applied step s11: ";
    game.print_step_info(s11, std::cout);
    std::cout << "\n";
    std::cout << "Board after s11:\n";
    game.currentGameState_.board_.display_board(std::cout);

    check_true(ctx, game.currentGameState_.gameStage_ == GameStage::FINISHED, "Game should be finished after black gets 5 in a line");
    check_true(ctx, game.currentGameState_.winnerPlayerIndex_ == 1, "Black should be the winner");

    // Section 4: Save/Load
    print_test_separator("4. Save/Load");

    /* Here the relative path is used. When the folder is executed, assume the terminal current folder is the code of Hmk3, i.e., BoardGamePlay, where the makefile appears. 
    
    .....[technical note].....
    Note that when the executable is running, the relative pathis relative the current folder at the terminal, not relative the path of the executable. 
    
    So, the command to run this executable fils should be like : 
    ./Test/testGoBang
    when the terminal current folder is BoardGamePlay, which is the folder where the makefile appears.
    */ 

    // assume the current folder at terminal is Bin
    // game.save_game_to_file("../Test/goBangTestSave_out.txt");

    // assume the current folder at terminal is BoardGamePlay, which is the folder where the makefile appears.
    game.save_game_to_file("./Test/goBangTestSave_out.txt");

    std::cout << "Saved game to ./Test/goBangTestSave_out.txt\n";

    GoBang loaded(7, "GoBang", {"person1", "person2"}); // the parameters here do not matter, because they will be overwritten by the loaded game information.
    loaded.load_game_from_file("./Test/goBangTestSave_in.txt");

    std::cout << "Loaded game from ./Test/goBangTestSave_in.txt\n";

    check_true(ctx, loaded.currentGameState_.board_.get_width() == 7, "Board width should be 7");
    check_true(ctx, loaded.currentGameState_.playerNames_[0] == "Alice", "First player should be Alice");
    check_true(ctx, loaded.currentGameState_.stepHistory_.size() == 5, "Step history should have 5 steps");
    check_true(ctx, loaded.currentGameState_.gameStage_ == GameStage::IN_PROGRESS, "Game should be in progress");

    // Section 5: Replay
    print_test_separator("5. Replay");
    loaded.replay(true); // Quiet replay, should not throw
    check_true(ctx, loaded.currentGameState_.stepHistory_.size() == 5, "Step history should still have 5 steps after replay");

    // Section 6: Play (Manual, optional)
    print_test_separator("6. Play (Manual, optional)");
    std::cout << "You may now play a short GoBang game (manual test).\n" 
    "During the game, enter Position -1 -1 to resign. \n"
    "Press Enter to start. ";
    std::cin.get();
    // Uncomment to enable manual play:
    game.reset_to_initial_state();
    game.play();

    // Summary
    std::cout << "\nTest sections: 6\n";
    // ctx has no member called total_checks, not perfect, but we do not want to change tools.cpp
    // just show the number of failed 
    std::cout <<  /* "Assertions: " << ctx.total_checks << ", */ 
       " Failed Assertions: " << ctx.failed_checks << std::endl;

    std::cout << "\n:) Hope you did not find problem when play the game.  \n";
}

int main() {
    CS111::BOARD_GAME::test_GoBang(); 
    return 0;
}
