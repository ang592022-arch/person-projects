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


#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <ostream>

namespace CS111
{
    // !!!!!!!!!!!!!! < Task 1 > !!!!!!!!!!!!!!
    // Add a namespace Tools, which is nested inside the CS111 namespace. All the functions belong to the Tools namespace.  
    // Hint: do not foget the closing brace. 
    namespace Tools{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    
     /**
     * @brief Clear the input queue by reading and discarding characters until a newline or end of input (EOF) is reached. 
     * @note It clears the fail state, then repeat reading a character from the input queue until '\n' or EOF is read. 
     * @note  Hint: can use cin.get() to read a single character, save it into an int variable, and check if it is '\n' or EOF. 
     * @note If EOF is reached, it will clear the EOF state to allow further input if needed. However, there is no guarantee that cin can resume to the normal state after EOF is reached, so if you see the message about EOF when calling this function, it is better to just stop the program and restart it if you want to continue testing. 
     */
        void clear_input_queue();

        /**
         * @brief Displays a prompt, and reads an integer from user input.
         * @param prompt The message to display to the user when asking for input.
         * @return int The valid integer entered by the user.
         * @note This function will repeat: prompting the user, and reading input until a valid integer is entered. 
         * @note When one attempt of reading an integer failes, It uses clear_input_queue() to flush any bad input, and cin.clear() to reset the error state. 
         * @note Even when reading an integer succeeds, it will also call clear_input_queue() to flush any extra input (e.g., if user types "123abc", it will read 123 as the integer, then flush "abc" from the input queue).   
         */
        int read_int(const std::string &prompt);


        /**
         * @brief Print one Unicode code point to a target output stream.
         * @param os Target stream.
         * @param c Unicode code point.
         * @note The code point is a number describing a symbol. The mapping from a code point to a symbol is defined by the Unicode standard. 
         * @note U'\u4F60' is a char32_t character literal. 
         * @note u'\u4F60' is a char16_t character literal, which is not the same as U'\u4F60'.
         * @note U'\u4F60' comparing with U'\U4F60': The former is a char32_t character literal, the latter is also a char32_t character literal, but it is not valid because \U must be followed by 8 hex digits, so it should be U'\U0000000000004F60' to be valid.
         * @note - For simple keyboard character, like 'A', who has an ASCII number that is small than 128, its unicode has a value that is the same as its ASCII value.  For example, the Unicode code point for 'A' is U+0041 (+ here is a separator, U means Unicode, 0041 means hex number 41), its decimal number is 65. Its byte sequence has just one byte, 0x41, same as its code point. 'a' has a Unicode code point U+0061, which is 97 in decimal, its byte sequence is 0x61.
         * @note - For more complex characters, like '你', its Unicode code point is U+4F60, which is 20320 in decimal. Its byte sequence in UTF-8 encoding has three bytes: 0xE4 0xBD 0xA0. Note that if 20320 is represented as unsigned integer in shortest form, its left most byte will be 0x4F, not 0xE4. So the byte sequence of a character in UTF-8 encoding is not the same as its Unicode code point for the characters who have code points larger than 127. 
         * @note - An example of character sequence with four bytes: the emoji "😀" has a Unicode code point U+1F600, which is 128512 in decimal. Its byte sequence in UTF-8 encoding has four bytes: 0xF0 0x9F 0x98 0x80.
         * @note - There is an algorithm to convert a Unicode code point to its UTF-8 byte sequence, which is what cout will output when we print a char32_t character. 
         * @note - Also, when the terminal (cout) reads a byte sequence, there is an algorithm to convert the byte sequence back to its code point. Especially, the bit pattern in the first byte will tell how many bytes are in the byte sequence. 
         * @note - When the terminal is in UTF-8 mode  , it will interpret the byte sequence as UTF-8 encoding, and display the corresponding character.   
         * @note - Usually a terminal is UTF-8 mode. For windows cmd, maybe need to change the code page to UTF-8 by running "chcp 65001" command.
         * @note - For more details about Unicode and UTF-8, see https://en.wikipedia.org/wiki/Unicode or https://en.wikipedia.org/wiki/UTF-8 or https://home.unicode.org/
         * @note - Example of calling this function:
         * @note      write_symbol(std:cout, U'\u4F60'); // '你'  
         * @note      write_symbol(std::cout, U'\U0001F600'); // '😀'     
         * @note      write_symbol(std::cout, U'\u03A9'); // 'Ω'
         * @note      write_symbol(std::cout, U'\u8ECA'); // '車'  
         * @note      write_symbol(std::cout, U'\u0041'); // 'A' 
         */
        void write_symbol(std::ostream &os, char32_t c);

        /**
         * @brief Given a Unicode code point (char32_t), print the corresponding character to the console through cout. It is a  portable helper, across different platforms and terminals, for printing Unicode characters.
         * @param c a Unicode code point represented as char32_t.
         */
        void print_char32(char32_t c);

        namespace Test
        {
            struct TestContext
            {
                int failed_checks = 0;
            };

            void print_test_banner(const std::string &title);
            void print_test_separator(const std::string &label = "");
            void check_true(TestContext &ctx, bool condition, const std::string &message);
            void check_equal_int(TestContext &ctx, int actual, int expected, const std::string &message);
            void check_equal_str(TestContext &ctx, const std::string &actual,
                                 const std::string &expected, const std::string &message);
        }

    }
}
#endif