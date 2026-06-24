/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 2
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * 
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "../Include/stones.hpp"
#include "../Include/tool.hpp"

using namespace std;
using namespace CS111::BOARD_GAME;
using CS111::Tools::Test::TestContext;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

int main() {
    TestContext ctx;
    print_test_banner("STONES CLASS FULL TEST PROGRAM");

    // ==============================================
    // Test 1: Create Stones with 5 empty stones
    // ==============================================
    print_test_separator("1. Create Stones object with 5 empty stones");
    try {
        Stones s(5);
        cout << ":) Created successfully. Number of stones: " << s.getStoneNum() << endl;
    } catch (...) {
        cout << ":( FAILED\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Test 2: Set a valid stone (index 0 -> BLACK)
    // ==============================================
    print_test_separator("2. Set stone at index 0 to BLACK");
    // setStone may throw if index is out of bounds, so we catch any exception here to check if the method works for valid input.
    try { 
        Stones s(5);
        s.setStone(0, symbols[0], Color::BLACK, Color::WHITE, Shape::CIRCLE);
        cout << ":) Set stone 0 successfully\n";
    } catch (...) {
        cout << ":( FAILED\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Test 3: Set Chinese Chess stone (index 1)
    // Some terminals may need UTF-8 code page (chcp 65001) to render symbols correctly.
    // ==============================================
    print_test_separator("3. Set stone at index 1 to Chinese Chess stone");
    try {
        Stones s(5);
        s.setStone(1, symbols[2], Color::RED, Color::BLACK, Shape::SQUARE);
    cout << ":) Set Chinese chess piece successfully\n";

    } catch (...) {
        cout << ":( FAILED\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Test 4: Access valid index
    // ==============================================
    print_test_separator("4. Get stone at valid index (0)");
    try {
        Stones s(5);
        Stone st = s.getStone(0);
        cout << ":) getStone(0) works\n";
    } catch (...) {
        cout << ":( FAILED\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Test 5: Access invalid index -> EXPECT ERROR
    // ==============================================
    print_test_separator("5. Try accessing index out of bounds (should throw)");
    try {
        Stones s(5);
        s.getStone(999);
        cout << ":( FAILED: No exception thrown!\n";
        ++ctx.failed_checks;
    } catch (const out_of_range& e) {
        cout << ":) CORRECTLY threw exception: " << e.what() << "\n";
    } catch (...) {
        cout << ":( FAILED: Wrong exception\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Test 6: showStones() - display all stones
    // ==============================================
    print_test_separator("6. showStones() method (print all stones)");
    try {
        Stones s(6);

        // Set some pieces for display test
        s.setStone(0, symbols[0], Color::BLACK, Color::WHITE, Shape::CIRCLE);
        s.setStone(1, symbols[1], Color::WHITE, Color::BLACK, Shape::CIRCLE);
        s.setStone(2, symbols[2], Color::RED, Color::BLACK, Shape::SQUARE);
        s.setStone(3, symbols[10], Color::BLACK, Color::WHITE, Shape::CIRCLE);
        s.setStone(4, symbols[20], Color::WHITE, Color::BLACK, Shape::CIRCLE);

        cout << "Calling showStones():\n\n";
        s.showStones();  // Your print function
        cout << "\n:) showStones() executed\n";
    } catch (...) {
        cout << ":( FAILED\n";
        ++ctx.failed_checks;
    }

    // ==============================================
    // Final result
    // ==============================================
    print_test_separator("Summary");
    if (ctx.failed_checks == 0) {
        cout << ":) ALL TESTS COMPLETED SUCCESSFULLY!\n";
    } else {
        cout << ":( SOME TESTS FAILED!\n";
    }
    cout << "Total failed checks: " << ctx.failed_checks << "\n";

    return ctx.failed_checks == 0 ? 0 : 1;
}