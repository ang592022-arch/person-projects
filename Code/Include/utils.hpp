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


#ifndef CS111_BOARD_GAME_UTILS_HPP
#define CS111_BOARD_GAME_UTILS_HPP

#include <iostream>
#include <string>

namespace CS111 {


    // Extracts a single token from the stream (no check).
    // Example: if the stream contains "GAME_NAME: \"GoBang\"", extract_token will return "GAME_NAME:" on the first call, then '"GoBang"' on the next.
    std::string extract_token(std::istream& is);

    // Extracts a whole line from the stream (no check).
    // Example: if the stream is at the start of a line, extract_line will return the entire line up to (but not including) the newline, and consume the newline.
    std::string extract_line(std::istream& is);

    // Attempts to extract and match the next token from the stream.
    // Skips leading whitespace, extracts the next token, and checks if it matches 'wanted'.
    // If matched, consumes the token and returns true. If not matched, stream is unchanged and returns false.
    // Example: if the stream contains "GAME_NAME: \"GoBang\"", extract_required_token(is, "GAME_NAME:") will consume and return true; otherwise, returns false and stream is unchanged.
    bool extract_required_token(std::istream& is, const std::string& wanted);

    // Attempts to extract and match the exact text from the current stream position (no whitespace skip, no newline skip).
    // Returns true if the exact text is found and consumed, false otherwise (stream unchanged if not matched).
    // Example: if the stream is at position "===End of Game Info===", extract_required_text(is, "===End of Game Info===") will consume and return true; if not matched, stream is unchanged.
    // Before calling the function, the position of the input stream should be located right on the first character. 
    // Be careful if the argument `text` contains some preceeding whitespace, that can make situation difficult. Better to avoid using text with preceeding whitespace for this function, or make sure the stream position is right before the first character of the text (which may be a whitespace character) before calling this function.
    bool extract_required_text(std::istream& is, const std::string& text);

    // skip whitespace until the first non-whitespace. If EOF is reached, clear the EOF state and return false. If the first non-whitespace character is found, put it back to the stream and return true. This can be used to check if the stream has reached EOF or not, and also to skip any leftover whitespace before waiting for user input.
    bool reach_non_whitespace(std::istream& is);

    // Pauses execution until user presses Enter.
    // Uses std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n') to ignore all characters up to and including the next newline.
    // This ensures that any leftover input is cleared before waiting for Enter.
    void pause_for_enter(const std::string& prompt = "Press Enter to continue...");

    // For a string s, return a new string with leading and trailing whitespace removed.
    std::string trim(const std::string &s);
}

#endif // CS111_BOARD_GAME_UTILS_HPP