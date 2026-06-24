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
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

using CS111::Menu;
using CS111::MenuItem;
using CS111::Tools::Test::TestContext;
using CS111::Tools::Test::check_equal_int;
using CS111::Tools::Test::check_equal_str;
using CS111::Tools::Test::check_true;
using CS111::Tools::Test::print_test_banner;
using CS111::Tools::Test::print_test_separator;

namespace {

Menu make_sample_menu() {
    Menu m;
    m += Menu::make_menu_item(1, "Start quest");
    m += Menu::make_menu_item(2, "Explore cave");
    m += Menu::make_menu_item(3, "Rest at camp");
    return m;
}

} // namespace

int main() {
    TestContext ctx;
    print_test_banner("MENU CLASS TEST SUITE");

    // ---------------------------------------------------------------------
    // Test Task 1: Basic construction, += insertion, duplicate-key behavior
    // ---------------------------------------------------------------------
    print_test_separator("1. Basic Construction and += Behavior");

    Menu menu;
    menu += Menu::make_menu_item(1, "Pizza");
    menu += Menu::make_menu_item(2, "Burger");
    menu += Menu::make_menu_item(3, "Salad");

    check_equal_int(ctx, menu.getSize(), 3, "Size after three insertions");
    check_equal_str(ctx, menu[1], "Pizza", "Key 1 maps to Pizza");

    menu += Menu::make_menu_item(2, "Duplicate should be ignored");
    check_equal_int(ctx, menu.getSize(), 3, "Duplicate key does not grow map");
    check_equal_str(ctx, menu[2], "Burger", "Original value kept for duplicate key");

    // ---------------------------------------------------------------------
    // Test Task 2: operator[] read/write, const lookup, and -= behavior
    // ---------------------------------------------------------------------
    print_test_separator("2. operator[] and -= Behavior");

    menu[3] = "Soup";
    check_equal_str(ctx, menu[3], "Soup", "Non-const operator[] updates existing key");

    menu[10] = "Tea"; // non-const operator[] inserts missing key
    check_equal_int(ctx, menu.getSize(), 4, "Non-const operator[] inserts missing key");
    check_equal_str(ctx, menu[10], "Tea", "Inserted key 10 has expected value");

    const Menu &const_menu = menu;
    check_equal_str(ctx, const_menu[999], "Nothing", "Const operator[] missing key returns Nothing");

    menu -= 10;
    check_equal_int(ctx, menu.getSize(), 3, "Erasing an existing key shrinks size");

    menu -= 12345;
    check_equal_int(ctx, menu.getSize(), 3, "Erasing a missing key does nothing");

    // ---------------------------------------------------------------------
    // Test Task 3: Deep copy correctness (copy ctor + copy assignment)
    // ---------------------------------------------------------------------
    print_test_separator("3. Deep Copy Behavior");

    Menu copy_constructed(menu);
    copy_constructed[1] = "Steak";
    check_equal_str(ctx, copy_constructed[1], "Steak", "Copy-constructed object can be modified");
    check_equal_str(ctx, menu[1], "Pizza", "Original unchanged after copy-construction");

    Menu copy_assigned;
    copy_assigned = menu;
    copy_assigned[2] = "Pasta";
    check_equal_str(ctx, copy_assigned[2], "Pasta", "Copy-assigned object can be modified");
    check_equal_str(ctx, menu[2], "Burger", "Original unchanged after copy-assignment");

    // ---------------------------------------------------------------------
    // Test Task 4: Move constructor behavior (actual usage + effects)
    // ---------------------------------------------------------------------
    print_test_separator("4. Move Constructor Behavior");

    Menu move_source_ctor = make_sample_menu();
    check_equal_int(ctx, move_source_ctor.getSize(), 3, "Move-source (ctor) starts with 3 items");

    // Explicitly invokes move constructor.
    Menu move_target_ctor(std::move(move_source_ctor));

    check_equal_int(ctx, move_target_ctor.getSize(), 3, "Move target (ctor) received all items");
    check_equal_str(ctx, move_target_ctor[1], "Start quest", "Moved data key 1 preserved");
    check_equal_str(ctx, move_target_ctor[2], "Explore cave", "Moved data key 2 preserved");

    // Moved-from object should still be assignable and safely reusable.
    move_source_ctor = Menu();
    move_source_ctor += Menu::make_menu_item(7, "Reused after move");
    check_equal_int(ctx, move_source_ctor.getSize(), 1, "Moved-from source can be reused via assignment");
    check_equal_str(ctx, move_source_ctor[7], "Reused after move", "Reused moved-from source works");

    // ---------------------------------------------------------------------
    // Test Task 5: Move assignment behavior (actual usage + effects)
    // ---------------------------------------------------------------------
    print_test_separator("5. Move Assignment Behavior");

    Menu move_source_assign;
    move_source_assign += Menu::make_menu_item(11, "Alpha");
    move_source_assign += Menu::make_menu_item(12, "Beta");

    Menu move_target_assign;
    move_target_assign += Menu::make_menu_item(99, "Old data to be replaced");
    check_equal_int(ctx, move_target_assign.getSize(), 1, "Move target initially has old data");

    // Explicitly invokes move assignment.
    move_target_assign = std::move(move_source_assign);

    check_equal_int(ctx, move_target_assign.getSize(), 2, "Move-assigned target now owns source data");
    check_equal_str(ctx, move_target_assign[11], "Alpha", "Move-assigned key 11 preserved");
    check_equal_str(ctx, move_target_assign[12], "Beta", "Move-assigned key 12 preserved");

    // Moved-from object should still be assignable and safely reusable.
    move_source_assign = Menu();
    move_source_assign += Menu::make_menu_item(20, "Gamma");
    check_equal_int(ctx, move_source_assign.getSize(), 1, "Move-assignment source can be reused");
    check_equal_str(ctx, move_source_assign[20], "Gamma", "Reused move-assignment source works");

    // ---------------------------------------------------------------------
    // Test Task 6: pick_choice behavior (valid and retry path)
    // ---------------------------------------------------------------------
    print_test_separator("6. pick_choice Behavior");

    Menu pick_menu;
    pick_menu += Menu::make_menu_item(1, "Play");
    pick_menu += Menu::make_menu_item(2, "Options");
    pick_menu += Menu::make_menu_item(0, "Quit");

    std::streambuf* original_cin_buf = std::cin.rdbuf();

    {
        std::istringstream simulated_input("2\n");
        std::cin.rdbuf(simulated_input.rdbuf());
        int picked = pick_menu.pick_choice();
        check_equal_int(ctx, picked, 2, "pick_choice returns a directly valid key");
    }

    {
        std::istringstream simulated_input("999\n0\n");
        std::cin.rdbuf(simulated_input.rdbuf());
        int picked = pick_menu.pick_choice();
        check_equal_int(ctx, picked, 0, "pick_choice retries after invalid key and accepts next valid key");
    }

    std::cin.rdbuf(original_cin_buf);

    // ---------------------------------------------------------------------
    // Test Task 7: Interactive story menu (manual loop until quit)
    // ---------------------------------------------------------------------
    print_test_separator("7. Interactive Story Menu (Manual)");

    Menu story_menu;
    story_menu += Menu::make_menu_item(0, "Quit the song");
    story_menu += Menu::make_menu_item(1, "Dog bark");
    story_menu += Menu::make_menu_item(2, "Bee sting");
    story_menu += Menu::make_menu_item(3, "Bird sing");

    bool chose_dog = false;
    bool chose_bee = false;
    bool chose_bird = false;
    int rounds = 0;

    std::cout << "Welcome to My Favorite Things mini story.\n";
    std::cout << "Please try at least one invalid choice first (example: 9 or abc), then continue.\n";
    std::cout << "The menu keeps running until you choose 0 to quit.\n";

    while (true) {
        ++rounds;
        std::cout << "\n--- Story Round " << rounds << " ---\n";
        std::cout << story_menu;

        int picked = story_menu.pick_choice();

        if (picked == 0) {
            std::cout << "Good night, little song. Thanks for playing. 🌙\n";
            break;
        }

        // to find the emoji codes, I used https://emojipedia.org/ , or, https://unicode.org/emoji/charts/full-emoji-list.html, and copied the emoji directly into the string literals below.
        // to show emoji on windows, use the command: chcp 65001 in the terminal to switch to UTF-8 encoding, and make sure to use a font that supports emoji (like Segoe UI Emoji).
        if (picked == 1) {
            chose_dog = true;
            std::cout << "🐶 The dog says: WOOF WOOF! The hills echo back.\n";
        } else if (picked == 2) {
            chose_bee = true;
            std::cout << "🐝 Ouch! Bee sting! But the melody keeps you smiling.\n";
        } else if (picked == 3) {
            chose_bird = true;
            std::cout << "🐦 A bird sings a bright note: la-la-la!\n";
        }
    }

    check_true(ctx, rounds >= 1, "Interactive loop runs at least one round");
    check_true(ctx, chose_dog || chose_bee || chose_bird,
               "At least one non-quit story action is exercised before exit");

    print_test_separator("Summary");
    if (ctx.failed_checks == 0) {
        std::cout << "All checks passed.\n";
        return 0;
    }

    std::cout << "Total failed checks: " << ctx.failed_checks << "\n";
    return EXIT_FAILURE;
}
