// Actor.cpp

#include "Actor.h"
#include "Dungeon.h"
#include "Item.h"
#include "utilities.h"
#include <algorithm>
#include <iostream>
#include <queue>

/////////////////////////////////////
// Actor Class Implementation
/////////////////////////////////////

// Actor constructor with parameters dungeon, owner and starting coordinates
Actor::Actor(Dungeon* d, int sr, int sc)
{
    // Set Actor's coordinates accordingly
    m_acol = sc;
    m_arow = sr;

    // Set Actor's initial statistics (to be modified in the derived class constructors)
    m_armorPoints = 0;
    m_dexterityPoints = 0;
    m_hitPoints = 0;
    m_maxHP = 0;
    m_sleepTime = 0;
    m_strengthPoints = 0;

    m_isWinner = false;
    m_isDead = false;
    m_dungeon = d;
}

// Actor destructor
Actor::~Actor()
{
    // Actor has nothing special to delete, therefore its destructor body is empty.
}

// Accessor: returns the Actor's row
int Actor::row() const
{
    return(m_arow);
}

// Accessor: returns the Actor's column
int Actor::col() const
{
    return(m_acol);
}

// Accessor: returns the Actor's armor points
int Actor::getArmor() const
{
    return(m_armorPoints);
}

// Accessor: returns the Actor's dexterity points
int Actor::getDexterity() const
{
    return(m_dexterityPoints);
}

// Accessor: returns the Actor's hit points
int Actor::getHP() const
{
    return(m_hitPoints);
}

// Accessor: returns the Actor's maximum hit points
int Actor::getMaxHP() const
{
    return(m_maxHP);
}

// Accessor: returns the Actor's sleep time
int Actor::getSleep() const
{
    return(m_sleepTime);
}

// Accessor: returns the Actor's strength points
int Actor::getStrength() const
{
    return(m_strengthPoints);
}

// Accessor: returns the Actor's weapon index
int Actor::getWeaponIndex() const
{
    return(0); // N.B. By design, only the Player can have an index other than zero
}

// Accessor: returns the Actor's dead status
bool Actor::isDead() const
{
    return(m_isDead);
}

// Accessor: returns the Actor's winner status
bool Actor::isWinner() const
{
    return(m_isWinner);
}

// Accessor: returns the Actor's dungeon pointer
Dungeon* Actor::getActorDungeon() const
{
    return(m_dungeon);
}

// Accessor: returns the Actor's type
std::string Actor::getActorName() const
{
    return(m_actorType);
}

// Accessor: displays the Actor's inventory
void Actor::dispInventory() const
{
    return; // N.B. Only the Player may display an inventory
}

// Mutator: changes the Actor's coordinates
void Actor::changeCoordinates(int deltaRow, int deltaCol)
{
    // N.B. By design, this function worries only about the coordinates themselves
    // For status checking, this responsibility is left to the higher code
    m_arow += deltaRow;
    m_acol += deltaCol;
}

// Mutator: changes the Actor's statistics
void Actor::changeStats(int modifier, std::string category)
{
    // Change the appropriate category's statistic
    if (category == "Max HP")
    {
        m_maxHP += modifier;

        // The maximum hit points level is 99
        if (m_maxHP > 99)
        {
            m_maxHP = 99;
        }
    }
    else if (category == "HP")
    {
        m_hitPoints += modifier;

        // The hit points level cannot exceed 99 nor the maximum hit points level
        if (m_hitPoints > 99)
        {
            m_hitPoints = 99;
        }
        else if (m_hitPoints > m_maxHP)
        {
            m_hitPoints = m_maxHP;
        }
    }
    else if (category == "Armor")
    {
        m_armorPoints += modifier;

        // The armor points level cannot exceed 99
        if (m_armorPoints > 99)
        {
            m_armorPoints = 99;
        }
    }
    else if (category == "Strength")
    {
        m_strengthPoints += modifier;

        // The strength points level cannot exceed 99
        if (m_strengthPoints > 99)
        {
            m_strengthPoints = 99;
        }
    }
    else if (category == "Dexterity")
    {
        m_dexterityPoints += modifier;

        // The dexterity points level cannot exceed 99
        if (m_dexterityPoints > 99)
        {
            m_dexterityPoints = 99;
        }
    }
    else if (category == "Sleep")
    {
        // This section is only used to decrement the existing sleep time.
        m_sleepTime += modifier;
    }
}

// Mutator: Actor picks up available Item
bool Actor::pickUp(int, int, bool)
{
    return(false); // N.B. By design, only the Player may pick up available Items
}

// Mutator: Actor drops appropriate spoils of war
void Actor::monsterDrop(std::string name)
{
    // N.B. By design, assume that the higher code calls appropriately
    if (m_actorType == "Player")
    {
        return; // While this should never trigger, it safeguards against a bad call.
    }

    bool dropChance; // Indicates whether a Monster drop occurs

    // N.B. By design, this function will not be called unless the tile will be empty
    if (name == "Goblin")
    {
        // Goblins have a 1/3 chance of dropping an item
        dropChance = trueWithProbability(1.0 / 3.0); 

        if (!dropChance)
        {
            return; // Unfortunately, the Goblin drops nothing
        }
        else
        {
            // Contains the probability that the Goblin drops a magic axe and not magic fangs
            bool isAxe = trueWithProbability(1.0 / 2.0);
            std::string dropType = "";

            if (isAxe)
            {
                dropType = "magic axe";
            }
            else
            {
                dropType = "magic fangs";
            }

            m_dungeon->addDrop(m_arow, m_acol, dropType, "Weapon");
        }
    }
    else if (name == "Snakewoman")
    {
        // Snakewomen have a 1/3 chance of dropping their magic fangs
        dropChance = trueWithProbability(1.0 / 3.0);

        if (!dropChance)
        {
            return; // Unfortunately, the Snakewoman drops nothing
        }
        else
        {
            std::string dropType = "magic fangs";
            m_dungeon->addDrop(m_arow, m_acol, dropType, "Weapon");
        }
    }
    else if (name == "Dragon")
    {
        dropChance = true; // Dragons are guaranteed to drop a Scroll of some kind
        std::string scrollTypes[5] = { "improve armor", "strength", "enhance health", 
            "enhance dexterity", "teleportation" };
        int index = randInt(0, 4); // Chooses a random Scroll to drop

        m_dungeon->addDrop(m_arow, m_acol, scrollTypes[index], "Scroll");
    }
    else if (name == "Bogeyman")
    {
        // Bogeymen have a 1/10 chance of dropping an item
        dropChance = trueWithProbability(1.0 / 10.0);

        if (!dropChance)
        {
            return; // Unfortunately, the Bogeyman drops nothing
        }
        else
        {
            std::string dropType = "magic axe";
            m_dungeon->addDrop(m_arow, m_acol, dropType, "Weapon");
        }
    }
}

// Mutator: the Actor attacks (i.e. the Monster attacks the Player)
bool Actor::attack(int r, int c)
{
    // N.B. By design, assume that the higher code calls appropriately
    
    // Retrieve the Player's coordinates
    int prow = m_dungeon->player()->row();
    int pcol = m_dungeon->player()->col();

    if (prow != r || pcol != c)
    {
        return(false); // While this should never trigger, it safeguards against a bad call
    }

    // Gather the attack parameters according to the Monster
    int weaponDexterity = 0;
    int weaponDamage = 0;
    std::string result = m_actorType;

    if (m_actorType == "Goblin" || m_actorType == "Bogeyman")
    {
        weaponDamage = 2;
        result += " slashes short sword at the ";
    }
    else if (m_actorType == "Dragon")
    {
        weaponDexterity = 2;
        weaponDamage = 4;
        result += " swings long sword at the ";
    }
    else // m_actorType == "Snakewoman"
    {
        weaponDexterity = 3;
        weaponDamage = 2;
        result += " strikes magic fangs at ";
    }

    result += "Player";

    // Determine whether the Actor hits or misses
    int attackerPoints = m_dexterityPoints + weaponDexterity;
    int defenderPoints = m_dungeon->player()->getDexterity() + m_dungeon->player()->getArmor();

    if (randInt(1, attackerPoints) < randInt(1, defenderPoints))
    {
        // Newline character is used in case multiple Monsters are attacking
        result += " and misses. \n"; 
        m_dungeon->setMonsterAttackString(result);
        return(false);  // The Actor missed, thus return immediately
    }
    else
    {
        // The Actor hits, thus determine the damage dealt
        int damagePoints = randInt(0, m_strengthPoints + weaponDamage - 1);
        m_dungeon->player()->changeStats(-damagePoints, "HP"); // Decrement Player's hit points

        // If the Player is dead, output the appropriate string
        if (m_dungeon->player()->getHP() <= 0)
        {
            result += ", dealing a final blow. \n";
            m_dungeon->player()->setDead(); // Sets Player to be dead
            m_dungeon->setMonsterAttackString(result);
            return(true); // The Actor hit and the Player is dead, thus return immediately
        }

        // If the Actor is a Snakewoman, check whether it puts the Player to sleep
        if (m_actorType == "Snakewoman")
        {
            // Magic Fangs of Sleep have a 1/5 chance of putting the defender to sleep
            bool setSleep = trueWithProbability(1.0 / 5.0);

            if (setSleep)
            {
                result += " and hits, putting the Player to sleep.";
                m_dungeon->player()->setSleep(randInt(2, 6));
            }
            else
            {
                result += " and hits. \n";
            }

            // Set the appropriate output string, and return immediately
            m_dungeon->setMonsterAttackString(result);
            return(true);
        }

        result += " and hits. \n";
        m_dungeon->setMonsterAttackString(result);
        return(true);
    }
}

// Mutator: sets the Actor's armor points
void Actor::setArmor(int armor)
{
    m_armorPoints = armor;
}

// Mutator: sets the Actor's dexterity points
void Actor::setDexterity(int dexterity)
{
    m_dexterityPoints = dexterity;
}

// Mutator: sets the Actor's hit points
void Actor::setHP(int hp)
{
    m_hitPoints = hp;
}

// Mutator: sets the Actor's maximum hit points
void Actor::setMaxHP(int maxHP)
{
    m_maxHP = maxHP;
}

// Mutator: sets the Actor's sleep time
void Actor::setSleep(int time)
{
    m_sleepTime = std::max(m_sleepTime, time);
}

// Mutator: sets the Actor's strength points
void Actor::setStrength(int strength)
{
    m_strengthPoints = strength;
}

// Mutator: sets the Actor's type
void Actor::setActorType(std::string name)
{
    m_actorType = name;
}

// Mutator: sets the Actor to be dead
void Actor::setDead()
{
    if (m_actorType == "Player")
    {
        m_isDead = true;
        return;
    }
    
    // Retrieve the index of the Monster that is killed
    int correctIndex = m_dungeon->actorAtPos(m_arow, m_acol);
    int r = m_dungeon->getMonsters()[correctIndex]->row();
    int c = m_dungeon->getMonsters()[correctIndex]->col();

    // Adjust the cell status accordingly
    if (m_dungeon->getStatus(r, c) == ACTOR)
    {
        m_dungeon->setStatus(r, c, EMPTY);
        std::string monsterName = m_dungeon->getMonsters()[correctIndex]->getActorName();
        monsterDrop(monsterName); // Sets the monster drop accordingly, if there is one
    }
    else // m_dungeon->getStatus(r, c) == ACTORITEM
    {
        m_dungeon->setStatus(r, c, ITEM);
    }

    m_dungeon->removeMonster(correctIndex); // Remove the Monster from the Dungeon
}

// Mutator: sets the Player as the winner
void Actor::setWinner()
{
    m_isWinner = true; // N.B. This is only called on the Player, as a Monster cannot win
}

/////////////////////////////////////
// Player Class Implementation
/////////////////////////////////////

// Player constructor with parameters dungeon and starting coordinates
Player::Player(Dungeon* d, int sr, int sc)
    : Actor(d, sr, sc)
{
    // Set Player's statistics accordingly
    setArmor(2);
    setDexterity(2);
    setHP(20); 
    setMaxHP(20); 
    setStrength(2);
    setActorType("Player");
    
    // Initially, the Player begins with a short sword
    Item* playerWeapon = new Weapon(getActorDungeon(), this, "short sword", -1, -1);
    m_inventory.push_back(playerWeapon);
    m_playerWeapon = 0; // Short sword is initially the first item in the inventory
}

// Player destructor
Player::~Player()
{
    // Delete the Player's inventory
    for (size_t i = 0; i < m_inventory.size(); i++)
    {
        delete m_inventory[i]; // Call delete on each inventory object
    }
}

// Accessor: displays the Player's inventory
void Player::dispInventory() const
{
    // As the function is constant, iterate through playerInventory variable instead
    std::vector<Item*> playerInventory = m_inventory;
    std::vector<Item*>::iterator it = playerInventory.begin();
    char front = 'a'; // Set our display prefix character
    std::cout << "Inventory: " << std::endl;

    // Loop through the inventory and print out the items
    for (it = playerInventory.begin(); it < playerInventory.end(); it++)
    {
        std::string itemname = (*it)->getName(); // Retrieve the Item's name

        // If Item is a Scroll, adjust the output string appropriately
        if (itemname == "teleportation" || itemname == "enhance dexterity" ||
            itemname == "enhance health" || itemname == "strength" ||
            itemname == "improve armor")
        {
            itemname = "A scroll called scroll of " + itemname;
        }

        std::cout << front << ". " << itemname << std::endl;
        front++; // Increment our display prefix character
    }
}

// Accessor: returns the Player's wielded Weapon index
int Player::getWeaponIndex() const
{
    return(m_playerWeapon);
}

// Mutator: the Player attacks [the Monster]
bool Player::attack(int r, int c)
{
    // Retrieve the Monster at the intended coordinates
    std::vector<Actor*> targetMonster = getActorDungeon()->getMonsters();
    int index = getActorDungeon()->actorAtPos(r, c);

    // N.B. Assume the higher code is calling this function appropriately
    if (index == -1)
    {
        // We have somehow attacked a cell which displayed a Monster, but no Monster was present
        return(false); // While this should never trigger, it safeguards against bad calls
    }

    // Determine the attack parameters and Monster accordingly
    Actor* playerTarget = targetMonster[index];
    std::string monsterName = playerTarget->getActorName();
    int attackerPoints = getDexterity() + m_inventory[m_playerWeapon]->getWeaponDexterity();
    int defenderPoints = playerTarget->getDexterity() + playerTarget->getArmor();

    // Retrieve the Player's currently wielded Weapon
    std::string weaponName = m_inventory[m_playerWeapon]->getName();
    std::string result = "Player ";

    // Adjust the result according to the currently wielded Weapon
    if (weaponName == "mace")
    {
        result += "swings mace at the ";
    }
    else if (weaponName == "short sword")
    {
        result += "slashes short sword at the ";
    }
    else if (weaponName == "long sword")
    {
        result += "swings long sword at the ";
    }
    else if (weaponName == "magic axe")
    {
        result += "chops magic axe at ";
    }
    else // weaponName == "magic fangs"
    {
        result += "strikes magic fangs at ";
    }

    result += monsterName;

    // Determine whether the Player hits or misses
    if (randInt(1, attackerPoints) < randInt(1, defenderPoints))
    {
        result += " and misses.";
        getActorDungeon()->setPlayerAttackString(result);
        return(false); // The Player missed, thus return immediately
    }
    else
    {
        // The Player hits, thus determine the damage dealth
        int damagePoints = randInt(0, getStrength() + 
            m_inventory[m_playerWeapon]->getWeaponDamage() - 1);
        playerTarget->changeStats(-damagePoints, "HP"); // Decrement Monster's hit points
        
        // If the Monster is dead, output the appropriate string
        if (playerTarget->getHP() <= 0)
        {
            result += ", dealing a final blow.";
            playerTarget->setDead(); // Sets Monster to be dead
            getActorDungeon()->setPlayerAttackString(result);
            return(true); // The Player hit and the Monster is dead, thus return immediately
        }

        // If the wielded Weapon is a magic fangs, check if it puts the Monster to sleep
        if (weaponName == "magic fangs")
        {
            // Magic Fangs of Sleep have a 1/5 chance of putting the defender to sleep
            bool isAsleep = trueWithProbability(1.0 / 5.0);

            if (isAsleep)
            {
                result += " and hits, putting the " + monsterName + " to sleep.";
                playerTarget->setSleep(randInt(2, 6));
            }
            else
            {
                result += " and hits.";
            }

            // Output the appropriate result string
            getActorDungeon()->setPlayerAttackString(result);
            return(true);
        }

        result += " and hits.";
        getActorDungeon()->setPlayerAttackString(result);
        return(true);
    }
}

// Mutator: the Player moves
bool Player::move(int drow, int dcol)
{
    // N.B. Assume that the higher code is calling this function appropriately
    
    // Reset current cell status
    if (getActorDungeon()->getStatus(row(), col()) == ACTOR)
    {
        getActorDungeon()->setStatus(row(), col(), EMPTY);
    }
    else if (getActorDungeon()->getStatus(row(), col()) == ACTORITEM)
    {
        getActorDungeon()->setStatus(row(), col(), ITEM);
    }
    
    changeCoordinates(drow, dcol); // Change the Player's coordinates

    // Set new cell status
    if (getActorDungeon()->getStatus(row(), col()) == EMPTY)
    {
        getActorDungeon()->setStatus(row(), col(), ACTOR);
    }
    else
    {
        getActorDungeon()->setStatus(row(), col(), ACTORITEM);
    }

    return(true);
}

// Mutator: Player picks up Item
bool Player::pickUp(int r, int c, bool gflag)
{
    // N.B. Assume that the higher code is calling this function appropriately
    // N.B. gflag indicates whether this function was called with 'g' or '>'
    
    int inventorySize = m_inventory.size(); // Retrieve the size of the Player's inventory

    if (getActorDungeon()->getStatus(r, c) == ACTORITEM && gflag && (inventorySize <= 25))
    {
        std::string result = "You pick up ";
        int correctIndex = getActorDungeon()->itemAtPos(r, c); // Retrieve the Idem's index

        // Obtain the Item's parameters (i.e. name, type, etc.)
        Item* myItem = getActorDungeon()->getAvailItems()[correctIndex];
        std::string itemName = myItem->getName();
        std::string itemClass = myItem->getType();

        if (myItem->getName() == "Stairway")
        {
            return(false); // Do nothing, because you must press '>'
        }
        else if (myItem->getName() == "Golden Idol")
        {
            result += "the golden idol.";
            getActorDungeon()->setPlayerPickUpString(result);
            setWinner(); // Set the Player as the winner
            return(true);
        }

        // Modify the output string depending on the Item type
        if (itemClass == "Weapon")
        {
            // Adjust the name for magic fangs of sleep
            if (itemName == "magic fangs")
            {
                itemName = "magic fangs of sleep";
            }
            
            result += "a " + itemName + ".";
        }
        else
        {
            result += "a scroll called scroll of " + itemName + ".";
        }

        getActorDungeon()->setPlayerPickUpString(result);

        // Transfer the Item from the Dungeon to the Player
        myItem->setOwner(this);
        m_inventory.push_back(myItem);
        getActorDungeon()->removeItem(correctIndex);
        getActorDungeon()->setStatus(r, c, ACTOR);

        return(true);
    }
    else if (getActorDungeon()->getStatus(r, c) == ACTORITEM && !gflag)
    {
        // In this scenario, only the Stairway would be a valid Item
        int correctIndex = getActorDungeon()->itemAtPos(r, c);
        Item* myItem = getActorDungeon()->getAvailItems()[correctIndex];

        if (myItem->getName() == "Stairway")
        {
            // Generate a new level
            int currlevel = getActorDungeon()->getLevel();
            getActorDungeon()->setStatus(r, c, ACTOR);
            getActorDungeon()->newLevel(currlevel + 1);
            return(true);
        }

        return(false);
    }
    else
    {
        int correctIndex = getActorDungeon()->itemAtPos(r, c); // Obtain the Item's index

        if (correctIndex == -1)
        {
            return(false); // While this should never trigger, it safeguards against bad calls
        }

        std::string name = getActorDungeon()->getAvailItems()[correctIndex]->getName();

        // Player can pick up the golden idol, even with a full inventory
        std::string result = "";

        if (name == "Golden Idol")
        {
            result = "You pick up the golden idol.";
            getActorDungeon()->setPlayerPickUpString(result);
            setWinner(); // Set Player as winner
            return(true);
        }

        result = "Your knapsack is full; you can't pick that up.";
        getActorDungeon()->setPlayerPickUpString(result);
        return(false);
    }
}

// Mutator: Player reads the Scroll
bool Player::readScroll(char c)
{
    int sizeChar = c - 97; // ASCII conversion from input character to integral value
    int size = m_inventory.size(); // Obtains the size of the Player's inventory

    if (sizeChar < 0 || sizeChar >= 26)
    {
        return(false); // We have an invalid character input, thus return immediately
    }
    else if (sizeChar >= size)
    {
        return(false); // Our character input is past the last Item, thus return immediately
    }
    else
    {
        if (m_inventory[sizeChar]->getType() == "Scroll")
        {
            m_inventory[sizeChar]->wieldItem(); // Player reads the selected Scroll

            std::string result = "You read the scroll called scroll of ";
            std::string scrollName = m_inventory[sizeChar]->getName();
            result += scrollName;

            // Set the output string depending on the Scroll type
            if (scrollName == "teleportation")
            {
                result += ". \nYou feel your body wrenched in space and time.";
            }
            else if (scrollName == "strength")
            {
                result += ". \nYour muscles bulge.";
            }
            else if (scrollName == "improve armor")
            {
                result += ". \nYour armor glows blue.";
            }
            else if (scrollName == "enhance health")
            {
                result += ". \nYou feel your heart beating stronger.";
            }
            else // scrollName == "enhance dexterity"
            {
                result += ". \nYou feel like less of a klutz.";
            }

            getActorDungeon()->setPlayerScrollString(result); // Set the output string

            // Delete the Scroll, as it is expended upon usage
            std::vector<Item*>::iterator it = m_inventory.begin() + sizeChar;
            delete (*it);
            it = m_inventory.erase(it);
            return(true);
        }
        else
        {
            std::string itemName = m_inventory[sizeChar]->getName();

            if (itemName == "magic fangs")
            {
                itemName = "magic fangs of sleep";
            }

            // Player cannot read a Weapon
            std::string result = "You can't read a " + itemName + ".";
            getActorDungeon()->setPlayerScrollString(result);
        }
        return(false);
    }
}

// Mutator: Player wields the Weapon
bool Player::wieldWeapon(char c)
{
    int sizeChar = c - 97; // ASCII conversion from input character to integral value
    int size = m_inventory.size(); // Obtains the size of the Player's inventory

    if (sizeChar < 0 || sizeChar >= 26)
    {
        return(false); // We have an invalid character input, thus return immediately
    }
    else if (sizeChar >= size)
    {
        return(false); // Our character input is past the last Item, thus return immediately
    }
    else
    {
        if (m_inventory[sizeChar]->getType() == "Weapon")
        {
            // Set the output string depending on the chosen Weapon
            std::string result = "You are wielding a ";
            std::string weaponName = m_inventory[sizeChar]->getName();

            if (weaponName == "magic fangs")
            {
                weaponName = "magic fangs of sleep";
            }

            result += weaponName + ".";
            getActorDungeon()->setPlayerWieldString(result);

            m_playerWeapon = sizeChar; // Change Player's index of the wielded Weapon
            return(true);
        }
        else
        {
            // Player cannot wield a Scroll
            std::string scrollName = m_inventory[sizeChar]->getName();
            std::string result = "You can't wield a scroll of " + scrollName + ".";
            getActorDungeon()->setPlayerWieldString(result);
        }
        return(false);
    }
}

/////////////////////////////////////
// Goblin Class Implementation
/////////////////////////////////////

// Goblin constructor with parameters dungeon, starting coordinates and smell distance
Goblin::Goblin(Dungeon* d, int sr, int sc, int smellDistance)
    : Actor(d, sr, sc)
{
    // Set Goblin's statistics accordingly
    int hp = randInt(15, 20); // Goblin begins with random hit points from 15-20
    setArmor(1);
    setDexterity(1);
    setHP(hp);
    setMaxHP(hp);
    setStrength(3);
    setActorType("Goblin");
    
    // All Goblins carry the short sword
    m_goblinWeapon = new Weapon(getActorDungeon(), this, "short sword", -1, -1);
    m_goblinSmellDistance = smellDistance; // Set the Goblin's smell distance
}

// Goblin destructor
Goblin::~Goblin()
{
    delete m_goblinWeapon; // Delete the Goblin's Weapon
}

// Mutator: Goblin reads Scroll (does nothing)
bool Goblin::readScroll(char)
{
    return(false);
}

// Mutator: Goblin wields Weapon (does nothing)
bool Goblin::wieldWeapon(char)
{
    return(false);
}

// Mutator: the Goblin moves 
bool Goblin::move(int drow, int dcol)
{
    // N.B. Unlike the Player move function, drow and dcol represent goblin's coordinates

    // Obtain the Player's coordinates
    int prow = getActorDungeon()->player()->row();
    int pcol = getActorDungeon()->player()->col();

    // Calculate the Manhattan Distance
    int manhattanDist = abs(prow - drow) + abs(pcol - dcol);

    // N.B. If Player isn't within the Manhattan Distance, then it is never reachable
    if (manhattanDist > m_goblinSmellDistance)
    {
        return(false); // Goblin does nothing
    }
    else if (manhattanDist == 1)
    {
        attack(prow, pcol); // Player is directly adjacent, thus attack the Player
    }
    else
    {
        // We initialize our movement parameters
        int deltaRow = 0; // Distance to move vertically
        int deltaCol = 0; // Distance to move horizontally

        bool N, S, E, W; // Booleans indicate whether directional tiles are traversable 
        N = true; S = true; E = true; W = true; // Initially, assume all paths are available

        // Indicates distance in each directiion
        int northSteps = 1000;
        int southSteps = 1000; 
        int eastSteps = 1000; 
        int westSteps = 1000;

        // Retrieve directional cells' status
        if (getActorDungeon()->getStatus(drow - 1, dcol) != EMPTY &&
            getActorDungeon()->getStatus(drow - 1, dcol) != ITEM)
        {
            N = false;
            northSteps = m_goblinSmellDistance + 1; // If not traversable, set invalid distance
        }
        if (getActorDungeon()->getStatus(drow + 1, dcol) != EMPTY &&
            getActorDungeon()->getStatus(drow + 1, dcol) != ITEM)
        {
            S = false;
            southSteps = m_goblinSmellDistance + 1; // If not traversable, set invalid distance
        }
        if (getActorDungeon()->getStatus(drow, dcol + 1) != EMPTY &&
            getActorDungeon()->getStatus(drow, dcol + 1) != ITEM)
        {
            E = false;
            eastSteps = m_goblinSmellDistance + 1; // If not traversable, set invalid distance
        }
        if (getActorDungeon()->getStatus(drow, dcol - 1) != EMPTY &&
            getActorDungeon()->getStatus(drow, dcol - 1) != ITEM)
        {
            W = false;
            westSteps = m_goblinSmellDistance + 1; // If not traversable, set invalid distance
        }

        // Initialize a grid to test for grid distance
        int maze[18][70];

        // Initialize our test maze
        for (int r = 0; r < getActorDungeon()->getRow(); r++)
        {
            for (int c = 0; c < getActorDungeon()->getCol(); c++)
            {
                if (getActorDungeon()->getStatus(r, c) != EMPTY && 
                    getActorDungeon()->getStatus(r, c) != ITEM)
                {
                    maze[r][c] = 1000; // This indicates that the cell is not traversable
                }
                else
                {
                    maze[r][c] = 0;
                }
            }
        }
        maze[prow][pcol] = 0; // Don't forget to set the target cell as reachable
        maze[drow][dcol] = 0; // For completeness, set the monster's current cell as reachable too
        int tracker = 1; // Marks the level of each visited cell

        if (pathExists(maze, drow, dcol, prow, pcol, tracker) && 
            shortestPath(maze, prow, pcol, drow, dcol) <= m_goblinSmellDistance)
        {
            // Calculate North steps
            if (N)
            {
                resetMaze(maze);
                if (pathExists(maze, drow - 1, dcol, prow, pcol, tracker))
                    northSteps = shortestPath(maze, prow, pcol, drow - 1, dcol);
            }

            // Calculate South steps
            if (S)
            {
                resetMaze(maze);
                if (pathExists(maze, drow + 1, dcol, prow, pcol, tracker))
                    southSteps = shortestPath(maze, prow, pcol, drow + 1, dcol);
            }

            // Calculate East steps
            if (E)
            {
                resetMaze(maze);
                if (pathExists(maze, drow, dcol + 1, prow, pcol, tracker))
                    eastSteps = shortestPath(maze, prow, pcol, drow, dcol + 1);
            }

            // Calculate West steps
            if (W)
            {
                resetMaze(maze);
                if (pathExists(maze, drow, dcol - 1, prow, pcol, tracker))
                    westSteps = shortestPath(maze, prow, pcol, drow, dcol - 1);
            }

            // Choose direction with the minimum distance
            if (N && northSteps <= southSteps && northSteps <= eastSteps && northSteps <= westSteps)
            {
                deltaRow = -1;
            }
            else if (S && southSteps <= northSteps && southSteps <= eastSteps && southSteps <= westSteps)
            {
                deltaRow = 1;
            }
            else if (W && westSteps <= northSteps && westSteps <= eastSteps && westSteps <= southSteps)
            {
                deltaCol = -1;
            }
            else if (E && eastSteps <= northSteps && eastSteps <= westSteps && eastSteps <= southSteps)
            {
                deltaCol = 1;
            }
            
            // Move the Goblin appropriately
            if (getActorDungeon()->getStatus(row(), col()) == ACTOR)
            {
                getActorDungeon()->setStatus(row(), col(), EMPTY);
            }
            else // getActorDungeon()->getStatus(row(), col()) == ACTORITEM
            {
                getActorDungeon()->setStatus(row(), col(), ITEM);
            }
            
            changeCoordinates(deltaRow, deltaCol);

            if (getActorDungeon()->getStatus(row(), col()) == EMPTY)
            {
                getActorDungeon()->setStatus(row(), col(), ACTOR);
            }
            else // getActorDungeon()->getStatus(row(), col()) == ITEM
            {
                getActorDungeon()->setStatus(row(), col(), ACTORITEM);
            }
        }
        else
        {
            return(false); // Player is either unreachable or not within smell distance
        }
    }

    return(false);
}

// Smell Algorithm Helper: determines whether Goblin can reach the Player
// N.B. This is almost an exact copy of my Homework 2, Question 3 solution
bool Goblin::pathExists(int maze[18][70], int sr, int sc, int er, int ec, int dist)
{
    bool solvable = false; // Indicates whether a valid solution exists

    // Declare two queues to keep track of the maze coordinates
    std::queue<int> rows;
    std::queue<int> columns;

    // Push the current coordinates onto the queue
    rows.push(sr);
    columns.push(sc);

    maze[sr][sc] = dist; // Mark the first cell as visited

    while (rows.size() != 0)
    {
        dist++; // Increment the distance

        // Retrieve the front coordinate
        int currentRow = rows.front();
        int currentCol = columns.front();

        // Pop off the current coordinates
        rows.pop();
        columns.pop();

        // If the current coordinate is the end coordinate, return true
        if (currentRow == er && currentCol == ec)
        {
            solvable = true;
        }

        // If south is unique, push the coordinates onto the queues and mark as encountered
        if (maze[currentRow + 1][currentCol] == 0)
        {
            rows.push(currentRow + 1);
            columns.push(currentCol);
            maze[currentRow + 1][currentCol] = dist;
        }

        // If west is unique, push the coordinates onto the queues and mark as encountered
        if (maze[currentRow][currentCol - 1] == 0)
        {
            rows.push(currentRow);
            columns.push(currentCol - 1);
            maze[currentRow][currentCol - 1] = dist;
        }

        // If north is unique, push the coordinates onto the queues and mark as encountered
        if (maze[currentRow - 1][currentCol] == 0)
        {
            rows.push(currentRow - 1);
            columns.push(currentCol);
            maze[currentRow - 1][currentCol] = dist;
        }

        // If east is unique, push the coordinates onto the queues and mark as encountered
        if (maze[currentRow][currentCol + 1] == 0)
        {
            rows.push(currentRow);
            columns.push(currentCol + 1);
            maze[currentRow][currentCol + 1] = dist;
        }
    }

    return(solvable);
}

// Smell Algorithm Helper: recursively determines the shortest path distance
int Goblin::shortestPath(int maze[18][70], int er, int ec, int sr, int sc)
{
    // Work from the target cell back to the starting cell, moving onto the tiles
    // that contain the smallest value. In the pathExists() function, if a path
    // exists, the use of a queue will mark all the adjacent cells with the same 
    // distance level, meaning that when we work backwards in this function, 
    // choosing the cells with the lowest value will guarantee that we are
    // traversing along the shortest grid path. The only catches: the function
    // must be called correctly, and a path must exist, otherwise this will throw 
    // up an infinite recursion.

    int distance = 0;

    if (er != sr || ec != sc)
    {
        int N, S, E, W; // Retrieve the status of the cell in the cardinals direction

        // Booleans indicate whether or not we can move in a given direction
        bool N1 = true;
        bool S1 = true;
        bool E1 = true;
        bool W1 = true;

        // Retrieve the status of the cardinal directional cells
        N = maze[er - 1][ec];
        S = maze[er + 1][ec];
        E = maze[er][ec + 1];
        W = maze[er][ec - 1];

        // If the cell is not traversable (i.e. ACTOR, ACTORITEM, or WALL), we set the 
        // directional boolean to be false. N.B. Walls are represented by 1000.
        if (N == 1000)
        {
            N1 = false;
        }
        if (S == 1000)
        {
            S1 = false;
        }
        if (E == 1000)
        {
            E1 = false;
        }
        if (W == 1000)
        {
            W1 = false;
        }

        // Move onto the cell with the minimum value (i.e. shortest distance to player)
        if (N1 && N <= S && N <= E && N <= W)
        {
            er--;
        }
        else if (S1 && S <= N && S <= E && S <= W)
        {
            er++;
        }
        else if (W1 && W <= N && W <= E && W <= S)
        {
            ec--;
        }
        else if (E1 && E <= N && E <= W && E <= S)
        {
            ec++;
        }
        
        distance++; // Increment the distance

        distance += shortestPath(maze, er, ec, sr, sc); // Recursively determine shortest path
    }

    return(distance);
}

// Smell Algorithm Helper: resets maze for further testing
void Goblin::resetMaze(int maze[18][70])
{
    for (int r = 0; r < 18; r++)
    {
        for (int c = 0; c < 70; c++)
        {
            if (maze[r][c] != 1000)
            {
                maze[r][c] = 0;
            }
        }
    }
}

/////////////////////////////////////
// Bogeyman Class Implementation
/////////////////////////////////////

// Bogeyman constructor with parameters dungeon and starting coordinates
Bogeyman::Bogeyman(Dungeon* d, int sr, int sc)
    : Actor(d, sr, sc)
{
    // Set Bogeyman's statistics accordingly
    int hp = randInt(5, 10); // Goblin begins with random hit points from 5-10
    setArmor(2);
    setDexterity(randInt(2, 3));
    setHP(hp);
    setMaxHP(hp);
    setStrength(randInt(2, 3));
    setActorType("Bogeyman");
    
    // All Bogeymen carry the short sword
    m_bogeymanWeapon = new Weapon(getActorDungeon(), this, "short sword", -1, -1);
}

// Bogeyman destructor
Bogeyman::~Bogeyman()
{
    delete m_bogeymanWeapon; // Delete the Bogeyman's Weapon
}

// Mutator: Bogeyman reads Scroll (does nothing)
bool Bogeyman::readScroll(char)
{
    return(false);
}

// Mutator: Bogeyman wields Weapon (does nothing)
bool Bogeyman::wieldWeapon(char)
{
    return(false);
}

// Mutator: the Bogeyman moves
bool Bogeyman::move(int drow, int dcol)
{
    // N.B. Unlike the player move, drow and dcol represent Bogeyman's coordinates
    
    // Obtain the Player's coordinates
    int prow = getActorDungeon()->player()->row();
    int pcol = getActorDungeon()->player()->col();

    // Calculate the Manhattan Distance
    int manhattanDist = abs(prow - drow) + abs(pcol - dcol);

    // If Player isn't within the Manhattan Distance, then it is never reachable
    if (manhattanDist > 5)
    {
        return(false);
    }
    else if (manhattanDist == 1)
    {
        attack(prow, pcol); // Player is directly adjacent, thus attack the Player
    }
    else
    {
        // Retrieve the Manhattan coordinate distances
        int manhattanRow = prow - drow;
        int manhattanCol = pcol - dcol;

        // Retrieve the current and neighboring cell statuses
        int currStatus = getActorDungeon()->getStatus(drow, dcol);
        int nStatus = getActorDungeon()->getStatus(drow - 1, dcol);
        int sStatus = getActorDungeon()->getStatus(drow + 1, dcol);
        int eStatus = getActorDungeon()->getStatus(drow, dcol + 1);
        int wStatus = getActorDungeon()->getStatus(drow, dcol - 1);

        // Specify the move cirection
        int deltaRow = 0; int deltaCol = 0;

        // Bogeyman moves North
        if (std::abs(prow - (drow - 1)) < std::abs(manhattanRow)
            && (nStatus == ITEM || nStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (nStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow - 1, dcol, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow - 1, dcol, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaRow = -1;
            changeCoordinates(deltaRow, deltaCol); // Change Bogeyman's coordinates
            return(true);
        }

        // Bogeyman moves South
        if (std::abs(prow - (drow + 1)) < std::abs(manhattanRow)
            && (sStatus == ITEM || sStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (sStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow + 1, dcol, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow + 1, dcol, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaRow = 1;
            changeCoordinates(deltaRow, deltaCol); // Change Bogeyman's coordinates
            return(true);
        }

        // Bogeyman moves East
        if (std::abs(pcol - (dcol + 1)) < std::abs(manhattanCol)
            && (eStatus == ITEM || eStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (eStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow, dcol + 1, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol + 1, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaCol = 1;
            changeCoordinates(deltaRow, deltaCol); // Change Bogeyman's coordinates
            return(true);
        }

        // Bogeyman moves West
        if (std::abs(pcol - (dcol - 1)) < std::abs(manhattanCol)
            && (wStatus == ITEM || wStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (wStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow, dcol - 1, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol - 1, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaCol = -1;
            changeCoordinates(deltaRow, deltaCol); // Change Bogeyman's coordinates
            return(true);
        }
    }

    return(false);
}

/////////////////////////////////////
// Snakewoman Class Implementation
/////////////////////////////////////

// Snakewoman constructor with parameters dungeon and starting coordinates
Snakewoman::Snakewoman(Dungeon* d, int sr, int sc)
    : Actor(d, sr, sc)
{
    // Set Snakewoman's statistics accordingly
    int hp = randInt(3, 6); // Snakewoman begins with hit points from 3-6
    setArmor(3);
    setDexterity(3);
    setHP(hp);
    setMaxHP(hp);
    setStrength(2);
    setActorType("Snakewoman");

    // All Snakewomen carry the magic fangs of sleep
    m_snakewomanWeapon = new Weapon(getActorDungeon(), this, "magic fangs", -1, -1);
}

// Snakewoman destructor
Snakewoman::~Snakewoman()
{
    delete m_snakewomanWeapon; // Delete the Snakewoman's Weapon
}

// Mutator: Snakewoman reads Scroll (does nothing)
bool Snakewoman::readScroll(char)
{
    return(false);
}

// Mutator: Snakewoman wields Weapon (does nothing)
bool Snakewoman::wieldWeapon(char)
{
    return(false);
}

// Mutator: the Snakewoman moves
bool Snakewoman::move(int drow, int dcol)
{
    // N.B. Unlike the player move, drow and dcol represent Snakewoman's coordinates

    // Obtain the Player's coordinates
    int prow = getActorDungeon()->player()->row();
    int pcol = getActorDungeon()->player()->col();

    // Calculate the Manhattan Distance
    int manhattanDist = abs(prow - drow) + abs(pcol - dcol);

    // If Player isn't within the Manhattan Distance, then it is never reachable
    if (manhattanDist > 3)
    {
        return(false);
    }
    else if (manhattanDist == 1)
    {
        attack(prow, pcol); // Player is directly adjacent, thus attack the Player
    }
    else
    {
        // Retrieve the Manhattan coordinate distances
        int manhattanRow = prow - drow;
        int manhattanCol = pcol - dcol;

        // Retrieve the current and neighboring cell statuses
        int currStatus = getActorDungeon()->getStatus(drow, dcol);
        int nStatus = getActorDungeon()->getStatus(drow - 1, dcol);
        int sStatus = getActorDungeon()->getStatus(drow + 1, dcol);
        int eStatus = getActorDungeon()->getStatus(drow, dcol + 1);
        int wStatus = getActorDungeon()->getStatus(drow, dcol - 1);

        // Specify the move cirection
        int deltaRow = 0; int deltaCol = 0;

        // Snakewoman moves North
        if (std::abs(prow - (drow - 1)) < std::abs(manhattanRow)
            && (nStatus == ITEM || nStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (nStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow - 1, dcol, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow - 1, dcol, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaRow = -1;
            changeCoordinates(deltaRow, deltaCol); // Change Snakewoman's coordinates
            return(true);
        }

        // Snakewoman moves South
        if (std::abs(prow - (drow + 1)) < std::abs(manhattanRow)
            && (sStatus == ITEM || sStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (sStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow + 1, dcol, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow + 1, dcol, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaRow = 1;
            changeCoordinates(deltaRow, deltaCol); // Change Snakewoman's coordinates
            return(true);
        }

        // Snakewoman moves East
        if (std::abs(pcol - (dcol + 1)) < std::abs(manhattanCol)
            && (eStatus == ITEM || eStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (eStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow, dcol + 1, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol + 1, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaCol = 1;
            changeCoordinates(deltaRow, deltaCol); // Change Snakewoman's coordinates
            return(true);
        }

        // Snakewoman moves West
        if (std::abs(pcol - (dcol - 1)) < std::abs(manhattanCol)
            && (wStatus == ITEM || wStatus == EMPTY))
        {
            // Set the status of the new cell accordingly
            if (wStatus == ITEM)
            {
                getActorDungeon()->setStatus(drow, dcol - 1, ACTORITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol - 1, ACTOR);
            }

            // Reset the status of the current cell accordingly
            if (currStatus == ACTORITEM)
            {
                getActorDungeon()->setStatus(drow, dcol, ITEM);
            }
            else
            {
                getActorDungeon()->setStatus(drow, dcol, EMPTY);
            }

            deltaCol = -1;
            changeCoordinates(deltaRow, deltaCol); // Change Snakewoman's coordinates
            return(true);
        }
    }

    return(false);
}

/////////////////////////////////////
// Dragon Class Implementation
/////////////////////////////////////

// Dragon constructor with parameters dungeon and starting coordinates
Dragon::Dragon(Dungeon* d, int sr, int sc)
    : Actor(d, sr, sc)
{
    int hp = randInt(20, 25); // Dragon begins with hit points from 20-25
    setArmor(4);
    setDexterity(4);
    setHP(hp);
    setMaxHP(hp);
    setStrength(4);
    setActorType("Dragon");

    // All Dragons carry the long sword
    m_dragonWeapon = new Weapon(getActorDungeon(), this, "long sword", -1, -1);
}

// Dragon destructor
Dragon::~Dragon()
{
    delete m_dragonWeapon; // Delete the Dragon's Weapon
}

// Mutator: Dragon reads Scroll (does nothing)
bool Dragon::readScroll(char)
{
    return(false);
}

// Mutator: Dragon wields Weapon (does nothing)
bool Dragon::wieldWeapon(char)
{
    return(false);
}

// Mutator: the Dragon moves
bool Dragon::move(int drow, int dcol)
{
    // Obtain the Player's coordinates
    int prow = getActorDungeon()->player()->row();
    int pcol = getActorDungeon()->player()->col();

    // Retrieve the Manhattan coordinate distances
    int manhattanRow = abs(prow - drow);
    int manhattanCol = abs(pcol - dcol);

    if (manhattanCol + manhattanRow == 1)
    {
        attack(prow, pcol); // Player is directly adjacent, thus attack the Player
    }

    return(false);
}