/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 2
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
 * 王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>
#include <string>
#include <map>

namespace CS111
{

    // MenuItem as specified
    /*
    struct MenuItem
    {
        int key;
        std::string value;
    };
    */
   typedef std::pair<int, std::string> MenuItem; // Alternative definition using std::pair, contains the int key and the string value.

    // Menu class as specified
    class Menu
    {
    private:
        // Exactly the required data member. The key and value of the menu are stored in in "choices". 
        // The key is an integer, and the value is a string.
        // Students need to learn how to use std::map to implement the menu.
        std::map<int, std::string>* choices;

    public:
        // Static helper function to create a MenuItem, using the parameters key and value. 
        // It returns a MenuItem with the given key and value.
        static MenuItem make_menu_item(int key, const std::string& value) ;
     

    // !!!!!!!!!!!!!! < Task 4 > !!!!!!!!!!!!!!
    //  provide the missing declaration of the members according to the comments: 
    //  There are totally 12 missing declarations. Write them at the correct places after their numbered comments.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        

        // The following 5 members are called the "Rule of Five" members. 
        // 4.1) Copy constructor
       Menu(const Menu &other);

        // 4.2) Move constructor
        Menu(Menu &&other) noexcept;

        // 4.3) Copy assignment operator  
       Menu &operator=(const Menu &other);

        // 4.4) Move assignment operator
        Menu &operator=(Menu &&other) noexcept;

        // 4.5) Destructor
       ~Menu();

        // 4.6) Default constructor
        Menu();
        
        // ---------------------------
        // Core methods
        // ---------------------------
        int getSize() const;

        // 4.7) operator [] for read and write
        std::string &operator[](int key);

        // 4.8) operator [] for read-only access (const)
        const std::string &operator[](int key) const;

        // 4.9) method to over load the  += operator.  
        /* Its argument is a MenuItem. If the key does not exists in the map, the key and string are inserted into the map. Otherwise if  the key already exists, it should print an error message and do nothing.*/
        Menu &operator+=(const MenuItem &item);

        // 4.10) method to overload the -= operator; remove by key. 
        // It takes an int key as argument. If the key exists in the map, it should be removed. Otherwise if the key does not exist, do nothing.
        Menu &operator-=(int key);

        // 4.11) 
        // declare a method:  pick_choice
        // parameter is void. 
        // Ask user to pick a valid integer key from the menu
        // Repeats until a valid choice is entered
        // it returns an integer, which is the key of the picked choice.
        int pick_choice();

        
        // 4.12) Friend display operator << to print the menu nicely
        friend std::ostream &operator<<(std::ostream &os, const Menu &menu);
    };

} // namespace CS111

#endif