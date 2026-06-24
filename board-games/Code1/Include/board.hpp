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

#ifndef BOARD_HPP
#define BOARD_HPP

#include "stones.hpp"
#include "myVector.hpp"
#include <ostream>
#include <string>
#include <utility>
// #include <vector>

namespace CS111
{
    namespace BOARD_GAME
    {
        enum class MarkStyle
        {
            NoMarks,
            Professional,
            Easy
        };

        enum class StonePositionStyle
        {
            AtBlank,
            AtCross
        };

        enum class GridStyle
        {
            NoLines,
            WithLines
        };

        class Board
        {
        public:
            class RowProxy
            {
            public:
                RowProxy(Board &board, int x) : board_(board), x_(x) {}
                Stone &operator[](int y);
                Stone &operator[](char col);

            private:
                Board &board_;
                int x_;
            };

            class ConstRowProxy
            {
            public:
                ConstRowProxy(const Board &board, int x) : board_(board), x_(x) {}
                const Stone &operator[](int y) const;
                const Stone &operator[](char col) const;

            private:
                const Board &board_;
                int x_;
            };

            Board(int width = 19, int height = 19, const Stones &stones = Stones{});

            int get_width() const noexcept { return width_; }
            int get_height() const noexcept { return height_; }
            MarkStyle get_mark_style() const noexcept { return mark_style_; }
            StonePositionStyle get_stone_position_style() const noexcept { return stone_pos_style_; }
            GridStyle get_grid_style() const noexcept { return grid_style_; }

            void set_mark_style(MarkStyle ms) noexcept { mark_style_ = ms; }
            void set_stone_position_style(StonePositionStyle sps) noexcept { stone_pos_style_ = sps; }
            void set_grid_style(GridStyle gs) noexcept { grid_style_ = gs; }

            const Stone &get_stone(int x, int y) const;
            void set_stone(int x, int y, const Stone &stone);
            void remove_stone(int x, int y);
            void make_empty();

            const Stone &get_stone(int row, char col) const;
            void set_stone(int row, char col, const Stone &stone);
            void remove_stone(int row, char col);

            RowProxy operator[](int x);
            ConstRowProxy operator[](int x) const;

            void display_board(std::ostream &os, MarkStyle ms, StonePositionStyle sps) const;
            void display_board(std::ostream &os, MarkStyle ms, StonePositionStyle sps, GridStyle gs) const;
            void display_board(std::ostream &os) const;

            // return the copy of the stone catelog, for external use (e.g., in board presets)
            Stones get_stones_catalog() const { return stones_catalog_; }   

            friend std::ostream &operator<<(std::ostream &os, const Board &board)
            {
                board.display_board(os);
                return os;
            }

        private:
            // std::vector<std::vector<Stone>> state_;
            MyVector<MyVector<Stone>> state_;
            Stones stones_catalog_;
            int width_;
            int height_;
            MarkStyle mark_style_;
            StonePositionStyle stone_pos_style_;
            GridStyle grid_style_;
            Stone empty_stone_;

            // A helper function to validate if the given (x, y) coordinates are within the bounds of the board. If not, it throws an out_of_range exception with a clear error message. Note that x is a number of columns (1-based), and y is a number of rows (1-based). Be careful to check the bounds correctly. For example, if using a professional style of position description, for position <5, D> the x coordinate is 4 (D is the 4th column), and the y coordinate is 5 (the row number).  
            void validate_xy(int x, int y) const;

            
            /**
             * @brief A helper function to convert from (row, col) style coordinates to (x, y) style coordinates.
             * 
             * @param row a row number (1-based)
             * @param col a column letter (e.g., 'A', 'B', ..., 'H', 'J', etc.) 'I' is skipped. 
             * @return std::pair<int, int> : a pair of (x, y) coordinates, where x is the column number (1-based) and y is the row number (1-based). For example, for position <5, D>, the row is 5 and the column is 'D', and this function should return the pair (4, 5) because D is the 4th column and the row number is 5. If the input row or column is out of bounds, it should throw an out_of_range exception with a clear error message. Note that the row number is 1-based, and the column letter starts from 'A' (or 'a') as the first column. Also note that in some board games like Go, there is no column 'I', so after 'H' comes 'J'. You need to handle this special case correctly in your implementation.
             */
            std::pair<int, int> to_xy(int row, char col) const;

            // return a reference to the stone at (x, y) position. Here x is the column number (1-based), and y is the row number (1-based). For example, for position <5, D>, x is 4 (D is the 4th column) and y is 5 (the row number). This function should first validate the coordinates using validate_xy, and if they are valid, return a reference to the stone at that position. If the coordinates are invalid, it should throw an out_of_range exception with a clear error message. The const version of this function should do the same but return a const reference.
            Stone &at_xy(int x, int y);
            
            const Stone &at_xy(int x, int y) const;
        };
    }
}

#endif