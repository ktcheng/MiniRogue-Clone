// Dungeon.h

#ifndef DUNGEON_INCLUDED
#define DUNGEON_INCLUDED

#include <string>
#include <vector>

/*
 Dungeon Design Notes:
 - By program design, dynamic cast is not used within this project, as I preferred to 
   keep all of my pointers of the base type.
 - Dungeon contains no derived classes. The status of each cell is stored with a list of
   constants, defined above the class definition. These contants are used throughout the 
   Dungeon class. 
 - While some functions may seem to have unintuitive return types (e.g. booleans that are
   not used later), this is deliberately done in order to see the flow of events in the code.
 - The Dungeon holds important information to the Game, such as a record of available 
   Items and the level's Actors. Vector containers are used to store this information.
 - For Dungeon layout, a private helper function is used to ensure that rooms do not overlap
   upon each other (i.e. they are distinct). Implementation of this is in the .cpp file.
 - For Game output, the Dungeon contains the appropriate strings, and command of all Game
   output is handled by the display() function.
*/

// Dungeon Dimensional Constants
const int DUNGEONROWS = 18;
const int DUNGEONCOLS = 70;

// Dungeon Cell Status Constants
const int EMPTY = 0; // Cell is empty
const int ACTOR = 1; // Cell contains Actor only
const int ITEM = 2; // Cell contains Item only
const int ACTORITEM = 3; // Cell contains both Actor and Item
const int WALLS = 9; // Cell is a wall

class Actor; // Indicates to compiler that Actor is a class
class Item; // Indicates to compiler that Item is a class

class Dungeon
{
public:
    // Constructors and Destructors
    Dungeon(int level, int smellDistance);
    ~Dungeon();

    // Accessor Functions
    int getRow() const; // Retrieve the Dungeon's row dimension
    int getCol() const; // Retrieve the Dungeon's column dimension
    int getLevel() const; // Retrieve the Dungeon's current level
    int getStatus(int r, int c) const; // Retrieve the Dungeon's cell status
    int actorAtPos(int r, int c) const; // Retrieve index of Actor at the position
    int itemAtPos(int r, int c) const; // Retrieve index of Item at the position
    bool inBounds(int r, int c) const; // Determines if given coordinates are valid
    void display() const; // Displays the Dungeon

    Actor* player() const; // Retrieves the Dungeon's Player pointer
    std::vector<Item*> getAvailItems() const; // Retrieves the vector of available items
    std::vector<Actor*> getMonsters() const; // Retrieves the vector of monsters

    // Mutator Functions
    bool addPlayer(int r, int c); // Adds Player to the Dungeon
    bool addrandItem(int r, int c, bool sflag, bool gflag); // Adds Item to the Dungeon

    // Adds the monster drop reward
    void addDrop(int r, int c, std::string name, std::string weaponClass);
    void generateLayout(int level); // Generates a unique Dungeon layout
    void newLevel(int level); // Generates a new Dungeon
    void removeItem(int index); // Removes Item from Dungeon
    void removeMonster(int index); // Removes monster from Dungeon
    void setStatus(int r, int c, int status); // Sets the Dungeon's cell status
   
    // Output String Functions
    void setPlayerAttackString(std::string s); // Sets Player's attack result
    void setPlayerPickUpString(std::string s); // Sets Player's pick up Item result
    void setPlayerScrollString(std::string s); //Sets Player's read Scroll result
    void setPlayerWieldString(std::string s); // Sets Player's wield Weapon result
    void setMonsterAttackString(std::string s); // Sets monster's attack result
    void setEmptyMonsterString(); // Sets monster's attack result to the empty string

private:
    // Private Layout Helper Function
    bool overlap(int crow, int ccol, int dwidth, int dheight);

    // Dungeon's data members
    int m_rows; // The Dungeon's row dimension
    int m_cols; // The Dungeon's column dimension
    int m_grid[DUNGEONROWS][DUNGEONCOLS]; // The Dungeon's information grid
    int m_level; // The Dungeon's level
    int m_monsterNum; // The Dungeon's number of monsters
    int m_smellDistance; // The Dungeon's Goblin smell distance

    Actor* m_player; // The Dungeon's Player
    std::vector<Item*> m_availItems; // The Dungeon's vector of available Items
    std::vector<Actor*> m_monsterList; // The Dungeon's vector of monsters

    std::string m_playerAttackResult; // The Dungeon's Player attack string
    std::string m_playerPickedUpItem; // The Dungeon's Player pick up string
    std::string m_playerReadScroll; // The Dungeon's Player read Scroll string
    std::string m_playerWieldWeapon; // The Dungeon's Player wield Weapon string
    std::string m_monsterAttackResult; // The Dungeon's monster attack string
};

#endif // DUNGEON_INCLUDED