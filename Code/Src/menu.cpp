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

#include "../Include/menu.hpp"
#include "../Include/tool.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace CS111
{
  
    MenuItem Menu::make_menu_item(int key, const std::string &value)
    {
    // !!!!!!!!!!!!!! < Task 5 > !!!!!!!!!!!!!!
    //  Implement the function according to the specification.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // the static helper function to create MenuItem
      return std::make_pair(key, value);
        
    }

    // !!!!!!!!!!!!!! < Task 6 > !!!!!!!!!!!!!!
    //  provide the missing declaration of the members according to the comments: 
    //  There are totally 12 missing declarations. Write them at the correct places after their numbered comments.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // The following 5 members are called the "Rule of Five" members.

    // --------------------------------------------------
    // 6.1 Copy Constructor (Rule of Five)
    // --------------------------------------------------
     Menu::Menu(const Menu& other)
    {
        if (other.choices != nullptr) {
            choices = new std::map<int, std::string>(*other.choices); // 深拷贝底层 map
        } else {
            choices = new std::map<int, std::string>();
        }
    }

    // --------------------------------------------------
    // 6.2 Move Constructor (Rule of Five)
    // --------------------------------------------------
    Menu::Menu(Menu&& other) noexcept
    {
        choices = other.choices; // 接管资源
        other.choices = nullptr; // 将源对象设为安全状态
    }

    // --------------------------------------------------
    // 6.3 Copy Assignment (Rule of Five)
    // --------------------------------------------------
    Menu& Menu::operator=(const Menu& other)
    {
        if (this != &other) {
            if (choices == nullptr) {
                choices = new std::map<int, std::string>();
            }
            if (other.choices != nullptr) {
                *choices = *other.choices;
            } else {
                choices->clear();
            }
        }
        return *this;
    }

    // --------------------------------------------------
    // 6.4 Move Assignment (Rule of Five)
    // --------------------------------------------------
    Menu& Menu::operator=(Menu&& other) noexcept
    {
        if (this != &other) {
            delete choices;          // 释放已有资源
            choices = other.choices; // 偷取资源
            other.choices = nullptr; // 清空源对象指针
        }
        return *this;
    }

    // --------------------------------------------------
    // 6.5 Destructor (Rule of Five)
    // --------------------------------------------------
    Menu::~Menu()
    {
        delete choices;
        choices = nullptr;
    }

    // --------------------------------------------------
    // 6.6 Default Constructor
    // --------------------------------------------------
    Menu::Menu()
    {
        choices = new std::map<int, std::string>();
    }

    // ---------------------------
    // the following are core methods
    // ---------------------------


    // --------------------------------------------------
    // getSize()
    // --------------------------------------------------
    int Menu::getSize() const
    {
        return choices->size();
    }

    // --------------------------------------------------
    // 6.7 operator[] – write access
    // --------------------------------------------------
    std::string& Menu::operator[](int key)
    {
     auto it = choices->find(key);
    if (it != choices->end()) {
        return it->second;
    }
   
    (*choices)[key] = "Default"; 
        return (*choices)[key];
    }

    // --------------------------------------------------
    // 6.8 operator[] – read access (const)
    // Returns "Nothing" if key not found
    // --------------------------------------------------
    const std::string &Menu::operator[](int key) const
    {
        auto it = choices->find(key);
        if (it != choices->end()) {
            return it->second;
        }
       static std::string default_str = "Default";
    return default_str;
    }

    // --------------------------------------------------
    // 6.9 operator += : add MenuItem
    // If key exists → error, do nothing
    // --------------------------------------------------
     Menu& Menu::operator+=(const MenuItem &item)
    {
        if (choices->find(item.first) != choices->end()) {
            cout << "Error: Key " << item.first << " already exists in the menu.\n";
        } else {
            choices->insert(item);
        }
        return *this;
    }

    // --------------------------------------------------
    // 6.10 operator -= : remove by key
    // --------------------------------------------------
      Menu& Menu::operator-=(int key)
    {
        choices->erase(key);
        return *this;
    }

    // --------------------------------------------------
    // 6.11 pick_choice()
    // Ask user for a valid menu key, loop until valid
    // --------------------------------------------------
    int Menu::pick_choice()
    {
        while (true) {
            // 调用之前实现的 Tools::read_int 函数进行安全的整数读取
            int choice = CS111::Tools::read_int("Enter your choice: ");
            if (choices->find(choice) != choices->end()) {
                return choice; // 找到了合法的菜单 Key，直接返回
            }
            cout << "Invalid choice. Please try again.\n";
        }
    }

    // --------------------------------------------------
    // 6.12 operator<< : display menu nicely
    // --------------------------------------------------
std::ostream& operator<<(std::ostream &os, const Menu &menu)
    {
        for (const auto &item : *menu.choices) {
            os << item.first << ". " << item.second << "\n";
        }
        return os;
    }

    

} // namespace CS111
