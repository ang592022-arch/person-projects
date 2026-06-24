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

#include "../Include/board_presets.hpp"

namespace CS111
{
    namespace BOARD_GAME
    {
        namespace // the helper functions with internal linkages are here in the unnamed namespace .
        {
            enum SymbolIndex
            {
                IDX_GO_BLACK = 0,
                IDX_GO_WHITE = 1,

                IDX_CN_BLACK_JU = 2,
                IDX_CN_BLACK_MA = 3,
                IDX_CN_BLACK_XIANG = 4,
                IDX_CN_BLACK_SHI = 5,
                IDX_CN_BLACK_JIANG = 6,
                IDX_CN_BLACK_PAO = 7,
                IDX_CN_BLACK_ZU = 8,

                IDX_CN_RED_JU = 9,
                IDX_CN_RED_MA = 10,
                IDX_CN_RED_XIANG = 11,
                IDX_CN_RED_SHI = 12,
                IDX_CN_RED_JIANG = 13,
                IDX_CN_RED_PAO = 14,
                IDX_CN_RED_ZU = 15,

                IDX_W_KING = 16,
                IDX_W_QUEEN = 17,
                IDX_W_ROOK = 18,
                IDX_W_BISHOP = 19,
                IDX_W_KNIGHT = 20,
                IDX_W_PAWN = 21,
                IDX_B_KING = 22,
                IDX_B_QUEEN = 23,
                IDX_B_ROOK = 24,
                IDX_B_BISHOP = 25,
                IDX_B_KNIGHT = 26,
                IDX_B_PAWN = 27
            };

            Stone make_stone(int symbol_index, Color symbol_color, Shape shape = Shape::CIRCLE)
            {
                return Stone{symbols[symbol_index], symbol_color, Color::EMPTY, shape};
            }

            void place_chinese_back_row(Board &board, int row, Color color)
            {
                const int *back_row = nullptr;

                const int black_back_row[9] = {
                    IDX_CN_BLACK_JU,
                    IDX_CN_BLACK_MA,
                    IDX_CN_BLACK_XIANG,
                    IDX_CN_BLACK_SHI,
                    IDX_CN_BLACK_JIANG,
                    IDX_CN_BLACK_SHI,
                    IDX_CN_BLACK_XIANG,
                    IDX_CN_BLACK_MA,
                    IDX_CN_BLACK_JU};

                const int red_back_row[9] = {
                    IDX_CN_RED_JU,
                    IDX_CN_RED_MA,
                    IDX_CN_RED_XIANG,
                    IDX_CN_RED_SHI,
                    IDX_CN_RED_JIANG,
                    IDX_CN_RED_SHI,
                    IDX_CN_RED_XIANG,
                    IDX_CN_RED_MA,
                    IDX_CN_RED_JU};

                back_row = (color == Color::RED) ? red_back_row : black_back_row;

                for (int x = 1; x <= 9; ++x)
                {
                    board.set_stone(x, row, make_stone(back_row[x - 1], color, Shape::SQUARE));
                }
            }

            void place_western_back_row(Board &board, int row, bool is_white)
            {
                const Color color = is_white ? Color::WHITE : Color::BLACK;
                const int back_row[8] = {
                    is_white ? IDX_W_ROOK : IDX_B_ROOK,
                    is_white ? IDX_W_KNIGHT : IDX_B_KNIGHT,
                    is_white ? IDX_W_BISHOP : IDX_B_BISHOP,
                    is_white ? IDX_W_QUEEN : IDX_B_QUEEN,
                    is_white ? IDX_W_KING : IDX_B_KING,
                    is_white ? IDX_W_BISHOP : IDX_B_BISHOP,
                    is_white ? IDX_W_KNIGHT : IDX_B_KNIGHT,
                    is_white ? IDX_W_ROOK : IDX_B_ROOK};

                for (int x = 1; x <= 8; ++x)
                {
                    board.set_stone(x, row, make_stone(back_row[x - 1], color));
                }
            }
        } // end of unnamed namespace

        // this function should not belong to the unnamed namespace, because it has external linkage, used in multiple files.
        Stones make_chinese_chess_stones()
        {
            Stone arr[15];

            // Stones stones(15); // 7 black + 7 red + 1 empty
            //  chinese chess pieces are usually displayed round. The empty stone can still be a circle for visual distinction.
            arr[0] = Stone{symbols[IDX_CN_BLACK_JU], Color::BLACK, Color::EMPTY, Shape::CIRCLE}; // Shape::SQUARE);
            arr[1] = Stone{symbols[IDX_CN_BLACK_MA], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[2] = Stone{symbols[IDX_CN_BLACK_XIANG], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[3] = Stone{symbols[IDX_CN_BLACK_SHI], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[4] = Stone{symbols[IDX_CN_BLACK_JIANG], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[5] = Stone{symbols[IDX_CN_BLACK_PAO], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[6] = Stone{symbols[IDX_CN_BLACK_ZU], Color::BLACK, Color::EMPTY, Shape::CIRCLE};

            arr[7] = Stone{symbols[IDX_CN_RED_JU], Color::RED, Color::EMPTY, Shape::CIRCLE};
            arr[8] = Stone{symbols[IDX_CN_RED_MA], Color::RED, Color::EMPTY, Shape::SQUARE};
            arr[9] = Stone{symbols[IDX_CN_RED_XIANG], Color::RED, Color::EMPTY, Shape::CIRCLE};
            arr[10] = Stone{symbols[IDX_CN_RED_SHI], Color::RED, Color::EMPTY, Shape::CIRCLE};
            arr[11] = Stone{symbols[IDX_CN_RED_JIANG], Color::RED, Color::EMPTY, Shape::CIRCLE};
            arr[12] = Stone{symbols[IDX_CN_RED_PAO], Color::RED, Color::EMPTY, Shape::CIRCLE};
            arr[13] = Stone{symbols[IDX_CN_RED_ZU], Color::RED, Color::EMPTY, Shape::CIRCLE};

            // The last stone is the empty stone

            // stones.setStone(14, get_empty_stone()) ; // EMPTY stone

            arr[14] = get_empty_stone(); // EMPTY stone

            Stones stones(15, arr); // 7 black + 7 red + 1 empty

            return stones;
        }

        Stones make_western_chess_stones()
        {
            // create an array of 13 stones: 6 white pieces, 6 black pieces, and 1 empty stone
            Stone arr[13];
            // Stone x = {...} syntax is allowed, aggregate initialization.
            // In the following, aggregate initialization to create a temporary Stone object, and then assigning it to the array element.
            arr[0] = Stone{symbols[IDX_W_KING], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[1] = Stone{symbols[IDX_W_QUEEN], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[2] = Stone{symbols[IDX_W_ROOK], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[3] = Stone{symbols[IDX_W_BISHOP], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[4] = Stone{symbols[IDX_W_KNIGHT], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[5] = Stone{symbols[IDX_W_PAWN], Color::WHITE, Color::EMPTY, Shape::CIRCLE};

            /*
            for (int i = 6; i < 16; ++i)
            {
                stones.setStone(i, symbols[IDX_W_PAWN], Color::WHITE, Color::EMPTY, Shape::CIRCLE);
            }
            */

            arr[6] = Stone{symbols[IDX_B_KING], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[7] = Stone{symbols[IDX_B_QUEEN], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[8] = Stone{symbols[IDX_B_ROOK], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[9] = Stone{symbols[IDX_B_BISHOP], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[10] = Stone{symbols[IDX_B_KNIGHT], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[11] = Stone{symbols[IDX_B_PAWN], Color::BLACK, Color::EMPTY, Shape::CIRCLE};

            arr[12] = get_empty_stone(); // EMPTY stone

            Stones stones(13, arr); // 6 white + 6 black + 1 empty

            // trouble, the following statement will pass nullptr to the constructor, which is not easy to handle.
            // Stones stones(13); // 6 white + 6 black + 1 empty
            // western chess pieces are usually displayed round. The empty stone can still be a circle for visual distinction.

            return stones;
        }

        Stones make_go_stones()
        {
            Stone arr[3]; 
            arr[0] = Stone{symbols[IDX_GO_BLACK], Color::BLACK, Color::EMPTY, Shape::CIRCLE};
            arr[1] = Stone{symbols[IDX_GO_WHITE], Color::WHITE, Color::EMPTY, Shape::CIRCLE};
            arr[2] = get_empty_stone(); // EMPTY stone
            Stones stones(3, arr); // 1 black + 1 white + 1 empty
            return stones;
        }

        Board make_chinese_chess_board()
        {
            Board board(9, 10, make_chinese_chess_stones());
            board.set_stone_position_style(StonePositionStyle::AtBlank);

            place_chinese_back_row(board, 1, Color::BLACK);
            board.set_stone(2, 3, make_stone(IDX_CN_BLACK_PAO, Color::BLACK, Shape::SQUARE));
            board.set_stone(8, 3, make_stone(IDX_CN_BLACK_PAO, Color::BLACK, Shape::SQUARE));
            board.set_stone(1, 4, make_stone(IDX_CN_BLACK_ZU, Color::BLACK, Shape::SQUARE));
            board.set_stone(3, 4, make_stone(IDX_CN_BLACK_ZU, Color::BLACK, Shape::SQUARE));
            board.set_stone(5, 4, make_stone(IDX_CN_BLACK_ZU, Color::BLACK, Shape::SQUARE));
            board.set_stone(7, 4, make_stone(IDX_CN_BLACK_ZU, Color::BLACK, Shape::SQUARE));
            board.set_stone(9, 4, make_stone(IDX_CN_BLACK_ZU, Color::BLACK, Shape::SQUARE));

            place_chinese_back_row(board, 10, Color::RED);
            // the number of rows in a chinese chess board is 10
            board.set_stone(2, 8, make_stone(IDX_CN_RED_PAO, Color::RED, Shape::SQUARE));
            board.set_stone(8, 8, make_stone(IDX_CN_RED_PAO, Color::RED, Shape::SQUARE));
            board.set_stone(1, 7, make_stone(IDX_CN_RED_ZU, Color::RED, Shape::SQUARE));
            board.set_stone(3, 7, make_stone(IDX_CN_RED_ZU, Color::RED, Shape::SQUARE));
            board.set_stone(5, 7, make_stone(IDX_CN_RED_ZU, Color::RED, Shape::SQUARE));
            board.set_stone(7, 7, make_stone(IDX_CN_RED_ZU, Color::RED, Shape::SQUARE));
            board.set_stone(9, 7, make_stone(IDX_CN_RED_ZU, Color::RED, Shape::SQUARE));

            return board;
        }

        Board make_western_chess_board()
        {
            Board board(8, 8, make_western_chess_stones());
            board.set_stone_position_style(StonePositionStyle::AtBlank);

            place_western_back_row(board, 1, false);
            for (int x = 1; x <= 8; ++x)
            {
                board.set_stone(x, 2, make_stone(IDX_B_PAWN, Color::BLACK));
            }

            for (int x = 1; x <= 8; ++x)
            {
                board.set_stone(x, 7, make_stone(IDX_W_PAWN, Color::WHITE));
            }
            place_western_back_row(board, 8, true);

            return board;
        }

        Board make_go_board()
        {
            Board board(19, 19, make_go_stones());
            board.set_stone_position_style(StonePositionStyle::AtCross);

            return board;
        }
    }
}
