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

#include "../Include/stones.hpp"
#include "../Include/tools.hpp"
#include <iostream> // for std::cout in showStones() method
#include <stdexcept>
// implementation of Stones class methods
namespace CS111
{
    namespace BOARD_GAME
    {
        // print a string for the color of the stone, for debugging or future UI purposes. For console-based display, this may not have much effect, but it can still be used to differentiate stones visually in text output.
        void showColor(Color color)
        {
            switch (color)
            {
            case Color::EMPTY:
                std::cout << "EMPTY";
                break;
            case Color::BLACK:
                std::cout << "BLACK";
                break;
            case Color::WHITE:
                std::cout << "WHITE";
                break;
            case Color::RED:
                std::cout << "RED";
                break;
            default:
                std::cout << "UNKNOWN_COLOR";
                break;
            }
        }

        // print a string for the shape of the stone, for debugging or future UI purposes. For console-based display, this may not have much effect, but it can still be used to differentiate stones visually in text output.
        void showShape(Shape shape)
        {
            // !!!!!!!!!!!!!! < Task 7 > !!!!!!!!!!!!!!
            // Implement the function according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
             switch (shape)
            {
            case Shape::CIRCLE:   std::cout << "CIRCLE"; break;
            case Shape::SQUARE:   std::cout << "SQUARE"; break;
            case Shape::TRIANGLE: std::cout << "TRIANGLE"; break;
            default:              std::cout << "UNKNOWN_SHAPE"; break;
            }
            
        }

        Stones::Stones(int num, Stone *arr) : stoneNum(num)
        {

            if (num <= 0 || arr == nullptr) // default as a set with a single empty stone if input is invalid.
            {
                stoneNum = 1; // default to 0 stones if invalid input
                stoneArr = new Stone[stoneNum];
                stoneArr[0] = get_empty_stone(); // default with one empty stone
            }
            else // assum num >0 and arr != nullptr, then initialize with the provided array.
            {
                stoneArr = new Stone[num];
                // Initialize all stones to empty by default

                for (int i = 0; i < num; ++i)
                {
                    stoneArr[i] = arr[i]; // copy from input array if provided
                }
            }
        }

        // !!!!!!!!!!!!!! < Task 8 > !!!!!!!!!!!!!!
        // Implement copy constructor of the Stones class
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         Stones::Stones(const Stones &other) : stoneNum(other.stoneNum)
         {
            stoneArr = new Stone[stoneNum];
            for (int i = 0; i < stoneNum; ++i)
            {
                stoneArr[i] = other.stoneArr[i];
            }
        }


        Stones &Stones::operator=(const Stones &other)
        {
            if (this == &other)
            {
                return *this;
            }

            // Allocate/copy first, then swap ownership by assignment.
            // This fixes the old crash where shallow pointer copy caused
            // two objects to delete the same array at program exit.
            Stone *newArr = new Stone[other.stoneNum];
            for (int i = 0; i < other.stoneNum; ++i)
            {
                newArr[i] = other.stoneArr[i];
            }

            delete[] stoneArr;
            stoneArr = newArr;
            stoneNum = other.stoneNum;
            return *this;
        }

        // !!!!!!!!!!!!!! < Task 9 > !!!!!!!!!!!!!!
        // Implement destructor  of the Stones class
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        Stones::~Stones()
        {
            delete[] stoneArr;
        }

        // change the stone in stoneArr at the given index to have the new properties specified by the parameters. If the index is invalid, it should throw an out_of_range exception with a clear error message.
        // if index is invalid, some exception should be thrown, with some proper error message, for example "Stone index out of bounds". 
        void Stones::setStone(int index, const Symbol &newSymbol, Color symColor, Color bgColor, Shape shp)
        {
            // !!!!!!!!!!!!!! < Task 10 > !!!!!!!!!!!!!!
            //  Implement the setStone method (its body) of the Stones class according to the specification.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
           if (index < 0 || index >= stoneNum)
            {
                throw std::out_of_range("Stone index out of bounds");
            }
            stoneArr[index].symbol = newSymbol;
            stoneArr[index].symbolColor = symColor;
            stoneArr[index].backgroundColor = bgColor;
            stoneArr[index].shape = shp;
        }

        void Stones::setStone(int index, const Stone &newStone)
        {
            if (index < 0 || index >= stoneNum)
            {
                // debug
                std::cerr << "setStone (overloaded): Invalid index " << index << " (stoneNum=" << stoneNum << ")\n";
                throw std::out_of_range("Stone index out of bounds");
            }
            stoneArr[index] = newStone;
        }

        void Stones::showStones() const
        {
            for (int i = 0; i < stoneNum; ++i)
            {
                const Stone &s = stoneArr[i];
                std::cout << "Stone " << i << ": Symbol=" << s.symbol.name
                          << " (U+" << std::hex << s.symbol.symbol << std::dec << ") "
                    /* Print char32_t through the shared cross-platform helper. */;
                CS111::Tools::write_symbol(std::cout, s.symbol.symbol);
                std::cout << ", SymbolColor=";
                showColor(s.symbolColor);
                std::cout << ", BackgroundColor=";
                showColor(s.backgroundColor);
                std::cout << ", Shape=";
                showShape(s.shape);
                std::cout << std::endl;
            }
        }
    }
}