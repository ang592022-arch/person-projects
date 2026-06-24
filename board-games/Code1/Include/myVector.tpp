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

#ifndef _MY_VECTOR_TPP_
#define _MY_VECTOR_TPP_

// the function definitions of MyVector are here.
#include "myVector.hpp"

namespace CS111
{
    // default capacity size is 10:
    enum
    {
        DEFAULT_CAPACITY = 10
    };

    template <typename T>
    MyVector<T>::MyVector(bool debug, T *arr, int size) : size(size), capacity(DEFAULT_CAPACITY), memory(new T[capacity]), debugMode(debug)
    {
        if (debugMode)
        {
            std::cout << "Constructor called" << std::endl;
        }
        if (arr != nullptr && size > 0)
        {
            while (capacity < size)
            {
                capacity *= 2; // double the capacity until it can accommodate the initial size
            }
            delete[] memory;          // delete the initially allocated memory
            memory = new T[capacity]; // allocate new memory with the updated capacity
            for (int i = 0; i < size; ++i)
            {
                memory[i] = arr[i];
            }
        }
    }

    template <typename T>
    MyVector<T>::MyVector(const MyVector &other) : size(other.size), capacity(other.capacity), debugMode(other.debugMode), memory(new T[capacity])
    {
        if (debugMode)
        {
            std::cout << "Copy constructor called" << std::endl;
        }
        for (int i = 0; i < size; ++i)
        {
            memory[i] = other.memory[i];
        }
    }

    template <typename T>
    MyVector<T>::MyVector(MyVector &&other) noexcept : size(other.size), capacity(other.capacity), debugMode(other.debugMode), memory(other.memory)
    {
        if (debugMode)
        {
            std::cout << "Move constructor called" << std::endl;
        }
        other.size = 0;
        other.capacity = 0;
        other.memory = nullptr;
    }

    // !!!!!!!!!!!!!! < Task 11 > !!!!!!!!!!!!!!
    //  Implement the  copy assignment operator of the MyVector class according to the specification.
    template <typename T>
    MyVector<T> &MyVector<T>::operator=(const MyVector &other)
    {
        if (debugMode)
        {
            std::cout << "Copy assignment operator called" << std::endl;
        }
        if (this != &other)
        {
            T *new_memory = new T[other.capacity];
            for (int i = 0; i < other.size; ++i)
            {
                new_memory[i] = other.memory[i];
            }
            delete[] memory;
            memory = new_memory;
            size = other.size;
            capacity = other.capacity;
            debugMode = other.debugMode;
        }
        return *this;
    }// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    

    // !!!!!!!!!!!!!! < Task 12 > !!!!!!!!!!!!!!
    //  Implement the  move assignment operator of the MyVector class according to the specification.
    template <typename T>
    MyVector<T> &MyVector<T>::operator=(MyVector &&other) noexcept
    {
        if (debugMode)
        {
            std::cout << "Move assignment operator called" << std::endl;
        }
        if (this != &other)
        {
            delete[] memory;
            memory = other.memory;
            size = other.size;
            capacity = other.capacity;
            debugMode = other.debugMode;

            other.memory = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }



    template <typename T>
    MyVector<T>::~MyVector()
    {

        delete[] memory;
    }

    template <typename T>
    void MyVector<T>::push_back(const T &element)
    {
        if (size >= capacity)
        {
            resize();
        }
        memory[size++] = element;
    }

    template <typename T>
    void MyVector<T>::pop_back()
    {
        if (size > 0)
        {
            --size;
        }
    }

    template <typename T>
    T &MyVector<T>::operator[](int index)
    {
        return memory[index];
    }

    template <typename T>
    const T &MyVector<T>::operator[](int index) const
    {
        return memory[index];
    }

    // forward declaration of the friend function for overloading the << operator, which is defined after the class definition.
    // without the following delaration, the compiler will say there is no member named operator<< in the name space CS111, even though in the definition CS111::operator<< is mentioned. This is because the compiler needs to know the declaration of the friend function before it can be used in the class definition. Of course, put the definition of operator<< here will also ok.
    template <typename U>
    std::ostream &operator<<(std::ostream &os, const MyVector<U> &vec);

} // end of namespace CS111

// define the resize function here.
// !!!!!!!!!!!!!! < Task 13 > !!!!!!!!!!!!!!
//  Implement the  resize function of the MyVector class according to the specification.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// double the capacity of the vector when the current size reaches the capacity, and copy the existing elements to the new memory location.
template <typename T>
void CS111::MyVector<T>::resize()
{
   capacity = (capacity == 0) ? DEFAULT_CAPACITY : capacity * 2;
    T *new_memory = new T[capacity];
    for (int i = 0; i < size; ++i)
    {
        new_memory[i] = memory[i];
    }
    delete[] memory;
    memory = new_memory;
}

// define the set_debug_mode function here.
template <typename T>
void CS111::MyVector<T>::set_debug_mode(bool mode)
{
    if (debugMode == true)
    {
        std::cout << "set_debug_mode called" << std::endl;
    }
    debugMode = mode;
    if (debugMode == true)
    {

        if (debugMode)
        {
            std::cout << "Debug mode enabled" << std::endl;
        }
        else
        {
            std::cout << "Debug mode disabled" << std::endl;
        }
    }
}

// define the friend function for overloading the << operator here.
// assume the type U can support << operator for outputting.
template <typename U>
// bug forgot to mention the namespace CS111 here, which will cause compilation error because the compiler cannot find the definition of MyVector<U> without the namespace.`
// std::ostream & operator<<(std::ostream &os, const CS111::MyVector<U> &vec)

std::ostream &CS111::operator<<(std::ostream &os, const CS111::MyVector<U> &vec)
{
    os << "[";
    for (int i = 0; i < vec.size; ++i)
    {
        os << vec.memory[i];
        if (i < vec.size - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// initialize the static member variable classInfo
template <typename T>
std::string CS111::MyVector<T>::classInfo = std::string("MyVector class template, with type T");

/*
Where to put the static member variable definition of a template class?
For a regular class, we can put the static member variable definition in the .cpp file. And we should avoid put such a definition in a header file because if we include that header file in multiple .cpp files, we will get multiple definitions of the static member variable, which will cause linker errors.

However, for a template class, we cannot do that because the compiler needs to know the definition of the static member variable when it instantiates the template class. Therefore, we need to put the static member variable definition in the header file, or in this case, in the .tpp file which is included in the header file. This way, when the compiler instantiates the template class, it will also have access to the definition of the static member variable. Will it cause multiple definitions of the static member variable if we include the .tpp file in multiple .cpp files? No, it will not cause multiple definitions because the static member variable is only defined once for each instantiation of the template class.  When the linker links the object files together, it will only link one definition of the static member variable for each instantiation of the template class. So, the linker will resolve the issue of multiple definition of the same static member.
*/

#endif