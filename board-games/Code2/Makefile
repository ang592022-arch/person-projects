#*****************************************************
# * Course: CS111  | Term: 2026 Spring
# * Instructor: zyliang@must.edu.mo
# * Homework Assignment: 3
# *****************************************************

# A simple makefile that can generate .o files and .obj files 
# It also works for both Mac and Windows. 
# all the .o .obj .exe .out files should appear in the Bin folder. 

# Where should we put this Makefile in the Bin folder? What is more common practice to put the Makefile? 
# Answer: It is more common practice to put the Makefile in the root directory of the project, not inside the Bin folder. The Makefile can then specify that the output files (like .o, .obj, .exe, .out) should be placed in the Bin folder. This way, the source code and build configuration are kept separate from the compiled binaries, which helps with organization and clarity.

# Compiler
# replace g++ with an available compiler on your system if needed (e.g., cl for Windows, or clang on some OS). 
CXX = g++

# why /utf-8 flag is needed for cl compiler?
# without it, compiling has the error:
#  warning C4819: The file contains a character that cannot be represented in the current code page (936). Save the file in Unicode format to prevent data loss

# Example only: CXXFLAGS = -std=c++17 -Wall -IInclude
# -Wall asks g++/clang++ to show many common warnings.
# -IInclude tells g++/clang++ to add Include/ to the header search path.
# /IInclude does the same job for the Windows cl compiler.
# In this project, the .cpp files already use relative includes such as "../Include/tools.hpp",
# so these compile commands do not need -IInclude or /IInclude.
# /EHsc means to enable C++ exception handling, and /utf-8 means to treat source files as UTF-8 encoded. This is important for ensuring that the compiler correctly handles any non-ASCII characters in the source code, which can prevent warnings about character encoding issues.
CLFLAGS = /std:c++17 /EHsc /utf-8

# Note that the .o files generated on Windows cannot be used on Mac and vice versa. So, run "make macclean" or "make winclean"  before compiling on a specific OS. 

# The rules for generating  .o files using g++ can be shared for both Windows, Mac, and Linux. 
# The rules for generating .obj files using cl are only for Windows.
# The rules for generating .exe files using g++ are only for Windows only. 
# The rules for generating .out files using g++ can be shared for both Mac and Linux. 

# the rules from the same source file (or module) are put next to each other. 
# for example, the rules for stones.o, stones.obj, and testStone.o, testStone.obj are put together, because they are all related to the stones module.
# The rules for executable files are put at end, because they depend on the .o and .obj files, while the .o and .obj files do not depend on the executable files.


# ====================
# generating .o and .obj files
# ====================

# --------- MyVector object files -----

Bin/testMyVector.o: Test/testMyVector.cpp
	$(CXX) -c Test/testMyVector.cpp -o Bin/testMyVector.o

Bin/testMyVector.obj: Test/testMyVector.cpp
	cl $(CLFLAGS) /c Test/testMyVector.cpp /FoBin/testMyVector.obj

# --------- stone object files -----
Bin/stones.o: Src/stones.cpp
	$(CXX) -c Src/stones.cpp -o Bin/stones.o

Bin/stones.obj: Src/stones.cpp
	cl $(CLFLAGS) /c Src/stones.cpp /FoBin/stones.obj

Bin/testStone.o: Test/testStone.cpp
	$(CXX) -c Test/testStone.cpp -o Bin/testStone.o	

Bin/testStone.obj: Test/testStone.cpp
	cl $(CLFLAGS) /c Test/testStone.cpp /FoBin/testStone.obj


# ---- tools object files ----

#  warning: nested namespace definition is a C++17 extension [-Wc++17-extensions]
Bin/tools.o: Src/tools.cpp
	$(CXX) -std=c++17 -c Src/tools.cpp -o Bin/tools.o

Bin/tools.obj: Src/tools.cpp Makefile
	cl $(CLFLAGS) /c Src/tools.cpp /FoBin/tools.obj

Bin/testTools.o: Test/testTools.cpp
	$(CXX) -c -std=c++17 Test/testTools.cpp -o Bin/testTools.o	

# : error C2429: language feature 'nested-namespace-definition' requires compiler flag '/std:c++17'
Bin/testTools.obj: Test/testTools.cpp Makefile
	cl $(CLFLAGS) /c Test/testTools.cpp /FoBin/testTools.obj

# ----- menu object files -----
Bin/testMenu.o: Test/testMenu.cpp
	$(CXX) -c Test/testMenu.cpp -o Bin/testMenu.o

Bin/testMenu.obj: Test/testMenu.cpp
	cl $(CLFLAGS) /c Test/testMenu.cpp /FoBin/testMenu.obj

Bin/menu.o: Src/menu.cpp
	$(CXX) -c Src/menu.cpp -o Bin/menu.o

Bin/menu.obj: Src/menu.cpp
	cl $(CLFLAGS) /c Src/menu.cpp /FoBin/menu.obj


# ----- board object files -----

Bin/board.o: Src/board.cpp
	$(CXX) -std=c++17 -c Src/board.cpp -o Bin/board.o

Bin/board.obj: Src/board.cpp
	cl $(CLFLAGS) /c Src/board.cpp /FoBin/board.obj

Bin/board_presets.o: Src/board_presets.cpp
	$(CXX) -std=c++17 -c Src/board_presets.cpp -o Bin/board_presets.o

Bin/board_presets.obj: Src/board_presets.cpp
	cl $(CLFLAGS) /c Src/board_presets.cpp /FoBin/board_presets.obj

Bin/testBoard.o: Test/testBoard.cpp
	$(CXX) -std=c++17 -c Test/testBoard.cpp -o Bin/testBoard.o

Bin/testBoard.obj: Test/testBoard.cpp
	cl $(CLFLAGS) /c Test/testBoard.cpp /FoBin/testBoard.obj


# --------- util object files -----
Bin/utils.o: Src/utils.cpp
	$(CXX) -std=c++17 -c Src/utils.cpp -o Bin/utils.o

Bin/testUtils.o: Test/testUtils.cpp
	$(CXX) -std=c++17 -c Test/testUtils.cpp -o Bin/testUtils.o

Bin/utils.obj: Src/utils.cpp
	cl $(CLFLAGS) /c Src/utils.cpp /FoBin/utils.obj

Bin/testUtils.obj: Test/testUtils.cpp
	cl $(CLFLAGS) /c Test/testUtils.cpp /FoBin/testUtils.obj

# -------- BoardGame object files ---------

Bin/boardGame.o: Src/boardGame.cpp
	$(CXX) -std=c++17 -c Src/boardGame.cpp -o Bin/boardGame.o

Bin/boardGame.obj: Src/boardGame.cpp
	cl $(CLFLAGS) /c Src/boardGame.cpp /FoBin/boardGame.obj	

# -------- boardGameTools object files ---------
Bin/boardGameTools.o: Src/boardGameTools.cpp
	$(CXX) -std=c++17 -c Src/boardGameTools.cpp -o Bin/boardGameTools.o

Bin/boardGameTools.obj: Src/boardGameTools.cpp
	cl $(CLFLAGS) /c Src/boardGameTools.cpp /FoBin/boardGameTools.obj

# -------- GoLike object files ---------
Bin/goLike.o: Src/goLike.cpp
	$(CXX) -std=c++17 -c Src/goLike.cpp -o Bin/goLike.o

Bin/goLike.obj: Src/goLike.cpp
	cl $(CLFLAGS) /c Src/goLike.cpp /FoBin/goLike.obj

# -------- GoBang object files ---------

Bin/goBang.o: Src/goBang.cpp
	$(CXX) -std=c++17 -c Src/goBang.cpp -o Bin/goBang.o

Bin/goBang.obj: Src/goBang.cpp
	cl $(CLFLAGS) /c Src/goBang.cpp /FoBin/goBang.obj

Bin/testGoBang.o: Test/testGoBang.cpp
	$(CXX) -std=c++17 -c Test/testGoBang.cpp -o Bin/testGoBang.o

Bin/testGoBang.obj: Test/testGoBang.cpp
	cl $(CLFLAGS) /c Test/testGoBang.cpp /FoBin/testGoBang.obj

# -------- NQueen object files ---------
Bin/nQueen.o: Src/nQueen.cpp
	$(CXX) -std=c++17 -c Src/nQueen.cpp -o Bin/nQueen.o

Bin/nQueen.obj: Src/nQueen.cpp
	cl $(CLFLAGS) /c Src/nQueen.cpp /FoBin/nQueen.obj

Bin/testNQueen.o: Test/testNQueen.cpp
	$(CXX) -std=c++17 -c Test/testNQueen.cpp -o Bin/testNQueen.o

Bin/testNQueen.obj: Test/testNQueen.cpp
	cl $(CLFLAGS) /c Test/testNQueen.cpp /FoBin/testNQueen.obj


# ====================================================
# generating Windows executables (.exe)  based on g++
# ====================================================

testMyVector.exe: Bin/testMyVector.o Bin/tools.o
	$(CXX) Bin/testMyVector.o Bin/tools.o -o Bin/testMyVector.exe

testStone.exe: Bin/testStone.o Bin/stones.o Bin/tools.o
	$(CXX) Bin/testStone.o Bin/stones.o Bin/tools.o -o Bin/testStone.exe

testTools.exe: Bin/testTools.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testTools.o Bin/tools.o -o Bin/testTools.exe

testMenu.exe: Bin/testMenu.o Bin/menu.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testMenu.o Bin/menu.o Bin/tools.o -o Bin/testMenu.exe

testBoard.exe: Bin/testBoard.o Bin/board.o Bin/board_presets.o Bin/stones.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testBoard.o Bin/board.o Bin/board_presets.o Bin/stones.o Bin/tools.o -o Bin/testBoard.exe

# util test executable rule
testUtils.exe: Bin/testUtils.o Bin/utils.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testUtils.o Bin/utils.o Bin/tools.o -o Bin/testUtils.exe

# GoBang test executable rule
testGoBang.exe: Bin/testGoBang.o Bin/goBang.o Bin/boardGameTools.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o 
	$(CXX) -std=c++17 Bin/testGoBang.o Bin/goBang.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o -o Bin/testGoBang.exe

# NQueen test executable rule
testNQueen.exe: Bin/testNQueen.o Bin/nQueen.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o  Bin/utils.o
	$(CXX) -std=c++17 Bin/testNQueen.o Bin/nQueen.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/utils.o -o Bin/testNQueen.exe


# ====================================================
# generating Windows executables (.exe)  based on cl
# ====================================================

clTestMyVector.exe: Bin/testMyVector.obj Bin/tools.obj
	cl Bin/testMyVector.obj Bin/tools.obj /FeBin/clTestMyVector.exe

clTestStone.exe: Bin/testStone.obj Bin/stones.obj Bin/tools.obj
	cl Bin/testStone.obj Bin/stones.obj Bin/tools.obj /FeBin/clTestStone.exe

# error C2429: language feature 'nested-namespace-definition' requires compiler flag '/std:c++17'
clTestTools.exe: Bin/testTools.obj Bin/tools.obj
# cl Bin/testTools.obj Bin/tools.obj /FeBin/clTestTools.exe
	cl $(CLFLAGS) Bin/testTools.obj Bin/tools.obj /FeBin/clTestTools.exe


clTestMenu.exe: Bin/testMenu.obj Bin/menu.obj Bin/tools.obj
	cl Bin/testMenu.obj Bin/menu.obj Bin/tools.obj /FeBin/clTestMenu.exe

clTestBoard.exe: Bin/testBoard.obj Bin/board.obj Bin/board_presets.obj Bin/stones.obj Bin/tools.obj
	cl Bin/testBoard.obj Bin/board.obj Bin/board_presets.obj Bin/stones.obj Bin/tools.obj /FeBin/clTestBoard.exe

# util test executable rule
clTestUtils.exe: Bin/testUtils.obj Bin/utils.obj Bin/tools.obj
	cl $(CLFLAGS) Bin/testUtils.obj Bin/utils.obj Bin/tools.obj /FeBin/clTestUtils.exe

# GoBang test executable rule
clTestGoBang.exe: Bin/testGoBang.obj Bin/goBang.obj Bin/tools.obj Bin/board.obj Bin/boardGame.obj Bin/boardGameTools.obj Bin/board_presets.obj Bin/stones.obj Bin/goLike.obj Bin/utils.obj 
	cl $(CLFLAGS) Bin/testGoBang.obj Bin/goBang.obj Bin/tools.obj Bin/board.obj Bin/boardGame.obj Bin/boardGameTools.obj Bin/board_presets.obj Bin/stones.obj Bin/goLike.obj Bin/utils.obj /FeBin/clTestGoBang.exe

# NQueen test executable rule
clTestNQueen.exe: Bin/testNQueen.obj Bin/nQueen.obj Bin/tools.obj Bin/board.obj Bin/boardGame.obj Bin/boardGameTools.obj Bin/board_presets.obj Bin/stones.obj  Bin/utils.obj
	cl $(CLFLAGS) Bin/testNQueen.obj Bin/nQueen.obj Bin/tools.obj Bin/board.obj Bin/boardGame.obj Bin/boardGameTools.obj Bin/board_presets.obj Bin/stones.obj Bin/utils.obj /FeBin/clTestNQueen.exe


# ====================================================
# generating Mac/Linux executables (.out) based on g++
# ====================================================
testMyVector.out: Bin/testMyVector.o Bin/tools.o
	$(CXX) Bin/testMyVector.o Bin/tools.o -o Bin/testMyVector.out

testStone.out: Bin/testStone.o Bin/stones.o
	$(CXX) Bin/testStone.o Bin/stones.o Bin/tools.o -o Bin/testStone.out

# nested namespace definition is a C++17 extension [-Wc++17-extensions]
testTools.out: Bin/testTools.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testTools.o Bin/tools.o -o Bin/testTools.out

testMenu.out: Bin/testMenu.o Bin/menu.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testMenu.o Bin/menu.o Bin/tools.o -o Bin/testMenu.out

testBoard.out: Bin/testBoard.o Bin/board.o Bin/board_presets.o Bin/stones.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testBoard.o Bin/board.o Bin/board_presets.o Bin/stones.o Bin/tools.o -o Bin/testBoard.out

testUtils.out: Bin/testUtils.o Bin/utils.o Bin/tools.o
	$(CXX) -std=c++17 Bin/testUtils.o Bin/utils.o Bin/tools.o -o Bin/testUtils.out

testGoBang.out: Bin/testGoBang.o Bin/goBang.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o
	$(CXX) -std=c++17 Bin/testGoBang.o Bin/goBang.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o -o Bin/testGoBang.out

testNQueen.out: Bin/testNQueen.o Bin/nQueen.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o
	$(CXX) -std=c++17 Bin/testNQueen.o Bin/nQueen.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/goLike.o Bin/utils.o -o Bin/testNQueen.out


# ====================================================
# rules for generating all executables and cleaning up
# mac and linux can share the same rules
# ====================================================

winall: testMyVector.exe testStone.exe testTools.exe testMenu.exe testBoard.exe testUtils.exe testGoBang.exe testNQueen.exe

winclall: clTestMyVector.exe clTestStone.exe clTestTools.exe clTestMenu.exe clTestBoard.exe clTestUtils.exe clTestGoBang.exe clTestNQueen.exe

macall: testMyVector.out testStone.out testTools.out testMenu.out testBoard.out testUtils.out testGoBang.out testNQueen.out

# Clean up
macclean:
	rm -f Bin/*.o Bin/*.obj Bin/*.exe Bin/*.out
#	rm -f Bin/*.o   Bin/*.out

winclean:
	del Bin\*.o Bin\*.obj Bin\*.exe Bin\*.out	


# !!!!!!!!!!!!!! < Hmk3 Task 11 > !!!!!!!!!!!!!!
# Add the rules for the newly added programming files for 

Bin/testNQueenAlg.o: Test/testNQueenAlg.cpp Include/nQueenAlg.hpp
	g++ -std=c++17 -c $< -o $@
Bin/nQueenAlg.o: Src/nQueenAlg.cpp Include/nQueenAlg.hpp
	g++ -std=c++17 -c $< -o $@
Bin/testNQueenAlg.exe: Bin/testNQueenAlg.o Bin/nQueenAlg.o Bin/nQueen.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/utils.o
	g++ -std=c++17 $^ -o $@
Bin/testConnect.o: Test/testConnect.cpp Include/connect.hpp
	g++ -std=c++17 -c $< -o $@
Bin/connect.o: Src/connect.cpp Include/connect.hpp
	g++ -std=c++17 -c $< -o $@


Bin/testConnect.exe: Bin/testConnect.o Bin/connect.o Bin/goLike.o Bin/tools.o Bin/board.o Bin/boardGame.o Bin/boardGameTools.o Bin/board_presets.o Bin/stones.o Bin/utils.o
	g++ -std=c++17 $^ -o $@

winall: ...原有列表... Bin/testNQueenAlg.exe Bin/testConnect.exe
# Put all the rules to generate .o, .obj, .exe, and .out files related to Connect and NQueenAlg modules.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

