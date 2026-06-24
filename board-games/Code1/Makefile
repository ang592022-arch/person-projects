# A simple makefile that can generate .o files and .obj files 
# It also works for both Mac and Windows. 
# all the .o .obj .exe .out files should appear in the Bin folder. 

# Where should we put this Makefile in the Bin folder? What is more common practice to put the Makefile? 
# Answer: It is more common practice to put the Makefile in the root directory of the project, not inside the Bin folder. The Makefile can then specify that the output files (like .o, .obj, .exe, .out) should be placed in the Bin folder. This way, the source code and build configuration are kept separate from the compiled binaries, which helps with organization and clarity.
CXXFLAGS += -std=c++17
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

Bin/testMyVector.o: Test/testMyVector.cpp
	$(CXX) -c Test/testMyVector.cpp -o Bin/testMyVector.o

Bin/testMyVector.obj: Test/testMyVector.cpp
	cl $(CLFLAGS) /c Test/testMyVector.cpp /FoBin/testMyVector.obj

Bin/testStone.o: Test/testStone.cpp
	$(CXX) -c Test/testStone.cpp -o Bin/testStone.o	

Bin/stones.o: Src/stones.cpp
	$(CXX) -c Src/stones.cpp -o Bin/stones.o

Bin/stones.obj: Src/stones.cpp
	cl $(CLFLAGS) /c Src/stones.cpp /FoBin/stones.obj

Bin/testStone.obj: Test/testStone.cpp
	cl $(CLFLAGS) /c Test/testStone.cpp /FoBin/testStone.obj

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

Bin/testMenu.o: Test/testMenu.cpp
	$(CXX) -c Test/testMenu.cpp -o Bin/testMenu.o

Bin/testMenu.obj: Test/testMenu.cpp
	cl $(CLFLAGS) /c Test/testMenu.cpp /FoBin/testMenu.obj

Bin/menu.o: Src/menu.cpp
	$(CXX) -c Src/menu.cpp -o Bin/menu.o

Bin/menu.obj: Src/menu.cpp
	cl $(CLFLAGS) /c Src/menu.cpp /FoBin/menu.obj


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


# for windows with g++
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


# for windows with cl
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

# for mac
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

winall: testMyVector.exe testStone.exe testTools.exe testMenu.exe testBoard.exe

winclall: clTestMyVector.exe clTestStone.exe clTestTools.exe clTestMenu.exe clTestBoard.exe

macall: testMyVector.out testStone.out testTools.out testMenu.out testBoard.out

# Clean up
macclean:
	rm -f Bin/*.o Bin/*.obj Bin/*.exe Bin/*.out
#	rm -f Bin/*.o   Bin/*.out

winclean:
	del Bin\*.o Bin\*.obj Bin\*.exe Bin\*.out	

