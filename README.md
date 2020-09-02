# MiniRogue-Clone
N.B. This project is finished.

# Description
This is a project pulled from a C++ course on data structures (COM SCI 32 @ UCLA), in which we were tasked with creating [from scratch] a fully functioning console-based MiniRogue clone. The game runs complete with multiple levels, character movement, combat functionality and monster control. I made use of various data structures, predominantly vectors, to optimize gameflow and content destruction. Recursion is heavily utilized to implement monster movement and decision making. 

N.B. As this is a project pulled from a CS course, the posting of this repository is to demonstrate the code and what I have learned, and is in no way meant to be a resource for plagiarism or cheating (copying code will only stunt your growth as a programmer anyways).

# Operation
To play the game, locate the file "main.cpp" (without the quotation marks). Assuming all the other C++ and header files are located in the correct directory, the game should run as is. For smooth operation, run the game in Release Mode in Visual Studio (as opposed to Debugger Mode). The parameter in main that is passed to the Game class dictates the monster smell distance. For a challenge, you may increase this number so that monsters can track your character across the entire dungeon! The game consists of five levels, and an inventory that can hold a maximum of 26 items. 
