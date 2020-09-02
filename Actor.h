// Actor.h

#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include <string>
#include <vector>

/*
 Actor [and Derived Class] Design Notes:
 - By program design, dynamic cast is not used within this project, as I preferred to 
   keep all of my pointers of the base type.
    - As a consequence of this, all Player specific functions are defined in the Actor
      class, and they are made virtual. The Monsters' version of those functions do 
      nothing, as a result.
 - Actor has five derived classes: Player, Bogeyman, Dragon, Goblin and Snakewoman.
 - The pure virtual functions inside the Actor class are the move(), readScroll() and
   wieldWeapon() functions. All actors move differently, and only the player may read
   scrolls or wield weapons.
 - While some functions may seem to have unintuitive return types (e.g. booleans that are
   not used later), this is deliberately done in order to see the flow of events in the code.
 - All Actors have a pointer to the dungeon they belong to, their character statistics
   and their type (player, bogeyman, etc.).
 - As protected data members are forbidden, there are some data members that really
   only pertain to the player, and are not accessed unless in reference to the player.
 - The player maintains a personal vector of Item*, which represents its inventory, as 
   well as the index in the inventory of the current weapon it is wielding.
 - All monsters have an Item* pointing to the currently wielded weapon. Only Goblin has
   an additional parameter, which is its smell distance.
*/

class Dungeon; // Indicates to compiler that Dungeon is a class
class Item; // Indicates to compiler that Dungeon is a class

// Abstract Base Class
class Actor
{
public:
    // Constructors and Destructors
    Actor(Dungeon* d, int sr, int sc);
    virtual ~Actor();

    // Accessor Functions
    int row() const; // Retrieve the Actor's row
    int col() const; // Retrieve the Actor's column
    int getArmor() const; // Retrieve the Actor's armor points
    int getDexterity() const; // Retrieve the Actor's dexterity points
    int getHP() const; // Retrieve the Actor's hit points
    int getMaxHP() const; // Retrieve the Actor's maximum hit points
    int getSleep() const; // Retrieve the Actor's sleep time
    int getStrength() const; // Retrieve the Actor's strength points

    virtual int getWeaponIndex() const; // Retrieve the index of the weapon wielded
    bool isDead() const; // Retrieve the Actor's dead status
    bool isWinner() const; // Retrieve the Actor's winner status
    Dungeon* getActorDungeon() const; // Retrieve the Actor's dungeon pointer
    std::string getActorName() const; // Retrieve the Actor's type
    virtual void dispInventory() const; // Displays the inventory
    
    // Mutator Functions
    void changeCoordinates(int deltaRow, int deltaCol); // Changes the Actor's coordinates
    void changeStats(int modifier, std::string category); // Changes the Actor's statistics
    virtual bool pickUp(int r, int c, bool gflag); // Actor's pick up Item call
    void monsterDrop(std::string name); // Creates the Actor's Item drop
    virtual bool attack(int r, int c); // Actor (Monster) attack call

    void setArmor(int armor); // Sets the Actor's armor points
    void setDexterity(int dexterity); // Sets the Actor's dexterity points
    void setHP(int hp); // Sets the Actor's initial hit points
    void setMaxHP(int maxHP); // Sets the Actor's maximum hit points
    void setSleep(int time); // Sets the Actor's sleep time
    void setStrength(int strength); // Sets the Actor's strength points

    void setActorType(std::string name); // Sets the Actor's type
    void setDead(); // Kills the Actor
    void setWinner(); // Sets the Actor as the winner

    // Pure Virtual Functions
    virtual bool move(int drow, int dcol) = 0; // Player and Monsters move differently
    virtual bool readScroll(char c) = 0; // Player reads selected scroll, if possible
    virtual bool wieldWeapon(char c) = 0; // Player wields selected item, if possible
    
private:
	int m_acol; // The Actor's column coordinate
	int m_arow; // The Actor's row coordinate
    int m_armorPoints; // The Actor's armor points
    int m_dexterityPoints; // The Actor's dexterity points
	int m_hitPoints; // The Actor's hit points
    int m_maxHP; // The Actor's maximum hit points
    int m_sleepTime; // The Actor's sleep time
    int m_strengthPoints; // The Actor's strength points
    
    bool m_isWinner; // The Actor's winner status
    bool m_isDead; // The Actor's live status

    Dungeon* m_dungeon; // The Actor's Dungeon
    std::string m_actorType; // The Actor's type
};

// Player Derived Class
class Player : public Actor
{
public:
    // Constructors and Destructors
    Player(Dungeon* d, int sr, int sc);
    virtual ~Player();

    // Accessor Functions
    virtual int getWeaponIndex() const; // Retrieve the Player's Weapon index
    virtual void dispInventory() const; // Displays the Player's inventory

    // Mutator Functions
    virtual bool attack(int r, int c); // Calls the Player attack
    virtual bool move(int drow, int dcol); // Moves the Player
    virtual bool pickUp(int r, int c, bool gflag); // Player picks up Item call
    virtual bool readScroll(char c); // Player reads Scroll call
    virtual bool wieldWeapon(char c); // Player wields Weapon call
private:
    std::vector<Item*> m_inventory; // The Player's inventory of Items
    int m_playerWeapon; // The Player's index of its current Weapon
};

// Goblin Derived Class
class Goblin : public Actor
{
public:
    // Constructors and Destructors
    Goblin(Dungeon* d, int sr, int sc, int smellDistance);
    virtual ~Goblin();

    // Mutator Functions
    virtual bool readScroll(char c); // Does nothing; only Player can read Scrolls
    virtual bool wieldWeapon(char c); // Does nothing; only Player can wield Weapons
    virtual bool move(int drow, int dcol); // Determines the Goblin's move

    // Smell Algorithm Helper Functions
    bool pathExists(int maze[18][70], int sr, int sc, int er, int ec, int dist);
    int shortestPath(int maze[18][70], int er, int ec, int sr, int sc);
    void resetMaze(int maze[18][70]); // Resets the maze for convenient testing
    
private:
    Item* m_goblinWeapon; // The Goblin's Weapon
    int m_goblinSmellDistance; // The Goblin's smell distance
};

// Bogeyman Derived Class
class Bogeyman : public Actor
{
public:
    // Constructors and Destructors
    Bogeyman(Dungeon* d, int sr, int sc);
    virtual ~Bogeyman();

    // Mutator Functions
    virtual bool readScroll(char c); // Does nothing; only Player can read Scrolls
    virtual bool wieldWeapon(char c); // Does nothing; only Player can wield Weapons
    virtual bool move(int drow, int dcol); // Determines the Bogeyman's move
private:
    Item* m_bogeymanWeapon; // The Bogeyman's Weapon
};

// Snakewoman Derived Class
class Snakewoman : public Actor
{
public: 
    // Constructors and Destructors
    Snakewoman(Dungeon* d, int sr, int sc);
    virtual ~Snakewoman();

    // Mutator Functions
    virtual bool readScroll(char c); // Does nothing; only Player can read Scrolls
    virtual bool wieldWeapon(char c); // Does nothing; only Player can wield Weapons
    virtual bool move(int drow, int dcol); // Determines the Snakewoman's move
private:
    Item* m_snakewomanWeapon; // The Snakewoman's Weapon
};

// Dragon Derived Class
class Dragon : public Actor
{
public:
    //Constructors and Destructors
    Dragon(Dungeon* d, int sr, int sc);
    ~Dragon();

    // Mutator Functions
    virtual bool readScroll(char c); // Does nothing; only Player can read Scrolls
    virtual bool wieldWeapon(char c); // Does nothing; only Player can wield Weapons
    virtual bool move(int drow, int dcol); // Determines the Dragon's move
private:
    Item* m_dragonWeapon; // The Dragon's Weapon
};

#endif // ACTOR_INCLUDED