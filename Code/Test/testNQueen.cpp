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

// testNQueen.cpp
// Test file for NQueen game (interactive two-player version)
// Consistent style: numbered sections, clear banners, summary report.

#include <iostream>
#include <fstream>
#include <vector>
#include "../Include/nQueen.hpp"
#include "../Include/tool.hpp"

using CS111::BOARD_GAME::NQueen;
using CS111::Tools::Test::TestContext;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;


// Friend function for white-box testing
void CS111::BOARD_GAME::test_NQueen() {
    TestContext ctx;
    print_test_banner("NQUEEN FUNCTIONAL TEST PROGRAM");

    // short name for the Step 
    using Step = CS111::BOARD_GAME::BoardGame::Step;

    // short name for the GameStage
    using GameStage = CS111::BOARD_GAME::BoardGame::GameStage;

    // short name for the StepType
    using StepType = CS111::BOARD_GAME::BoardGame::StepType;

    // Section 1: Constructor and Initial State
    print_test_separator("1. Constructor and Initial State");
    NQueen game(8, "NQueen", {"Alice", "Bob"});
    check_true(ctx, game.currentGameState_.board_.get_width() == 8, "Board width should be 8");
    check_true(ctx, game.currentGameState_.playerNames_[0] == "Alice", "Player 1 name should be Alice");
    check_true(ctx, game.currentGameState_.playerNames_[1] == "Bob", "Player 2 name should be Bob");
    // current player? 
    check_true(ctx, game.currentGameState_.currentPlayerIndex_ == 1, "Current player ID should be 1 (Alice)");
    check_true(ctx, game.currentGameState_.gameStage_ == GameStage::NOT_STARTED, "Game stage should be NOT_STARTED");
    // total number of players
    check_true(ctx, game.currentGameState_.playerNumber_ == 2, "Total number of players should be 2");

    // Section 2: Step Validation
    print_test_separator("2. Step Validation");

    // this function is needed for printing a step, when a symbol of the stone of a step is printed, so better include the right stone in the step. 
    // for the functions is_step_valid and do_step_and_update, the stone of a step is not needed. 


    // several steps are made. To simulate the game, when a step that is valid, do the step and update the game.

    auto bind_put_stone_from_player = [&](Step &step)
    {
        auto stones = game.currentGameState_.board_.get_stones_catalog();

        step.stone = stones.getStone(step.player - 1);
    };

    // Place a queen at (1,1) for Alice (valid)
    // player 1, type put_stone, position (1,1)
    Step s1{1, StepType::PUT_STONE, {1, 1}};
    bind_put_stone_from_player(s1); // assign the correct stone to the step according to the player id in the step.  
    game.print_step_info(s1, std::cout);
    std::cout << "\n"; 
    check_true(ctx, game.is_step_valid(s1), "Valid step should be accepted");
    // add the step to the game history. this is asumed before a step is applied. 
    game.currentGameState_.stepHistory_.push_back(s1);
    game.do_step_and_update(s1);

    // Try to place a queen at (2,3) for Bob (  valid)
    Step s2{2, StepType::PUT_STONE, {2, 3}};
    bind_put_stone_from_player(s2);   
    game.print_step_info(s2, std::cout);
    std::cout << "\n"; 
    check_true(ctx, game.is_step_valid(s2), "Valid step should be accepted");
    game.currentGameState_.stepHistory_.push_back(s2);
    game.do_step_and_update(s2);

    // Try to place a queen at (2,3) for Alice (should be invalid at the occupied position)
    Step s3{1, StepType::PUT_STONE, {2, 3}};
    bind_put_stone_from_player(s3);   
    game.print_step_info(s3, std::cout);
    std::cout << "\n"; 
    check_true(ctx, !game.is_step_valid(s3), "Invalid step at occupied position should be rejected");

    // Place a queen at (3,5) for Alice (valid)
    Step s4{1, StepType::PUT_STONE, {3, 5}};
    bind_put_stone_from_player(s4);  
    game.print_step_info(s4, std::cout);
    std::cout << "\n"; 
    check_true(ctx, game.is_step_valid(s4), "Valid step should be accepted");
    game.currentGameState_.stepHistory_.push_back(s4);
    game.do_step_and_update(s4);

    // Section 3: Win Condition
    print_test_separator("3. Win Condition");
    // Place a queen at (4, 4) for Bob (diagonal threat)
    Step s5{2, StepType::PUT_STONE, {4, 4}};
    bind_put_stone_from_player(s5);   
    game.print_step_info(s5, std::cout);
    std::cout << "\n"; 
    check_true(ctx, game.is_step_valid(s5), "Valid step should be accepted");
    game.currentGameState_.stepHistory_.push_back(s5);
    game.do_step_and_update(s5);

    check_true(ctx, game.currentGameState_.gameStage_ == GameStage::FINISHED, "Game should be finished after a conflict occurs");
    check_true(ctx, game.currentGameState_.winnerPlayerIndex_ == 1, "Alice should be the winner after Bob's move is threatened");

    // Section 4: Save and Load
    print_test_separator("4. Save and Load");
    
    // Save the current game to a file. assume the current folder in the terminal is the folder where the makefile is. 
    // the saveFile path is always relative to the current folder of the terminal. 
    // the same file will be used for testing both save and load. 
    std::string saveFile = "./Test/nQueenTestSave.txt";
    game.save_game_to_file(saveFile);
    // Load the game from the file
    NQueen loadedGame(8, "NQueen", {"Alice", "Bob"});
    loadedGame.load_game_from_file(saveFile);
    // Check loaded state
    check_true(ctx, loadedGame.currentGameState_.board_.get_width() == 8, "Loaded board width should be 8");
    check_true(ctx, loadedGame.currentGameState_.playerNames_[0] == "Alice", "Loaded player 1 name should be Alice");
    check_true(ctx, loadedGame.currentGameState_.gameStage_ == GameStage::FINISHED, "Loaded game stage should be FINISHED");
    check_true(ctx, loadedGame.currentGameState_.winnerPlayerIndex_ == 1, "Loaded winner should be Alice");

    // Section 5: Replay
    print_test_separator("5. Replay");
    loadedGame.replay(true); // Should replay the game from the beginning, quitely without printing step info, and without waiting for user input. This is just to test the replay functionality, not for user interaction in this test program.
    // After replay, the step history should be preserved
    check_true(ctx, loadedGame.currentGameState_.stepHistory_.size() == game.currentGameState_.stepHistory_.size(), "Step history should be preserved after replay");

    // A section to play the game interactively, which can be used for manual testing. It is optional, and can be commented out if not needed.
    print_test_separator("6. Play (Manual, optional)");
    std::cout << "You may now play a short NQueen game (manual test).\n";
    NQueen interactiveGame(8, "NQueen", {"Alice", "Bob"});
    interactiveGame.play(); // This will start an interactive game, 

    // Summary
    std::cout << "\nTest sections: 5\n";
    //std::cout << "Assertions: " << ctx.total_checks << ", Failed: " 
    std::cout << "Failed: " << ctx.failed_checks << std::endl;
    //if (ctx.failed_checks != 0) std::exit(1);
    std::cout << "\n:) Hope you did not find problem when play the game.  \n";
}

int main() {
     CS111::BOARD_GAME::test_NQueen();
    return 0;
}
