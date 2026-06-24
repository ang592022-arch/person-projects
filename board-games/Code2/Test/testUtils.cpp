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

#include <iostream>
#include <sstream>
#include "../Include/utils.hpp"
#include "../Include/tool.hpp"

using CS111::extract_token;
using CS111::extract_line;
using CS111::extract_required_token;
using CS111::extract_required_text;
using CS111::pause_for_enter;
using CS111::trim;
using CS111::reach_non_whitespace;
using CS111::Tools::Test::TestContext;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

int main() {
    TestContext ctx;
    print_test_banner("UTILS FUNCTION TEST PROGRAM");

    // Test 1: extract_token
    print_test_separator("1. extract_token basic usage");
    std::istringstream iss1("GAME_NAME: \"GoBang\"");
    check_true(ctx, extract_token(iss1) == "GAME_NAME:", "First token should be 'GAME_NAME:'");
    check_true(ctx, extract_token(iss1) == "\"GoBang\"", "Second token should be '\"GoBang\"'");

    // Test 2: extract_line
    print_test_separator("2. extract_line basic usage");
    std::istringstream iss2("Line one\nLine two\n");
    check_true(ctx, extract_line(iss2) == "Line one", "First line should be 'Line one'");
    check_true(ctx, extract_line(iss2) == "Line two", "Second line should be 'Line two'");

    // Test 3: extract_required_token (success and fail)
    print_test_separator("3. extract_required_token success/fail");
    std::istringstream iss3("GAME_NAME: \"GoBang\"");
    check_true(ctx, extract_required_token(iss3, "GAME_NAME:"), "Should match 'GAME_NAME:'");
    check_true(ctx, !extract_required_token(iss3, "PLAYER_NAME:"), "Should fail to match 'PLAYER_NAME:'");

    // Test 4: extract_required_text (success and fail)
    print_test_separator("4. extract_required_text success/fail");
    std::istringstream iss4("===End of Game Info===");
    check_true(ctx, extract_required_text(iss4, "===End of Game Info==="), "Should successfully extract '===End of Game Info==='");
    std::istringstream iss5("Hello world");
    check_true(ctx, !extract_required_text(iss5, "Goodbye"), "Should fail to extract 'Goodbye'");

    // Test 5: reach_non_whitespace (success and EOF), together used with extract_required_text
    print_test_separator("5. reach_non_whitespace with extract_required_text");
    std::istringstream iss6("   ===End of Game Info===");
    check_true(ctx, reach_non_whitespace(iss6), "Should reach non-whitespace character");
    check_true(ctx, extract_required_text(iss6, "===End of Game Info==="), "Should successfully extract '===End of Game Info==='");
    std::istringstream iss7("   ");
    check_true(ctx, !reach_non_whitespace(iss7), "Should reach EOF without finding non-whitespace");


    // Test 6: pause_for_enter (manual, not auto-tested)
    print_test_separator("6. pause_for_enter (manual)");
    std::cout << "Please press Enter to continue...\n";
    pause_for_enter();

    // test for trim
    print_test_separator("7. trim function");
    std::string s1 = "   Hello World   ";
    std::string s2 = trim(s1);
    check_true(ctx, s2 == "Hello World", "Trim should remove leading and trailing whitespace");

    // Summary
    constexpr int num_test_sections = 7; // Numbered/visual test blocks
    constexpr int num_assertions = 11;    // Number of check_true calls (pause_for_enter is not an assertion)
    std::cout << "\n===== TEST SUMMARY =====\n";
    std::cout << "Test sections:   " << num_test_sections << std::endl;
    std::cout << "Assertions:      " << num_assertions << std::endl;
    std::cout << "Failed asserts:  " << ctx.failed_checks << std::endl;
    std::cout <<   "========================\n";
    return ctx.failed_checks == 0 ? 0 : 1;
}