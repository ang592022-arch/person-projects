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

#include <cstdlib>
#include <iostream>
#include <clocale>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../Include/board.hpp"
#include "../Include/board_presets.hpp"
#include "../Include/tool.hpp"

using namespace CS111::BOARD_GAME;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;
using CS111::Tools::Test::TestContext;

namespace
{
    int count_char_occurrences(const std::string &text, char ch)
    {
        int count = 0;
        for (char c : text)
        {
            if (c == ch)
            {
                ++count;
            }
        }
        return count;
    }

    bool string_contains(const std::string &text, const std::string &needle)
    {
        return text.find(needle) != std::string::npos;
    }

    bool board_is_empty(const Board &board)
    {
        for (int y = 1; y <= board.get_height(); ++y)
        {
            for (int x = 1; x <= board.get_width(); ++x)
            {
                if (board.get_stone(x, y).symbol.symbol != U' ')
                {
                    return false;
                }
            }
        }
        return true;
    }

    void print_board_block(const std::string &label, const Board &board,
                           MarkStyle mark_style, GridStyle grid_style)
    {
        std::cout << "\n[" << label << "]\n";
        board.display_board(std::cout, mark_style, board.get_stone_position_style(), grid_style);
        std::cout << "\n";
    }

    void print_three_styles(const Board &board)
    {
        print_board_block("NoLines + NoMarks", board, MarkStyle::NoMarks, GridStyle::NoLines);
        print_board_block("NoLines + Professional", board, MarkStyle::Professional, GridStyle::NoLines);
        print_board_block("WithLines + NoMarks", board, MarkStyle::NoMarks, GridStyle::WithLines);
        print_board_block("WithLines + Professional", board, MarkStyle::Professional, GridStyle::WithLines);
        print_board_block("WithLines + Easy", board, MarkStyle::Easy, GridStyle::WithLines);
    }
}

int main()
{

    // On windows, when symbol output is going through UTF-16 WriteConsoleW, the following two Set function calls are mostly not needed.
    // But if printing unicode is still through cout, a narrow output stream, then these calls will help ensure that the console correctly interprets the UTF-8 encoded bytes written by cout, allowing the Unicode symbols to display properly.

    /*
    Windows console historically defaults to legacy code pages (for example 437, 936), not UTF-8.
    A code page is a system used by computers (especially Windows) to map byte values to specific characters in a character set. Each code page defines a table of characters for a particular language or group of languages. For example:

    Code page 437: Original IBM PC (US English) character set, includes ASCII plus box-drawing and special symbols.
    Code page 936: Simplified Chinese (GBK).
    Code page 65001: UTF-8 (Unicode).
    On Windows, the console uses a code page to interpret and display text. If the code page is not set to UTF-8 (65001), some Unicode characters (like emoji or CJK symbols) may not display correctly. Setting the code page to 65001 allows the console to handle UTF-8 encoded text, supporting a much wider range of characters.

    With the following two SetConsole* calls, it is not necessary to do
     chcp 65001
    on windows cmd
    */

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Tells the console to interpret bytes written by narrow output (like std::cout) as UTF-8.
    SetConsoleCP(CP_UTF8);       // Tells the console to interpret keyboard input bytes (narrow input path) as UTF-8.
#endif

    std::setlocale(LC_ALL, "");
    TestContext ctx;

    print_test_banner("BOARD CLASS TEST SUITE");

    try
    {
        // -----------------------------------------------------------------
        // Test Task 1: Core coordinate APIs and operator[][]
        // -----------------------------------------------------------------
        print_test_separator("1. Core APIs and operator[][]");
        std::cout << "[COVERAGE] Direct: operator[][], set/get/remove with (x, y), set/get/remove with (row, col), make_empty\n";
        std::cout << "[COVERAGE] Indirect (via presets): repeated placement patterns in make_*_board helpers\n";

        Board board(9, 9);
        Stone black = {symbols[0], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
        Stone white = {symbols[1], Color::WHITE, Color::EMPTY, Shape::CIRCLE};

        board[3][4] = black;
        check_true(ctx, board[3][4].symbol.symbol == black.symbol.symbol,
                   "operator[][] can set/get stone");

        board.set_stone(2, 6, black);
        check_true(ctx, board.get_stone(2, 6).symbol.symbol == black.symbol.symbol,
                   "set_stone/get_stone with (x, y) works");

        board.remove_stone(2, 6);
        check_true(ctx, board.get_stone(2, 6).symbol.symbol == U' ',
                   "remove_stone with (x, y) resets to empty stone");

        board.set_stone(5, 'D', white);
        check_true(ctx, board.get_stone(5, 'D').symbol.symbol == white.symbol.symbol,
                   "set_stone/get_stone with (row, col) works");

        board.remove_stone(5, 'D');
        check_true(ctx, board.get_stone(5, 'D').symbol.symbol == U' ',
                   "remove_stone with (row, col) resets to empty stone");

        board.make_empty();
        check_true(ctx, board_is_empty(board),
                   "make_empty clears every position on the board");

        bool caught_out_of_range = false;
        try
        {
            (void)board[10][1];
        }
        catch (const std::out_of_range &)
        {
            caught_out_of_range = true;
        }
        check_true(ctx, caught_out_of_range, "out-of-range access throws exception");

        // -----------------------------------------------------------------
        // Test Task 2: Chinese chess board preset
        // -----------------------------------------------------------------
        print_test_separator("2. Chinese Chess Board Preset");

        Board chinese = make_chinese_chess_board();
        check_true(ctx, chinese.get_width() == 9 && chinese.get_height() == 10,
                   "Chinese board size is 9x10");
        check_true(ctx, chinese.get_stone_position_style() == StonePositionStyle::AtBlank,
                   "Chinese board uses AtBlank");
        check_true(ctx, chinese.get_grid_style() == GridStyle::NoLines,
                   "Chinese board starts with NoLines grid style");

        std::cout << "Chinese board display in separated styles:\n";
        print_three_styles(chinese);

        // -----------------------------------------------------------------
        // Test Task 3: Western chess board preset
        // -----------------------------------------------------------------
        print_test_separator("3. Western Chess Board Preset");

        Board western = make_western_chess_board();
        check_true(ctx, western.get_width() == 8 && western.get_height() == 8,
                   "Western board size is 8x8");
        check_true(ctx, western.get_stone_position_style() == StonePositionStyle::AtBlank,
                   "Western board uses AtBlank");
        check_true(ctx, western.get_grid_style() == GridStyle::NoLines,
                   "Western board starts with NoLines grid style");

        std::cout << "Western board display in separated styles:\n";
        print_three_styles(western);

        // -----------------------------------------------------------------
        // Test Task 4: Go board preset
        // -----------------------------------------------------------------
        Board go = make_go_board();

        {
            print_test_separator("4. Go Board Preset");

            // Board go = make_go_board();
            check_true(ctx, go.get_width() == 19 && go.get_height() == 19,
                       "Go board size is 19x19");
            check_true(ctx, go.get_stone_position_style() == StonePositionStyle::AtCross,
                       "Go board uses AtCross");
            check_true(ctx, go.get_grid_style() == GridStyle::NoLines,
                       "Go board starts with NoLines grid style");

            check_true(ctx, go.get_stone(10, 10).symbol.symbol == U' ',
                       "Go center star stays empty");

            std::cout << "We put 8 stones on the Go board, to make it more interesint for display testing. " << std::endl;
            /*
            const Stone black = make_stone(IDX_GO_BLACK, Color::BLACK);
            const Stone white = make_stone(IDX_GO_WHITE, Color::WHITE);
            */
            const Stone black = go.get_stones_catalog().getStone(0); // Black Go stone symbol
            const Stone white = go.get_stones_catalog().getStone(1); // White Go stone symbol

            // bug fixed. board is 9x9, set_stone(4, 16, black) and set_stone(16, 4, black) will cause out-of-range error.

            // board.set_stone(4, 4, black);
            // board.set_stone(4, 16, black);
            go.set_stone(16, 4, black);
            go.set_stone(16, 16, black);

            go.set_stone(4, 10, white);
            go.set_stone(10, 4, white);
            go.set_stone(10, 16, white);
            go.set_stone(16, 10, white);

            std::cout << "Go board display in separated styles:\n";
            print_three_styles(go);
        }
        // -----------------------------------------------------------------
        // Test Task 5: RowProxy char-index and mixed indexing styles
        // -----------------------------------------------------------------
        print_test_separator("5. RowProxy char-index and mixed styles");

        const auto color_name = [](Color c)
        {
            switch (c)
            {
            case Color::EMPTY:
                return "EMPTY";
            case Color::BLACK:
                return "BLACK";
            case Color::WHITE:
                return "WHITE";
            case Color::RED:
                return "RED";
            }
            return "UNKNOWN";
        };

        const auto shape_name = [](Shape s)
        {
            switch (s)
            {
            case Shape::CIRCLE:
                return "CIRCLE";
            case Shape::SQUARE:
                return "SQUARE";
            case Shape::TRIANGLE:
                return "TRIANGLE";
            }
            return "UNKNOWN";
        };

        std::cout << "Go board before manual placements (Task 5 demo):\n";
        print_board_block("Go before Task 5 operations", go, MarkStyle::Professional, GridStyle::WithLines);

        std::cout << "Placing stones now:\n";
        std::cout << "  - WHITE at [5]['D'] (same as [5][4])\n";
        std::cout << "  - BLACK at [10][10] (center)\n";
        std::cout << "  - WHITE at [1]['A'] (corner)\n";
        std::cout << "  - BLACK at [7]['K'] (common position)\n";

        go[5]['D'] = black;
        check_true(ctx, go[5]['D'].symbol.symbol == black.symbol.symbol,
                   "operator[](char) supports professional column labels");
        check_true(ctx, go.get_stone(5, 'D').symbol.symbol == black.symbol.symbol,
                   "char index writes can be observed by get_stone(row, col)");

        go[5][4] = white;
        check_true(ctx, go[5][4].symbol.symbol == white.symbol.symbol,
                   "operator[](int) still supports easy numeric indexing");
        check_true(ctx, go[5]['D'].symbol.symbol == white.symbol.symbol,
                   "operator[](int) and operator[](char) target the same board position for row/column input");

        go[10][10] = black;
        check_true(ctx, go[10][10].symbol.symbol == black.symbol.symbol,
                   "can place a black stone at the board center");

        go[1]['A'] = white;
        check_true(ctx, go[1]['A'].symbol.symbol == white.symbol.symbol,
                   "can place a white stone at a corner position");

        go[7]['K'] = black;
        check_true(ctx, go[7]['K'].symbol.symbol == black.symbol.symbol,
                   "can place a black stone at a common interior position");

        std::cout << "Go board after placements at [5]['D'] (same as [5][4]), [10][10], [1]['A'], and [7]['K']:\n";
        print_board_block("Go after Task 5 operations", go, MarkStyle::Professional, GridStyle::WithLines);

        const Stone &stone_prof = go[5]['D'];
        const Stone &stone_easy = go[5][4];
        const Stone &stone_center = go[10][10];
        const Stone &stone_corner = go[1]['A'];
        const Stone &stone_common = go[7]['K'];
        const Stone &stone_empty = go[2]['B'];
        std::cout << "Read stone at [5]['D'] => symbol=" << stone_prof.symbol.name
                  << ", symbolColor=" << color_name(stone_prof.symbolColor)
                  << ", backgroundColor=" << color_name(stone_prof.backgroundColor)
                  << ", shape=" << shape_name(stone_prof.shape) << "\n";
        std::cout << "Read stone at [5][4] => symbol=" << stone_easy.symbol.name
                  << ", symbolColor=" << color_name(stone_easy.symbolColor)
                  << ", backgroundColor=" << color_name(stone_easy.backgroundColor)
                  << ", shape=" << shape_name(stone_easy.shape) << "\n";
        std::cout << "Read stone at [10][10] => symbol=" << stone_center.symbol.name
                  << ", symbolColor=" << color_name(stone_center.symbolColor)
                  << ", backgroundColor=" << color_name(stone_center.backgroundColor)
                  << ", shape=" << shape_name(stone_center.shape) << "\n";
        std::cout << "Read stone at [1]['A'] => symbol=" << stone_corner.symbol.name
                  << ", symbolColor=" << color_name(stone_corner.symbolColor)
                  << ", backgroundColor=" << color_name(stone_corner.backgroundColor)
                  << ", shape=" << shape_name(stone_corner.shape) << "\n";
        std::cout << "Read stone at [7]['K'] => symbol=" << stone_common.symbol.name
                  << ", symbolColor=" << color_name(stone_common.symbolColor)
                  << ", backgroundColor=" << color_name(stone_common.backgroundColor)
                  << ", shape=" << shape_name(stone_common.shape) << "\n";
        std::cout << "Read stone at [2]['B'] (expected empty) => symbol=" << stone_empty.symbol.name
                  << ", symbolColor=" << color_name(stone_empty.symbolColor)
                  << ", backgroundColor=" << color_name(stone_empty.backgroundColor)
                  << ", shape=" << shape_name(stone_empty.shape) << "\n";

        check_true(ctx, stone_empty.symbol.symbol == U' ',
                   "readback includes an empty position example");

        const Board &const_go = go;
        check_true(ctx, const_go[5]['D'].symbol.symbol == white.symbol.symbol,
                   "ConstRowProxy operator[](char) supports read access");

        bool caught_invalid_i = false;
        try
        {
            (void)go[2]['i'];
        }
        catch (const std::out_of_range &)
        {
            caught_invalid_i = true;
        }
        check_true(ctx, caught_invalid_i, "lowercase 'i' is rejected as an invalid professional label");

        bool caught_invalid_I = false;
        try
        {
            (void)go[2]['I'];
        }
        catch (const std::out_of_range &)
        {
            caught_invalid_I = true;
        }
        check_true(ctx, caught_invalid_I, "uppercase 'I' is rejected as an invalid professional label");

        // -----------------------------------------------------------------
        // Test Task 6: make_empty on preset boards
        // -----------------------------------------------------------------
        print_test_separator("6. make_empty on preset boards");

        Board cleared_chinese = chinese;
        cleared_chinese.make_empty();
        check_true(ctx, board_is_empty(cleared_chinese),
                   "make_empty clears a populated Chinese chess board");
        check_true(ctx, cleared_chinese.get_width() == chinese.get_width() && cleared_chinese.get_height() == chinese.get_height(),
                   "make_empty keeps board dimensions unchanged");
        check_true(ctx, cleared_chinese.get_stone_position_style() == chinese.get_stone_position_style() && cleared_chinese.get_grid_style() == chinese.get_grid_style(),
                   "make_empty keeps board display styles unchanged");

        // -----------------------------------------------------------------
        // Test Task 7: Copy and assignment behavior
        // -----------------------------------------------------------------
        print_test_separator("7. Copy and Assignment");

        Board copied = chinese;
        copied[1][1] = Stone{symbols[0], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
        check_true(ctx, chinese[1][1].symbol.symbol != copied[1][1].symbol.symbol,
                   "Copy construction keeps independent board state");

        Board assigned(9, 10);
        assigned = chinese;
        check_true(ctx, assigned.get_stone(5, 1).symbol.symbol == chinese.get_stone(5, 1).symbol.symbol,
                   "Copy assignment copies board content");

        // -----------------------------------------------------------------
        // Test Task 8: Display layout invariants
        // -----------------------------------------------------------------
        print_test_separator("8. Display layout invariants");

        Board display_probe(3, 2);

        std::ostringstream with_lines_blank_stream;
        display_probe.display_board(with_lines_blank_stream,
                                    MarkStyle::NoMarks,
                                    StonePositionStyle::AtBlank,
                                    GridStyle::WithLines);

        const std::string with_lines_blank = with_lines_blank_stream.str();
        check_true(ctx, string_contains(with_lines_blank, "+----+----+----+\n"),
                   "AtBlank + WithLines contains expected horizontal border pattern");
        check_true(ctx, count_char_occurrences(with_lines_blank, '+') == 12,
                   "AtBlank + WithLines has expected number of '+' border intersections");
        check_true(ctx, count_char_occurrences(with_lines_blank, '|') == 8,
                   "AtBlank + WithLines has expected number of vertical border lines");

        std::ostringstream no_lines_stream;
        display_probe.display_board(no_lines_stream,
                                    MarkStyle::NoMarks,
                                    StonePositionStyle::AtCross,
                                    GridStyle::NoLines);

        const std::string no_lines = no_lines_stream.str();
        check_true(ctx, count_char_occurrences(no_lines, '+') == 0 && count_char_occurrences(no_lines, '|') == 0,
                   "NoLines mode does not print grid border characters");

        // interactive testing

        /* -----------------------------------------------------------------
         Test Task 9: Interactive testing
         * Create a go board, call it small_go,  with 9x9 size and WithLines grid style, professional mark style, and AtCross stone position style.
         * Print the board small_go.
         * Ask the user to input three stones, by repeating three times of the following :
            - Ask for the position and color of a stone. For example, the user can input "5 D black" to place a black stone at row 5 and column D. The program should validate the user input, and if the input is invalid, ask the user to re-enter until a valid input is received.  Note: for column input, only uppercase letters are accepted, and 'I' is not a valid column label. So for a 9x9 board, valid column labels are A, B, C, D, E, F, G, H, J.
            - put the stone on the board (by calling the set_stone function).
        - print the board, so we can see the three stones on the board.
        - Remove the second stone (second according to the order of user's input), and print the board again, so we can verify that only the first and third stone remain on the board.
                  ----------------------------------------------------------------- */
        print_test_separator("9. Interactie testing (manual verification)");

        // !!!!!!!!!!!!!! < Task 20 > !!!!!!!!!!!!!!
        //  provide the missing code of Test Task 9 according to the comments.
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // ----- missing code of task 20 start -----
        Board small_go(9, 9, make_go_stones());
        small_go.set_mark_style(MarkStyle::Professional);
        small_go.set_grid_style(GridStyle::WithLines);
        small_go.set_stone_position_style(StonePositionStyle::AtCross);

        std::cout << "Initial 9x9 Go Board:\n";
        small_go.display_board(std::cout);

        struct InputRecord { int r; char c; };
        InputRecord records[3];

        for (int i = 0; i < 3; ++i)
        {
            while (true)
            {
                std::cout << "\nEnter stone " << (i + 1) << " (format: Row Col Color, e.g., '5 D black'): ";
                int row;
                char col;
                std::string color_str;
                
                std::cin >> row >> col >> color_str;

                if (std::cin.fail())
                {
                    CS111::Tools::clear_input_queue();
                    std::cout << "Invalid input format. Please try again.\n";
                    continue;
                }
                CS111::Tools::clear_input_queue(); 

                if (color_str != "black" && color_str != "white")
                {
                    std::cout << "Invalid color. Must be 'black' or 'white'. Please try again.\n";
                    continue;
                }

                try
                {
                    const Stone& st = (color_str == "black") ? 
                        small_go.get_stones_catalog().getStone(0) : 
                        small_go.get_stones_catalog().getStone(1);
                    
                    small_go.set_stone(row, col, st);
                    records[i] = {row, col};
                    break; 
                }
                catch (const std::exception& e)
                {
                    std::cout << "Error: " << e.what() << ". Please try again.\n";
                }
            }
        }

        std::cout << "\nBoard after 3 stones placed:\n";
        small_go.display_board(std::cout);

        std::cout << "\nRemoving the second stone at (" << records[1].r << " " << records[1].c << ")...\n";
        small_go.remove_stone(records[1].r, records[1].c);
        
        std::cout << "\nBoard after removal of the second stone:\n";
        small_go.display_board(std::cout);
    

        // ----- missing code of task 20 end -----

    } //---------------------- end of try block -------------------
    catch (const std::exception &e)
    {
        ++ctx.failed_checks;
        std::cout << "[FAIL] Unexpected exception: " << e.what() << "\n";
    }

    print_test_separator("Summary");
    if (ctx.failed_checks == 0)
    {
        std::cout << "All checks passed.\n";
        return 0;
    }

    std::cout << "Total failed checks: " << ctx.failed_checks << "\n";
    std::cout << "   程序运行完成！作者：王子昂" << std::endl;
    return EXIT_FAILURE;
    
}
