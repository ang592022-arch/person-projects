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

#include "../Include/tool.hpp"
#include <iostream>
#include <string>

// _WIN32 is defined for both g++ and cl when compiling on Windows,  both for 32-bit and 64-bit targets. So it is a reliable macro to use for Windows-specific code.

#ifdef _WIN32
#include <windows.h> // for Windows console handling in print_char32
#endif

using namespace std;

namespace CS111::Tools
{
    // C++ 17 allows nested namespace
    // annoymous namespace, include global names with internal linkage. These names can only be used in tools.cpp. Like static global names.
    namespace
    {
        // A function useful for Mac terminal, where printing uses utf8.
        // Translate a Unicode code point (char32_t) to a UTF-8 encoded string (std::string) that can be written to the terminal using cout.
        std::string encode_utf8(char32_t c)
        {
            std::string utf8;

            if (c <= 0x7F)
            {
                utf8.push_back(static_cast<char>(c));
            }
            else if (c <= 0x7FF)
            {
                utf8.push_back(static_cast<char>(0xC0 | (c >> 6)));
                utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c <= 0xFFFF)
            {
                utf8.push_back(static_cast<char>(0xE0 | (c >> 12)));
                utf8.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c <= 0x10FFFF)
            {
                utf8.push_back(static_cast<char>(0xF0 | (c >> 18)));
                utf8.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else
            {
                utf8 = "\xEF\xBF\xBD"; // it means an invalid code point, the replacement character U+FFFD in UTF-8 encoding
            }

            return utf8;
        }

#ifdef _WIN32
        // A function useful for Windows console, where printing uses utf16. It converts a char32_t code point to a UTF-16 encoded string (std::wstring) that can be written to the console using WriteConsoleW.
        std::wstring encode_utf16_for_console(char32_t c)
        {
            std::wstring utf16;

            if (c <= 0xFFFF)
            {
                utf16.push_back(static_cast<wchar_t>(c));
            }
            else if (c <= 0x10FFFF)
            {
                char32_t v = c - 0x10000;
                utf16.push_back(static_cast<wchar_t>(0xD800 + (v >> 10)));
                utf16.push_back(static_cast<wchar_t>(0xDC00 + (v & 0x3FF)));
            }
            else
            {
                utf16.push_back(static_cast<wchar_t>(0xFFFD));
            }

            return utf16;
        }
#endif
    }

    void write_symbol(std::ostream &os, char32_t c)
    {
#ifdef _WIN32
        // Windows console APIs are UTF-16 based, so write through WriteConsoleW when possible.
        if (&os == &std::cout)
        {
            /* HANDLE is declared in windows.h. A handle means a reference to an operating system resource.  Actually it is a pointer.
             GetStdHandle returns a handle to the specified standard device (standard input, standard output, or standard error). In this case, we want to get the handle for standard output (console), so we use GetStdHandle(STD_OUTPUT_HANDLE).
             If the function succeeds, the return value is a handle to the specified device (in this case, the console output). If the function fails, the return value is INVALID_HANDLE_VALUE (it is -1). So we need to check if hConsole is not INVALID_HANDLE_VALUE before using it.
            */
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD consoleMode = 0; // DWORD is unsigned long. 0 means we want to get the current console mode without changing it.

            /* GetConsoleMode returns nonzero if the console is workable (e.g., it is a real console, not redirected to a file), and it fills the consoleMode variable with the current mode. If the function fails, it returns zero. So we check if hConsole is valid and GetConsoleMode succeeds before using WriteConsoleW to write UTF-16 characters.
             The reason we check GetConsoleMode is that if the console is not in a normal state (e.g., it is redirected to a file, or it is an old Windows console that does not support Unicode), then we should fall back to writing UTF-8 bytes through os << encode_utf8(c) instead of using WriteConsoleW, which may fail or produce garbled output.
            */

            // the following if condition, explained in English means:
            // when the std output stream is available, and the console (the command line) is in workable state, ready for writing UTF-16 characters.
            if (hConsole != INVALID_HANDLE_VALUE && GetConsoleMode(hConsole, &consoleMode))
            {
                const std::wstring utf16 = encode_utf16_for_console(c);

                DWORD written = 0;
                // utf16.data() returns a pointer to the underlying array of wchar_t characters, and utf16.size() returns the number of wchar_t characters in the string. WriteConsoleW will write those UTF-16 characters directly to the console, which should display the correct Unicode character if the console supports it.
                WriteConsoleW(hConsole, utf16.data(), static_cast<DWORD>(utf16.size()), &written, nullptr);
                return;
            }
        }
#endif

        // on windows, if the console is not working in UTF-16 mode, then still use the UTF-8 encoding and write through os, which may work in some cases.

        os << encode_utf8(c);
    }

    void print_char32(char32_t c)
    {
        write_symbol(std::cout, c);
    }

    void clear_input_queue()
    {
        // !!!!!!!!!!!!!! < Task 2 > !!!!!!!!!!!!!!
        // Implement the function according to the specification.
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        cin.clear(); 
        int ch;
        while ((ch = cin.get()) != '\n' && ch != EOF)
        {
            
        }
        if (ch == EOF)
        {
            cin.clear(); 
        }
        
    }

    int read_int(const string &prompt)
    {
        // !!!!!!!!!!!!!! < Task 3 > !!!!!!!!!!!!!!
        // Implement the function according to the specification.
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        int value;
        while (true)
        {
            cout << prompt;
            if (cin >> value)
            {                
                return value;
            }
            else
            {
                clear_input_queue();
            }
        }
    }

    namespace Test
    {
        void print_test_banner(const std::string &title)
        {
            cout << "\n=============================================================\n";
            cout << title << "\n";
            cout << "=============================================================\n";
        }

        void print_test_separator(const std::string &label)
        {
            cout << "\n---------------------------------------------\n";
            if (!label.empty())
            {
                cout << label << "\n";
                cout << "---------------------------------------------\n";
            }
        }

        void check_true(TestContext &ctx, bool condition, const std::string &message)
        {
            if (condition)
            {
                cout << "[PASS] " << message << "\n";
            }
            else
            {
                cout << "[FAIL] " << message << "\n";
                ++ctx.failed_checks;
            }
        }

        void check_equal_int(TestContext &ctx, int actual, int expected, const std::string &message)
        {
            if (actual == expected)
            {
                cout << "[PASS] " << message << " (" << actual << ")\n";
            }
            else
            {
                cout << "[FAIL] " << message << " (actual=" << actual
                     << ", expected=" << expected << ")\n";
                ++ctx.failed_checks;
            }
        }

        void check_equal_str(TestContext &ctx, const std::string &actual,
                             const std::string &expected, const std::string &message)
        {
            if (actual == expected)
            {
                cout << "[PASS] " << message << " (\"" << actual << "\")\n";
            }
            else
            {
                cout << "[FAIL] " << message << " (actual=\"" << actual
                     << "\", expected=\"" << expected << "\")\n";
                ++ctx.failed_checks;
            }
        }
    }

} // namespace CS111::Tools