/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 2
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 * 没用结构化绑定 把 auto [x,y] 改成用 std::pair 接收返回值，然后手动 .first/.second 
 * 包括后面修复 print_centered_text 里的 left_padding / right_padding 
 * 修复 RowProxy / ConstRowProxy 里的 auto [x,y] 都写成了C++17也兼容的形式 原写法给注释掉了
 *****************************************************/

#include "../Include/board.hpp"
#include "../Include/tool.hpp"

#include <cctype>
#include <iomanip>
#include <stdexcept>
#include <string>

namespace
{
    int console_display_width(char32_t symbol)
    {
        if (symbol == U'\0')
        {
            return 0;
        }

        if (symbol < 0x1100)
        {
            return 1;
        }

        const bool is_wide =
            (symbol >= 0x1100 && symbol <= 0x115F) ||
            (symbol >= 0x2E80 && symbol <= 0xA4CF) ||
            (symbol >= 0xAC00 && symbol <= 0xD7A3) ||
            (symbol >= 0xF900 && symbol <= 0xFAFF) ||
            (symbol >= 0xFE10 && symbol <= 0xFE19) ||
            (symbol >= 0xFE30 && symbol <= 0xFE6F) ||
            (symbol >= 0xFF00 && symbol <= 0xFF60) ||
            (symbol >= 0xFFE0 && symbol <= 0xFFE6);

        return is_wide ? 2 : 1;
    }

    std::string column_label(int x, CS111::BOARD_GAME::MarkStyle ms)
    {
        if (ms == CS111::BOARD_GAME::MarkStyle::Professional)
        {
            int letter_index = x;
            if (letter_index >= 9)
            {
                ++letter_index;
            }
            return std::string(1, static_cast<char>('A' + letter_index - 1));
        }

        if (ms == CS111::BOARD_GAME::MarkStyle::Easy)
        {
            return std::to_string(x);
        }

        return std::string();
    }
}

namespace CS111
{
    namespace BOARD_GAME
    {
        Board::Board(int width, int height, const Stones &stones)
            : stones_catalog_(stones),
              width_(width),
              height_(height),
              mark_style_(MarkStyle::Professional),
              stone_pos_style_(StonePositionStyle::AtCross),
              grid_style_(GridStyle::NoLines),
              empty_stone_({symbols[sizeof(symbols) / sizeof(Symbol) - 1], Color::EMPTY, Color::EMPTY, Shape::CIRCLE})
        {
            if (width_ <= 0 || height_ <= 0)
            {
                throw std::invalid_argument("Board size must be positive");
            }

            for (int x = 0; x < width_; ++x)
            {
                MyVector<Stone> column;
                for (int y = 0; y < height_; ++y)
                {
                    column.push_back(empty_stone_);
                }
                state_.push_back(column);
            }
        }

        void Board::validate_xy(int x, int y) const
        {
            // !!!!!!!!!!!!!! < Task 14 > !!!!!!!!!!!!!!
            //  Implement the method according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if (x < 1 || x > width_ || y < 1 || y > height_)
            {
                throw std::out_of_range("Board coordinates out of bounds");
            }
        }

        // x is the column number (1-based), and y is the row number (1-based). For example, for position <5, D>, x is 4 (D is the 4th column) and y is 5 (the row number).
        Stone &Board::at_xy(int x, int y)
        {
            validate_xy(x, y);
            // bug, state_[0] will return the first row, not the first column. So, the column at row y and column x should be state_[y-1][x-1], not state_[x-1][y-1].
            // return state_[x - 1][y - 1];
            return state_[y - 1][x - 1];
        }

        const Stone &Board::at_xy(int x, int y) const
        {
            validate_xy(x, y);
            // return state_[x - 1][y - 1];
            return state_[y - 1][x - 1];
        }

        std::pair<int, int> Board::to_xy(int row, char col) const
        {
            // !!!!!!!!!!!!!! < Task 15 > !!!!!!!!!!!!!!
            //  Implement the method according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
           if (col < 'A' || col > 'Z' || col == 'I' || col == 'i')
            {
                throw std::out_of_range("Invalid column label");
            }
            col = std::toupper(col);
            int x = col - 'A' + 1;
            if (col > 'I') {
                x -= 1; 
            }
            validate_xy(x, row);
            return {x, row};
        }

        const Stone &Board::get_stone(int x, int y) const
        {
            return at_xy(x, y);
        }

        void Board::set_stone(int x, int y, const Stone &stone)
        {
            at_xy(x, y) = stone;
        }

        // if x and y are valid, set the stone at (x, y) to empty_stone_. If the coordinates are invalid, it should throw an out_of_range exception with a clear error message. Hint: the at_xy function can be helpful.
        void Board::remove_stone(int x, int y)
        {
            // !!!!!!!!!!!!!! < Task 16 > !!!!!!!!!!!!!!
            //  Implement the method according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            validate_xy(x, y);
            at_xy(x, y) = empty_stone_;
          
        }

        // This function should set all positions on the board to empty_stone_. Hint: you can use nested loops to iterate through all positions, and use the at_xy function to set each position to empty_stone_.
        void Board::make_empty()
        {
            // !!!!!!!!!!!!!! < Task 17 > !!!!!!!!!!!!!!
            //  Implement the method according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for (int x = 1; x <= width_; ++x)
            {
                for (int y = 1; y <= height_; ++y)
                {
                    at_xy(x, y) = empty_stone_;
                }
            }
        }

        const Stone &Board::get_stone(int row, char col) const
        {
            //const auto [x, y] = to_xy(row, col);
            auto pos = to_xy(row, col);
        int x = pos.first;
        int y = pos.second;
            return at_xy(x, y);
        }

        void Board::set_stone(int row, char col, const Stone &stone)
        {
            //  structured binding declaration, C++17 feature, to unpack the pair returned by to_xy into x and y variables.
            //const auto [x, y] = to_xy(row, col);
            auto pos = to_xy(row, col);
        int x = pos.first;
        int y = pos.second;
            at_xy(x, y) = stone;
        }

        // remove stone at position specified by (row, col), for example remove_stone(5, 'D') will remove the stone at the row 5 and column D (which is the 4th column). This function should first convert the (row, col) to (x, y) using to_xy, and then set the stone at (x, y) to empty_stone_. If the input row or column is invalid, it should throw an out_of_range exception with a clear error message.
        void Board::remove_stone(int row, char col)
        {
            //const auto [x, y] = to_xy(row, col);
            auto pos = to_xy(row, col);
        int x = pos.first;
        int y = pos.second;
            at_xy(x, y) = empty_stone_;
        }

        Board::RowProxy Board::operator[](int x)
        {
            return RowProxy(*this, x);
        }

        Board::ConstRowProxy Board::operator[](int x) const
        {
            return ConstRowProxy(*this, x);
        }

        Stone &Board::RowProxy::operator[](int y)
        {
            return board_.at_xy(y, x_);
        }

        Stone &Board::RowProxy::operator[](char col)
        {
            //const auto [x, y] = board_.to_xy(x_, col);
            auto pos = board_.to_xy(x_, col);
            int x = pos.first;
            int y = pos.second;
            return board_.at_xy(x, y);
        }

        const Stone &Board::ConstRowProxy::operator[](int y) const
        {
            return board_.at_xy(y, x_);
        }

        const Stone &Board::ConstRowProxy::operator[](char col) const
        {
            //const auto [x, y] = board_.to_xy(x_, col);
            auto pos = board_.to_xy(x_, col);
            int x = pos.first;
            int y = pos.second;
            return board_.at_xy(x, y);
        }

        void Board::display_board(std::ostream &os, MarkStyle ms, StonePositionStyle sps) const
        {
            display_board(os, ms, sps, grid_style_);
        }

        void Board::display_board(std::ostream &os, MarkStyle ms, StonePositionStyle sps, GridStyle gs) const
        {
            struct CenteringPlan
            {
                int field_width;
                // Precomputed for common console glyph widths: 1 (narrow) and 2 (wide CJK).
                int left_pad_for_width_1;
                int right_pad_for_width_1;
                int left_pad_for_width_2;
                int right_pad_for_width_2;
            };

            struct Layout
            {
                bool has_marks;
                bool no_grid_lines;
                bool stones_at_blank;

                int row_label_width;
                int no_lines_column_left_margin;
                int at_blank_top_header_left_margin;
                int border_row_left_margin_with_marks;
                int easy_top_header_left_margin;
                int at_cross_horizontal_gap;

                CenteringPlan no_lines_centering;
                CenteringPlan blank_cell_centering;
            };

            const Layout layout = {
                ms != MarkStyle::NoMarks,
                gs == GridStyle::NoLines,
                sps == StonePositionStyle::AtBlank,

                2,
                3,
                4,
                3,
                2,
                3,

                // field 3 => width 1: (1,1), width 2: (0,1)
                {3, 1, 1, 0, 1},
                // field 4 => width 1: (1,2), width 2: (1,1)
                {4, 1, 2, 1, 1}};

            // Prints the stone symbol at board position (x, y) to the output stream.
            // Uses the symbol stored in the Stone object at that position.
            // return type is const auto in a lambda function means the return type will be deduced, and the reeturned value cannot be changed.
            // In contrast, if the return type is auto, it also mean the return type will be deduced and the returned value can be changed. 
            const auto print_stone = [this, &os](int x, int y)
            {
                CS111::Tools::write_symbol(os, at_xy(x, y).symbol.symbol);
            };

            // Returns the display width (in console columns) of the stone symbol at (x, y).
            // Handles wide Unicode symbols for proper alignment.
            const auto stone_width = [this](int x, int y)
            {
                return console_display_width(at_xy(x, y).symbol.symbol);
            };

            // Prints the row label (number) for row y, if marks are enabled.
            // Uses setw for alignment and adds a space after the label.
            const auto print_row_label = [&](int y)
            {
                if (!layout.has_marks)
                {
                    return;
                }
                os << std::setw(layout.row_label_width) << y << ' ';
            };

            // Computes left and right padding needed to center content of given width in a field.
            // Returns a pair: (left_padding, right_padding).
            // If content is wider than field, no padding is added.
            const auto compute_center_padding = [](int field_width, int content_width)
            {
                const int total_padding = field_width - content_width;
                const int left_padding = total_padding > 0 ? total_padding / 2 : 0;
                const int right_padding = total_padding > 0 ? total_padding - left_padding : 0;
                return std::pair<int, int>{left_padding, right_padding};
            };

            // Prints the column labels (A, B, ..., or 1, 2, ...) at the top of the board.
            // left_margin: spaces before the first label.
            // field_width: width of each label field.
            // center_labels: if true, centers each label in its field.
            const auto print_column_labels = [&](int left_margin, int field_width, bool center_labels = false)
            {
                if (!layout.has_marks)
                {
                    return;
                }

                os << std::string(left_margin, ' ');
                for (int x = 1; x <= width_; ++x)
                {
                    const std::string label = column_label(x, ms);
                    if (!center_labels)
                    {
                        os << std::setw(field_width) << label;
                    }
                    else
                    {
                        // Center the label in the field using computed padding.
                        //const auto [left_padding, right_padding] =
                            //compute_center_padding(field_width, static_cast<int>(label.size()));
                 auto pad = compute_center_padding(field_width, static_cast<int>(label.size()));
                    int left_padding = pad.first;
                    int right_padding = pad.second;

                        os << std::string(left_padding, ' ');
                        os << label;
                        os << std::string(right_padding, ' ');
                    }
                }
                os << '\n';
            };


            // !!!!!!!!!!!!!! < Task 18 > !!!!!!!!!!!!!!
            //  Implement the lambda function print_centered_text, according to the specification below
            const auto print_centered_text = [&](const std::string& text, int field_width)
            {
                //const auto [left_padding, right_padding] = compute_center_padding(field_width, static_cast<int>(text.size()));
                auto pad = compute_center_padding(field_width, (int)text.size());
                 int left_padding = pad.first;
                 int right_padding = pad.second;
                os << std::string(left_padding, ' ') << text << std::string(right_padding, ' ');
            };// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            /**
             * @brief print_centered_text: A lambda function to print text centered in a field of given width. 
             * @param text The string to be printed.
             * @param field_width The width of the field in which to center the text.
             * @return void, can also be, const auto. 
             * @note It captures everything. 
             * @note - [Computation] is as follows
             * @note -- call the compute_center_padding function to get the left and right padding needed, which are numbers representing the number of space characters, to center the text in the field. 
             * @note -- then it prints the left padding spaces, followed by the text, and then the right padding spaces.  
             * @note - [Usage]
             * @note -- This is useful for aligning column labels or other text elements in the board display, especially when the field width is greater than the text width. If the text is wider than the field, it will simply print the text without padding.
             * @note -- This function is used in the display_board method to ensure that column labels and other text elements are properly centered in their respective fields, contributing to a neat and organized board display.
             */
            


            // Prints the stone at (x, y) centered in a field, using the provided CenteringPlan.
            // Handles both narrow and wide symbols, and falls back to computed padding for rare cases.
            const auto print_centered_stone = [&](int x, int y, const CenteringPlan &plan)
            {
                const int symbol_w = stone_width(x, y);

                int left_padding = 0;
                int right_padding = 0;

                if (symbol_w == 1)
                {
                    left_padding = plan.left_pad_for_width_1;
                    right_padding = plan.right_pad_for_width_1;
                }
                else if (symbol_w == 2)
                {
                    left_padding = plan.left_pad_for_width_2;
                    right_padding = plan.right_pad_for_width_2;
                }
                else
                {
                    // Fallback: compute padding for unusual symbol widths.
                    //const auto [computed_left, computed_right] =
                        //compute_center_padding(plan.field_width, symbol_w);
                    auto pad = compute_center_padding(plan.field_width, symbol_w);
                        int computed_left = pad.first;
                        int computed_right = pad.second;
                    left_padding = computed_left;
                    right_padding = computed_right;
                }

                os << std::string(left_padding, ' ');
                print_stone(x, y);
                os << std::string(right_padding, ' ');
            };

            if (layout.no_grid_lines)
            {
                // print the column labels for NoLines style, which are centered in a field of width 3, with a left margin of 3 spaces before the first label.
                print_column_labels(layout.no_lines_column_left_margin,
                                    layout.no_lines_centering.field_width,
                                    true);
                
                  // !!!!!!!!!!!!!! < Task 19 > !!!!!!!!!!!!!!
                 //  provide the missing code according to the comments. 
                 // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                 
                 /* The computation: 
                 for each row ( there are height_ rows), print the row as follows: 
                    - first print the row label (number) for the current row, using the print_row_label lambda function.
                    - then for each column in the current row (there are width_ columns), print the stone at that position using the print_centered_stone lambda function, with the centering plan for no_lines style.
                       Hint: layout.no_lines_centering is the CenteringPlan struct instance for no_lines style. 
                    - after printing all columns in the current row, print a newline character.
                 */
                // ----- missing code of task 19 start -----
                    for (int y = 1; y <= height_; ++y)
                {
                    print_row_label(y);
                    for (int x = 1; x <= width_; ++x)
                    {
                        print_centered_stone(x, y, layout.no_lines_centering);
                    }
                    os << '\n';
                }

                // ---------- missing code of task 19 end --- 
                return;
            }

            if (layout.stones_at_blank)
            {
                if (layout.has_marks)
                {
                    os << std::string(layout.at_blank_top_header_left_margin, ' ');
                    for (int x = 1; x <= width_; ++x)
                    {
                        print_centered_text(column_label(x, ms), layout.blank_cell_centering.field_width + 1);
                    }
                    os << '\n';
                }

                const auto print_horizontal_border = [&]()
                {
                    if (layout.has_marks)
                    {
                        os << std::string(layout.border_row_left_margin_with_marks, ' ');
                    }
                    for (int x = 1; x <= width_; ++x)
                    {
                        os << '+' << std::string(layout.blank_cell_centering.field_width, '-');
                    }
                    os << "+\n";
                };

                for (int y = 1; y <= height_; ++y)
                {
                    print_horizontal_border();
                    print_row_label(y);

                    for (int x = 1; x <= width_; ++x)
                    {
                        os << '|';
                        print_centered_stone(x, y, layout.blank_cell_centering);
                    }
                    os << "|\n";
                }

                print_horizontal_border();
                return;
            }

            if (layout.has_marks)
            {
                if (ms == MarkStyle::Easy)
                {
                    os << std::string(layout.easy_top_header_left_margin, ' ');
                    for (int x = 1; x <= width_; ++x)
                    {
                        print_centered_text(column_label(x, ms), 4);
                    }
                    os << '\n';
                }
                else
                {
                    os << std::string(layout.no_lines_column_left_margin, ' ');
                    for (int x = 1; x <= width_; ++x)
                    {
                        os << column_label(x, ms);
                        if (x != width_)
                        {
                            os << std::string(layout.at_cross_horizontal_gap, ' ');
                        }
                    }
                    os << '\n';
                }
            }

            for (int y = 1; y <= height_; ++y)
            {
                print_row_label(y);
                for (int x = 1; x <= width_; ++x)
                {
                    if (at_xy(x, y).symbol.symbol == empty_stone_.symbol.symbol)
                    {
                        CS111::Tools::write_symbol(os, U'\u00B7');
                    }
                    else
                    {
                        print_stone(x, y);
                    }

                    if (x != width_)
                    {
                        os << std::string(layout.at_cross_horizontal_gap, ' ');
                    }
                }
                os << '\n';

                if (y != height_)
                {
                    if (layout.has_marks)
                    {
                        os << std::string(layout.border_row_left_margin_with_marks, ' ');
                    }
                    os << '\n';
                }
            }
        }

        void Board::display_board(std::ostream &os) const
        {
            display_board(os, mark_style_, stone_pos_style_, grid_style_);
        }
    }
}
