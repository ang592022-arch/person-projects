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

#include "../Include/utils.hpp"
#include <stdexcept>
#include <limits>
#include <cctype> // for isspace()

namespace CS111 {



    // Extracts a single token from the stream (no check).
    // Example: if the stream contains "GAME_NAME: \"GoBang\"", extract_token will return "GAME_NAME:" on the first call, then '"GoBang"' on the next.
    // Calling this function will skip any leading whitespace, then read characters until the next whitespace (space, tab, newline) or EOF is reached, and return the extracted token as a string. A token contains no whitespace. If the stream is at EOF before reading any character, it will return an empty string. 
    std::string extract_token(std::istream& is) {
        std::string token;
        is >> token;
        return token;
    }

    


    // Extracts a whole line from the stream (no check).
    // Example: if the stream is at the start of a line, extract_line will return the entire line up to (but not including) the newline, and consume the newline.
    std::string extract_line(std::istream& is) {
        std::string line;
        std::getline(is >> std::ws, line); 
        return line;
    }

    // Attempts to extract and match the next token from the stream.
    // Skips leading whitespace, extracts the next token, and checks if it matches 'wanted'.
    // If matched, consumes the token and returns true. If not matched, stream is unchanged and returns false.
    // Example: if the stream contains "GAME_NAME: \"GoBang\"", extract_required_token(is, "GAME_NAME:") will consume and return true; otherwise, returns false and stream is unchanged.
    bool extract_required_token(std::istream& is, const std::string& wanted) {
        std::istream::pos_type pos = is.tellg();
        std::string token;
        is >> token;
        if (!is) {
            is.clear();
            is.seekg(pos);
            return false;
        }
        if (token == wanted) {
            return true;
        } else {
            is.clear();
            is.seekg(pos);
            return false;
        }
    }

    // Attempts to extract and match the exact text from the current stream position (no whitespace skip, no newline skip).
    // Returns true if the exact text is found and consumed, false otherwise (stream unchanged if not matched).
    // Example: if the stream is at position "===End of Game Info===", extract_required_text(is, "===End of Game Info===") will consume and return true; if not matched, stream is unchanged.
    bool extract_required_text(std::istream& is, const std::string& text) {
        std::istream::pos_type pos = is.tellg();
        for (char ch : text) {
            int c = is.get();
            if (c == EOF || static_cast<char>(c) != ch) {
                if (c != EOF) is.unget();
                is.clear();
                is.seekg(pos);
                return false;
            }
        }
        return true;
    }

    bool reach_non_whitespace(std::istream& is){
        while (true) {
            int c = is.peek();
            if (c == EOF) {
                is.clear();
                return false;
            }
            if (!std::isspace(c)) {
                return true;
            }
            is.get(); // consume the whitespace character
        }
    }

    // Pauses execution until user presses Enter.
    // Uses std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n') to ignore all characters up to and including the next newline.
    // This ensures that any leftover input is cleared before waiting for Enter.
    // Here the max() function means to ignore as many characters as possible until the '\n' is found, which is safer than using a fixed number like 80, because if there are more than 80 characters before the '\n', then the remaining characters will not be ignored, and will be read in the next input operation, which may cause problems. So it is better to use numeric_limits<streamsize>::max() to ignore as many characters as possible until the '\n' is found.
    void pause_for_enter(const std::string& prompt) {
        std::cout << prompt;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

   // If a lambda function, like the following declaration, is used multiple times, better to define a normal function instead. 
   // auto trim = [](const std::string &s) -> std::string

   // For a string s, return a new string with leading and trailing whitespace removed.
   std::string trim(const std::string &s)
        {
        // !!!!!!!!!!!!!! < Hmk3 Task 1 > !!!!!!!!!!!!!!
        // Implement the function according to the specification.
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
        --end;

    return s.substr(start, end - start);
        }


} // namespace CS111
