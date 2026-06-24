
1. Include/tools.hpp
 add back the `namespace Tools { ... }` layer inside `namespace CS111`.
Reason: namespace structure is important and small enough for an early task.

2. Src/tools.cpp
implement `clear_input_queue()`.
Focus: `cin.get()`, loop until newline or EOF, clear stream state carefully.

3. Src/tools.cpp
implement `read_int(const std::string&)`.
Focus: input-validation loop, `cin.fail()`, reuse of `clear_input_queue()`.

4. Include/menu.hpp
Provide the missing declaration of 12 methods:  4.1 ... 4.12
Comments show the numbers of these 12 subtasks. They include: 
- Rule of Five functions
- default constructor
- `operator[]`
- `operator+=`
- `operator-=`
- `pick_choice`
- stream output friend

5. Src/menu.cpp
implement `make_menu_item`.
Focus: `std::pair` / `std::make_pair`.

6. Src/menu.cpp
totally 12 pieces of function definitions 6.1 ... 6.12
- They correspond to the declarations of 4.1 ... 4.12. 

7. Src/stones.cpp
 implement   `showShape(Shape)`.
Focus: `switch` on enum class values.

8. Src/stones.cpp
 implement copy constructor  
Focus: deep copy  

9. Src/stones.cpp
 implement  the destructor.
 memory cleanup.

10. Src/stones.cpp
 implement `setStone(...)`.
Focus: bounds checking and field assignment.

11. Include/myVector.tpp
implement copy assignment operator.
Focus: delete old memory, allocate new memory, copy elements.

12. Include/myVector.tpp
implement move assignment operator.
Focus: transfer ownership and reset moved-from object.

13. Include/myVector.tpp
 implement `resize()`.
Focus: capacity doubling and copying existing elements.

14. Src/board.cpp
Task 19: implement `validate_xy(int x, int y) const`.
Focus: coordinate range checking and exception throwing.
note: add comments

15. Src/board.cpp
 implement `to_xy(int row, char col) const`.
Focus: uppercase conversion, skip `I`, convert board labels to internal coordinates.

16. Src/board.cpp
implement `remove_stone(int x, int y)`.
Focus: reuse `at_xy` and `empty_stone_`.

17. Src/board.cpp
implement `make_empty()`.
Focus: nested loops over the board and reuse of `empty_stone_`.

18.  Src/board.cpp
 in the `display_board(...)` function  
Implement the `print_centered_text` lambda expression 

19. Src/board.cpp
 in the `display_board(...)` function  
Write the body of the `if (layout.no_grid_lines)` part. 

20. Test/testBoard.cpp
Add one interactive test, 
- show a 9x9 go board, empty, print it
- ask user to put three stones on the board
    - The user tells the color (black or white), and positions of the three stones. 
    - print the board
    - then delete one stone from the board (by printing an empty stone)
    - then print the board. 
