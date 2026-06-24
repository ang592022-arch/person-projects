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

#ifndef MYVECTOR_HPP
#define MYVECTOR_HPP

#include <iostream>
#include <string>

namespace CS111
{

    template <typename T>
    class MyVector
    {
    public:
        // default constructor, can also accept an array and its size to initialize the vector, and a debug mode flag to enable debug mode if needed.
        MyVector(bool debug = false, T* arr = nullptr, int size = 0);
 
        // copy constructor
        MyVector(const MyVector &other);

        // move constructo
        MyVector(MyVector &&other) noexcept;

        // copy assignment operator
        MyVector &operator=(const MyVector &other);

        // move assignment operator
        MyVector &operator=(MyVector &&other) noexcept;

        // destructor
        ~MyVector();

        void push_back(const T &element);
        void pop_back();

        int get_size() const { return size; }
        int get_capacity() const { return capacity; }
        bool get_debug_mode() const { return debugMode; }
        std::string get_class_info() const { return classInfo; }

        void set_debug_mode(bool mode); //  { debugMode = mode; }

        // overload the subscript operator to access elements in the vector
        T &operator[](int index);
        const T &operator[](int index) const;

        // overloading the << operator for outputting the contents of the vector
        template <typename U>
        friend std::ostream &operator<<(std::ostream &os, const MyVector<U> &vec);

    private:

        // class information: a string explain the class
        static std::string classInfo;

        bool debugMode = false; // default is false, set to true to enable debug mode

        void resize(); // function to resize the vector when capacity is reached

        int size;
        int capacity;
        T *memory;
    };
} // end of

#include "myVector.tpp"

#endif // MYVECTOR_HPP