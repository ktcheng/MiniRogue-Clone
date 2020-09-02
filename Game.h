// Game.h

#ifndef GAME_INCLUDED
#define GAME_INCLUDED

/*
 Game Design Notes:
 - By program design, dynamic cast is not used within this project, as I preferred to 
   keep all of my pointers of the base type.
 - Game is by far my most simple class. It contains no derived classes, and only three 
   notable functions: one to play Game, one to take Player turn and one for monster turn.
 - The only data member is Game's Dungeon, which will contain all the Actors, Items, the 
   status of each cell, as well as display the state of the Game.
 - The Game class is the predominant driver code for this project. It's tasks include making
   sure that functions are not called inappropriately and that play is smooth.
*/

class Dungeon; // Indicates to compiler that Dungeon is a class

class Game
{
public:
    // Constructors and Destructors
    Game(int goblinSmellDistance);
    ~Game();

    // Mutator Functions
    void play(); // Plays Game
    void playerTurn(char ckey, bool& cheat); // Takes Player turn
    void monsterTurn(); // Takes monsters' turn
private:
    Dungeon* m_dungeon; // The Game's Dungeon
};

#endif // GAME_INCLUDED