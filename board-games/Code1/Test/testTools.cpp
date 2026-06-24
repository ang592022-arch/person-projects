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

#include "../Include/tools.hpp"
#include <iostream>
#include <string>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

// Namespace for test helpers (matches CS111 namespace convention)

using namespace CS111::Tools;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

namespace CS111::Test
{
    /**
     * @brief Tests the clear_input_queue() function with various input scenarios
     */
    void test_clear_input_queue()
    {
        print_test_separator("1. clear_input_queue()");
        std::cout << "waiting here for you to type some input (e.g., 'abc123' then Enter) to test clear_input_queue()...\n"; 
        
        // Test 1: Basic leftover input (numeric + string)
        std::cout << "\nTest 1: Enter '123abc' (then press Enter): ";
        // Trigger failed input state first (mimic real usage with read_int)
        int dummy;
        std::cin >> dummy; // Reads 123, leaves "abc\n" in buffer
        clear_input_queue();
        
        // Verify buffer is clear by reading next input
        std::cout << "Enter any text to confirm buffer is clear: ";
        std::string testStr;
        std::getline(std::cin, testStr);
        std::cout << "Received: '" << testStr << "' (buffer cleared correctly)\n" << std::endl;

        // Test 2: Empty input (only newline)
        std::cout << "Test 2: Press Enter (empty input): ";
        clear_input_queue();
        std::cout << "Enter text to verify: ";
        std::getline(std::cin, testStr);
        std::cout << "Received: '" << testStr << "' (handled empty input)\n" << std::endl;

        // Test 3: EOF simulation (Ctrl+D on Unix/Ctrl+Z on Windows)
        std::cout << "Test 3: Press Ctrl+D (NO NEED to press Enter, UNIX/Linux/MacOS) or Ctrl+Z  then Enter (Windows): ";
        clear_input_queue();
        std::cout << "clear_input_queue() handled EOF correctly\n" << std::endl;
        std::cout<< "if read_int() cannot work anymore upon the EOF contition, restart the program and avoid generating the EOF signal at this step, so that you can continue testing the read_int() function in the next test suite. \n" << std::endl;

    std::cout << "clear_input_queue() manual checks complete.\n" << std::endl;

  
    }

    /**
     * @brief Tests the read_int() function with valid/invalid input scenarios
     */
    void test_read_int()
    {
        print_test_separator("2. read_int()");

        // Test 1: Valid integer input (basic case)
        std::cout << "\nTest 1: Enter a valid integer (e.g., 42): ";
        int validInt = read_int("Enter integer: ");
        std::cout << "Success! Read integer: " << validInt << "\n" << std::endl;

        // Test 2: Invalid input (non-numeric)
        std::cout << "Test 2: Enter non-integer (e.g., 'abc'): ";
        int invalidRecovered = read_int("Enter integer: ");
        std::cout << "Success! Recovered and read integer: " << invalidRecovered << "\n" << std::endl;

        // Test 3: Mixed input (valid integer + extra text)
        std::cout << "Test 3: Enter integer with extra text (e.g., '99xyz'): ";
        int mixedInt = read_int("Enter integer: ");
        std::cout << "Success! Extracted integer: " << mixedInt << " (ignored extra text)\n" << std::endl;

        // Test 4: Negative integer (edge case)
        std::cout << "Test 4: Enter negative integer (e.g., -789): ";
        int negativeInt = read_int("Enter integer: ");
        std::cout << "Success! Read negative integer: " << negativeInt << "\n" << std::endl;

        // Test 5: Zero (edge case)
        std::cout << "Test 5: Enter 0: ";
        int zeroInt = read_int("Enter integer: ");
        std::cout << "Success! Read zero: " << zeroInt << "\n" << std::endl;

        std::cout << "read_int() manual checks complete.\n" << std::endl;
    }

    void test_print_char32()
    {
        print_test_separator("3. print_char32()");

        // Set locale to support Unicode (critical on some systems)
        std::locale::global(std::locale(""));

        // Reference map of the intended original characters:
        // U+0041 -> A
        // U+4F60 -> 你
        // U+1F600 -> 😀
        // U+03A9 -> Ω
        // U+8ECA -> 車
        // U+8F66 -> 车
        // U+2655 -> ♕
        // U+265B -> ♛

        std::cout << " - ASCII character U+0041: ";
        print_char32(U'\u0041'); // 'A'
        std::cout << " (expected: LATIN CAPITAL LETTER A)\n";

        std::cout << " - Chinese character U+4F60: ";
        print_char32(U'\u4F60');
        std::cout << " (expected: CJK character for NI / you)\n";

        std::cout << " - Emoji U+1F600: ";
        
        // bug, for code point with more than 4 hex digits, need to use \U instead of \u, and need to have 8 hex digits after \U, so need to add leading zeros.
        // print_char32(U'\u1F60'); // '😀' 
        // // print_char32(U'\u1F600'); // compile error \u cannot be followed by more than 4 hex digits.
        print_char32(U'\U0001F600');
        std::cout << " (expected: GRINNING FACE emoji)\n";

        std::cout << " - Greek letter U+03A9: ";
        print_char32(U'\u03A9');
        std::cout << " (expected: GREEK CAPITAL LETTER OMEGA)\n";

        std::cout << " - Chinese character U+8ECA: ";
        print_char32(U'\u8ECA');
        std::cout << " (expected: CJK character for CHE )\n";

        std::cout << " - Chinese character U+8F66: ";
        print_char32(U'\u8F66');
        std::cout << " (expected: simplified CJK character for CHE )\n";

        std::cout << " - Chess white queen symbol U+2655: ";
        print_char32(U'\u2655');
        std::cout << " (expected: WHITE CHESS QUEEN symbol)\n";

        std::cout << " - Chess black queen symbol U+265B: ";
        print_char32(U'\u265B');
        std::cout << " (expected: BLACK CHESS QUEEN symbol)\n";

            std::cout << "print_char32() manual checks complete.\n" << std::endl;
    }

    /**
     * @brief Runs all test suites for Tools module
     */
    void run_all_tools_tests()
    {
        print_test_banner("CS111::Tools Test Harness");

        std::cout <<" welcome !!!\n";

        // Run individual test suites
        test_print_char32();
        test_clear_input_queue();
        test_read_int();
       

        print_test_separator("Summary");
        std::cout << "Manual tools tests finished.\n";
    }
} // namespace CS111::Test



// Main entry point for test executable
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::setlocale(LC_ALL, ".UTF-8");

    // The following two lines may improve performance of cin/cout, with large I/O. But it cause halting, cout does not show printing. Need to add cout.flush() after each cout statement to make sure the output is printed before halting.
    // Commenting out the following lines to avoid halting issue in some environments.

    // Optimizations for faster I/O (but risky if misused)
    // std::ios_base::sync_with_stdio(false); // Disables synchronization between C/C++ stdio (e.g., printf <-> cout), for better I/O performance (optional but common)
    // std::cin.tie(nullptr);                 // Unlinks cin from cout (cout no longer flushes before cin reads input)

    // Run all test suites
    CS111::Test::run_all_tools_tests();
    std::cout << "   程序运行完成！作者：王子昂" << std::endl;
    return 0;
}