/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 2
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

// Test all functions of myVector.hpp
#include <iostream>
#include "../Include/myVector.hpp"
#include "../Include/tool.hpp"

using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

template <typename U>
void test_my_vector(U* arr, int size) // accept an array and its size (number of elements) for the testing. 
{
    using namespace CS111;
    print_test_separator("Default constructor");
    CS111::MyVector<U> vec;
    print_test_separator("set_debug_mode");
    vec.set_debug_mode(true);
    print_test_separator("push_back");
    for (int i = 0; i < size; ++i)
    {
        vec.push_back(arr[i]);
    }
    print_test_separator("operator <<");
    std::cout << "Vector contents: " << vec << std::endl;
    std::cout << ".....Vector class info, testing get_class_info() :\n" << vec.get_class_info() << std::endl;
    std::cout << ".....Vector size, testing get_size():\n " << vec.get_size() << std::endl;
    std::cout << ".....Vector capacity, testing get_capacity(): \n" << vec.get_capacity() << std::endl;

    // test copy constructor
    print_test_separator("copy constructor");
    CS111::MyVector<U> vec_copy(vec);
    std::cout << "Copy Vector contents: " << vec_copy << std::endl;

    // test move constructor
    print_test_separator("move constructor");
    CS111::MyVector<U> vec_move(std::move(vec));
    std::cout << "Move Vector contents: " << vec_move << std::endl;
    std::cout << "Original Vector contents after move: " << vec << std::endl;

    // test copy assignment operator
    print_test_separator("copy assignment operator");
    CS111::MyVector<U> vec_assign;
    vec_assign = vec_copy;
    std::cout << "Assigned Vector contents: " << vec_assign << std::endl;

    // test move assignment operator
    print_test_separator("move assignment operator");
    CS111::MyVector<U> vec_move_assign;
    vec_move_assign = std::move(vec_copy);
    std::cout << "Move Assigned Vector contents: " << vec_move_assign << std::endl;
    std::cout << "Original Copy Vector contents after move assignment: " << vec_copy << std::endl;  

    //  test pop_back
    print_test_separator("pop_back");
    vec_move_assign.pop_back();
    std::cout << ".....Move Assigned Vector contents after pop_back: " << vec_move_assign << std::endl;

    // test subscript operator
    print_test_separator("subscript operator");
    std::cout << "Element at index 5: " << vec_move_assign[5] << std::endl;
    vec_move_assign[5] = 99; 
    std::cout << "Element at index 5 after modification: " << vec_move_assign[5] << std::endl;


    // test const subscript operator
    print_test_separator("const subscript operator");
    const CS111::MyVector<U> const_vec(vec_move_assign);
    std::cout << "Element at index 5 in const vector: " << const_vec[5] << std::endl;
    
    // test destructor by creating a scope
    print_test_separator("destructor");
    {
        CS111::MyVector<U> vec_destruct;
        vec_destruct.push_back(arr[0]);
        vec_destruct.push_back(arr[1]);
        std::cout << "Vector contents before going out of scope: " << vec_destruct << std::endl;
    }   // vec_destruct goes out of scope here, destructor should be called

    // test resize function by pushing back more elements to trigger resize
    print_test_separator("resize function");
    for (int i = 15; i < 30; ++i)
    {
        vec_move_assign.push_back(arr[i%size]); // push back more elements to trigger resize
    }
    std::cout << "Move Assigned Vector contents after pushing more elements: " << vec_move_assign << std::endl;

    // test set_debug_mode function by toggling debug mode on and off
    print_test_separator("set_debug_mode toggle");
    vec_move_assign.set_debug_mode(false);

    // test get_debug_mode function
    print_test_separator("get_debug_mode");
    std::cout << "Debug mode is currently: " << (vec_move_assign.get_debug_mode() ? "enabled" : "disabled") << std::endl;
}

int main()
{    
    using namespace CS111;
    using namespace std;
    print_test_banner("MyVector Class Template Tests");
    print_test_separator("Run with int type");
    int arr[30];
    for (int i = 0; i < 30; ++i)    {
        arr[i] = i + 1; // fill the array with values from 1 to 30
    }
    test_my_vector<int>(arr, 30);
    print_test_separator("Run with std::string type");
    string str_arr[30];
    for (int i = 0; i < 30; ++i)    {
        str_arr[i] = "String " + to_string(i + 1); // fill the array with string values "String 1", "String 2", ..., "String 30"
    }
    test_my_vector<std::string>(str_arr, 30 );
    return 0;
}


