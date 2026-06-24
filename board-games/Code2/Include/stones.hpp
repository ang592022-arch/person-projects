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

#ifndef _STONE_HPP_
#define _STONE_HPP_

#include <string>
#include <cstdint> // For char32_tv
// for std::out_of_range
#include <stdexcept> // For std::out_of_range exception
#include <iostream>  // For std::cout in showStones()

namespace CS111
{
    namespace BOARD_GAME
    {
        /**
         * @brief Enumeration for stone color (foreground/symbol color or background color)
         * EMPTY = no stone, BLACK/WHITE/RED = standard stone colors
         */
        enum class Color
        {
            EMPTY,
            BLACK,
            WHITE,
            RED
        };

        void showColor(Color color); // For debugging: display color as text

        /**
         * @brief Struct to hold a Unicode symbol and its human-readable name
         * @details Used for text-based display of game stones (supports full Unicode, including Chinese characters)
         */
        struct Symbol
        {
            char32_t symbol; // 32-bit Unicode character (covers all Unicode planes, including Chinese)
            // std::string name; // Human-readable name (e.g., "CHINA_JU" for 車)
            // using std::string will not allow symbols (the array ) to be constexpr,  because std::string will require dynamic memory allocation, which is not allowed in a constexpr context. To make the symbols array constexpr, we can use a C-style string (const char*) for the name instead, which can be initialized at compile time without dynamic memory allocation. This way, we can still have human-readable names for the symbols while keeping the array constexpr.
            // Using wchar_t may also work, and it is easy to print using wcout. But, wcout is not as reliable for cross-platform console output, especially for Chinese characters, and it may require additional setup (like setting the locale) to work correctly. On the other hand, using const char* with UTF-8 encoding is more universally supported across different platforms and terminals without needing special configuration. Therefore, using const char* for the name is a more practical choice for our use case where we want to display symbols in a text-based console application.
            // Using char16_t for the name would not be ideal because they are not guaranteed to be able to represent all Unicode characters, and they also do not support string literals in a way that would allow us to easily initialize them at compile time. Using const char* allows us to have simple string literals for the names without worrying about encoding issues or dynamic memory allocation, making it suitable for our use case where we want a fixed set of symbols with human-readable names.

            const char *name; // C-style string for human-readable name (to avoid dynamic memory issues in struct)
        };

        /**
         * @brief Enumeration for stone shape (visual styling for text/UI display)
         * CIRCLE = default for most board games, SQUARE/TRIANGLE = alternative visual styles
         */
        enum class Shape
        {
            CIRCLE,  // Standard shape (e.g., Go/Chess stones)
            SQUARE,  // Alternative shape, a rare but possible style.
            TRIANGLE // Niche shape for special stones, a rare but possible style.
        };

        void showShape(Shape shape); // For debugging: display shape as text

        /**
         * @brief Unicode Symbol Reference Table (for text-based game board display)
         *
         * | Unicode Code Point | Name               | Literal | Description                  |
         * |--------------------|--------------------|---------|------------------------------|
         * | U+229B             | BLACK              | ⊛       | Go stone (larger marker)     |
         * | U+25EF             | WHITE              | ◯       | Go stone (hollow circle)     |
         * | U+2654             | WHITE_KING         | ♔       | Chess white king             |
         * | U+2655             | WHITE_QUEEN        | ♕       | Chess white queen            |
         * | U+2656             | WHITE_ROOK         | ♖       | Chess white rook             |
         * | U+2657             | WHITE_BISHOP       | ♗       | Chess white bishop           |
         * | U+2658             | WHITE_KNIGHT       | ♘       | Chess white knight           |
         * | U+2659             | WHITE_PAWN         | ♙       | Chess white pawn             |
         * | U+265A             | BLACK_KING         | ♚       | Chess black king             |
         * | U+265B             | BLACK_QUEEN        | ♛       | Chess black queen            |
         * | U+265C             | BLACK_ROOK         | ♜       | Chess black rook             |
         * | U+265D             | BLACK_BISHOP       | ♝       | Chess black bishop           |
         * | U+265E             | BLACK_KNIGHT       | ♞       | Chess black knight           |
         * | U+265F             | BLACK_PAWN         | ♟       | Chess black pawn             |
         * | U+8ECA             | CHINA_BLACK_JU     | 車      | Chinese Chess black chariot  |
         * | U+99AC             | CHINA_BLACK_MA     | 馬      | Chinese Chess black horse    |
         * | U+8C61             | CHINA_BLACK_XIANG  | 象      | Chinese Chess black elephant |
         * | U+58EB             | CHINA_BLACK_SHI    | 士      | Chinese Chess black advisor  |
         * | U+5C07             | CHINA_BLACK_JIANG  | 將      | Chinese Chess black general  |
         * | U+7832             | CHINA_BLACK_PAO    | 砲      | Chinese Chess black cannon   |
         * | U+5352             | CHINA_BLACK_ZU     | 卒      | Chinese Chess black pawn     |
         * | U+4FE5             | CHINA_RED_JU       | 俥      | Chinese Chess red chariot    |
         * | U+508C             | CHINA_RED_MA       | 傌      | Chinese Chess red horse      |
         * | U+76F8             | CHINA_RED_XIANG    | 相      | Chinese Chess red elephant   |
         * | U+4ED5             | CHINA_RED_SHI      | 仕      | Chinese Chess red advisor    |
         * | U+5E25             | CHINA_RED_JIANG    | 帥      | Chinese Chess red general    |
         * | U+70AE             | CHINA_RED_PAO      | 炮      | Chinese Chess red cannon     |
         * | U+5175             | CHINA_RED_ZU       | 兵      | Chinese Chess red pawn       |
         * | U+FFFD             | UNKNOWN            | �       | Replacement character        |
         * | U+003F             | QUESTION_MARK      | ?       | Plain question mark          |
         * | U+0020             | EMPTY              | (space) | Empty board position         |
         */

        // constexpr will enforce the compiler to do some checking at compile time, also, it makes launching the program faster because the symbols are already initialized at compile time, and it can also help optimize the code by allowing the compiler to make certain assumptions about the values of the symbols. Since these symbols are constant and known at compile time, using constexpr is appropriate here. It will ensure that the symbols are initialized at compile time and cannot be modified at runtime, which is ideal for this use case where we want a fixed set of symbols for our game stones.
        //  which can help catch errors early and optimize the code. Since the symbols are constant and known at compile time, using constexpr is appropriate here.
        constexpr Symbol symbols[] = {
            // Go stones
            // {U'\u25CF', "BLACK"}, // ● (U+25CF), old smaller black Go stone
            // {U'\u25CB', "WHITE"}, // ○ (U+25CB), old smaller white Go stone
            {U'\u229B', "BLACK"}, // ⊛ (larger distinct marker, avoids filled dark stone)
            {U'\u25EF', "WHITE"}, // ◯ (hollow marker, clear on both dark/light terminals)

            // Chinese Chess pieces (Unicode literals + escape codes for clarity)
            // also ok : {U'車', "CHINA_JU"}, {U'馬', "CHINA_MA"}, {U'象', "CHINA_XIANG"}, {U'士', "CHINA_SHI"}, {U'將', "CHINA_JIANG"}, {U'炮', "CHINA_PAO"}, {U'卒', "CHINA_ZU"},

            //  sometimes the symplified chinese character 车 may also share the same code point U+8F66, then, 车 could be printed.
            // but we will use the traditional one 車 for better visual distinction from the empty space.
            /*
            {U'\u8F66', "CHINA_JU"},    // 車 (U+8F66)
            {U'\u9A6C', "CHINA_MA"},    // 馬 (U+9A6C)
            {U'\u8C61', "CHINA_XIANG"}, // 象 (U+8C61)
            {U'\u58EB', "CHINA_SHI"},   // 士 (U+58EB)
            {U'\u5C06', "CHINA_JIANG"}, // 將 (U+5C06)
            {U'\u70AE', "CHINA_PAO"},   // 炮 (U+70AE)
            {U'\u5352', "CHINA_ZU"},    // 卒 (U+5352)
            */

            {U'\u8ECA', "CHINA_BLACK_JU"},    // 車
            {U'\u99AC', "CHINA_BLACK_MA"},    // 馬
            {U'\u8C61', "CHINA_BLACK_XIANG"}, // 象
            {U'\u58EB', "CHINA_BLACK_SHI"},   // 士
            {U'\u5C07', "CHINA_BLACK_JIANG"}, // 將
            {U'\u7832', "CHINA_BLACK_PAO"},   // 砲
            {U'\u5352', "CHINA_BLACK_ZU"},    // 卒

            {U'\u4FE5', "CHINA_RED_JU"},    // 俥
            {U'\u508C', "CHINA_RED_MA"},    // 傌
            {U'\u76F8', "CHINA_RED_XIANG"}, // 相
            {U'\u4ED5', "CHINA_RED_SHI"},   // 仕
            {U'\u5E25', "CHINA_RED_JIANG"}, // 帥
            {U'\u70AE', "CHINA_RED_PAO"},   // 炮
            {U'\u5175', "CHINA_RED_ZU"},    // 兵

            // International Chess pieces
            {U'\u2654', "WHITE_KING"},   // ♔ (U+2654)
            {U'\u2655', "WHITE_QUEEN"},  // ♕ (U+2655)
            {U'\u2656', "WHITE_ROOK"},   // ♖ (U+2656)
            {U'\u2657', "WHITE_BISHOP"}, // ♗ (U+2657)
            {U'\u2658', "WHITE_KNIGHT"}, // ♘ (U+2658)
            {U'\u2659', "WHITE_PAWN"},   // ♙ (U+2659)
            {U'\u265A', "BLACK_KING"},   // ♚ (U+265A)
            {U'\u265B', "BLACK_QUEEN"},  // ♛ (U+265B)
            {U'\u265C', "BLACK_ROOK"},   // ♜ (U+265C)
            {U'\u265D', "BLACK_BISHOP"}, // ♝ (U+265D)
            {U'\u265E', "BLACK_KNIGHT"}, // ♞ (U+265E)
            {U'\u265F', "BLACK_PAWN"},   // ♟ (U+265F)

            // Fallback/empty symbols
            {U'\uFFFD', "UNKNOWN"},       // � (U+FFFD)
            {U'\u003F', "QUESTION_MARK"}, // ? (U+003F)
            {U'\u0020', "EMPTY"}          // (space) (U+0020). The empty symbol can be handled specially. Maybe the space will not be printed in some case.

        };

        /**
         * @brief Struct representing a single game stone
         * @details Combines visual properties (symbol, color, shape) for text-based UI display
         */
        struct Stone
        {
            Symbol symbol; // Unicode symbol + name for display
            // The following three fields are useful for some advanced graphical printing or UI system. For console-based display, they may not have much effect, but they can still be used for debugging or future extensions.
            Color symbolColor;     // Foreground color of the stone symbol
            Color backgroundColor; // Background color of the stone (for UI contrast)
            Shape shape;           // Visual shape of the stone (cosmetic). For example, Chinese chess pieces are usually displayed as squares, while Go stones are circles. This field can be used to differentiate them visually in a text-based display (e.g., using different Unicode symbols or styling).
        };

        // a way to get the empty stone
        inline Stone get_empty_stone()
        {
            int emptyIndex = sizeof(symbols) / sizeof(Symbol) - 1; // index of the last symbol (empty)

            // C++ (since c++11) supports aggregate initialization for structs and classes that do not have user-defined constructors
            return Stone{symbols[emptyIndex], Color::EMPTY, Color::EMPTY, Shape::CIRCLE}; // EMPTY stone
        }

        /**
         * @brief Class to manage a collection of Stone objects. I.e., all the different types of stones used in a game (e.g., black/white Go stones, chess pieces, etc.)
         * @details Provides storage and operations for a set of game stones (e.g., all stones in a game)
         * @note The stone in the collection should be distinct from each other.
         */
        class Stones
        {
        private:
            Stone *stoneArr; // Dynamically allocated array of stones
            int stoneNum;    // Number of stones in the array
        public:
            /**
             * @brief Constructor: Initialize Stones with a given number of empty stones; It can also work as a default constructor.
             * @param num Number of stones to allocate
             * @param arr Optional array of Stone objects to initialize from (if nullptr, initializes to empty stones)
             */
            // Stones(int num = 2, Stone *arr = nullptr);
            // let the default value of num be 0, consistent to the default value of arr which is nullptr.
            Stones(int num = 0, Stone *arr = nullptr);

            /**
             * @brief Construct a new Stones object by copying another Stones object
             * @param other The Stones object to copy from
             * @note Stones owns dynamic memory, so this class follows Rule of Three
             *       (copy constructor, copy assignment, destructor).
             */
            Stones(const Stones &other);

            /**
             * @brief Copy assignment operator (deep copy).
             * @param other The Stones object to copy from
             * @return Stones& Reference to this object
             * @note Added to fix a double-free bug: without this deep copy,
             *       default assignment copied only the raw pointer.
             */
            Stones &operator=(const Stones &other);

            /**
             * @brief Destructor: Clean up dynamically allocated stone array
             */
            ~Stones();

            /**
             * @brief Get the number of stones in the collection
             * @return int Number of stones
             */
            int getStoneNum() const { return stoneNum; }

            /**
             * @brief Get a reference to a stone at a given index (for modification/access)
             * @param index Index of the stone (0-based)
             * @return Stone& Reference to the stone (throws if index is invalid)
             */
            Stone &getStone(int index)
            {
                if (index < 0 || index >= stoneNum)
                {
                    throw std::out_of_range("Stone index out of bounds");
                }
                return stoneArr[index];
            }

            /**
             * @brief Set a stone's properties at a given index
             * @param index Index of the stone (0-based)
             * @param newSymbol Symbol for the stone
             * @param symColor Foreground color of the symbol
             * @param bgColor Background color of the stone
             * @param shp Shape of the stone
             * @note Throws out_of_range exception if index is invalid, with a proper error message like "Stone index out of bounds".
             */
            void setStone(int index, const Symbol &newSymbol, Color symColor, Color bgColor, Shape shp);

            // overloaded version of setStone that takes a Stone object directly
            void setStone(int index, const Stone &newStone);

            /**
             * @brief Display all stones in the collection (for debugging purposes)
             *
             */
            void showStones() const; // For debugging: display all stones in the collection
        };
    }
}

#endif // _STONE_HPP_