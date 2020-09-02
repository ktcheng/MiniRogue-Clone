// Game.cpp

#include "Actor.h"
#include "Dungeon.h"
#include "Game.h"
#include "Item.h"
#include "utilities.h"
#include <iostream>

// Game constructor with parameter Goblin smell distance
Game::Game(int goblinSmellDistance)
{
    // Create the dungeon
    m_dungeon = new Dungeon(0, goblinSmellDistance);

    // Retrieve valid coordinates to add the player
    int initrow = randInt(1, m_dungeon->getRow() - 2);
    int initcol = randInt(1, m_dungeon->getCol() - 2);
    
    while (m_dungeon->getStatus(initrow, initcol) != EMPTY &&
        m_dungeon->getStatus(initrow, initcol) != ITEM)
    {
        // If the coordinates are invalid, keep resetting until valid
        initrow = randInt(1, m_dungeon->getRow() - 2);
        initcol = randInt(1, m_dungeon->getCol() - 2);
    }

    m_dungeon->addPlayer(initrow, initcol);

    // Set the status of the player cell accordingly
    if (m_dungeon->getStatus(initrow, initcol) == EMPTY)
    {
        m_dungeon->setStatus(initrow, initcol, ACTOR);
    }
    else
    {
        m_dungeon->setStatus(initrow, initcol, ACTORITEM);
    }
}

// Game destructor
Game::~Game()
{
    delete m_dungeon; // Delete the Game's Dungeon
}

// Mutator: the Player takes their turn
void Game::playerTurn(char ckey, bool& cheat)
{
    // Reset Player result strings at the beginning of each turn
    m_dungeon->setPlayerAttackString("");
    m_dungeon->setPlayerPickUpString("");
    m_dungeon->setPlayerScrollString("");
    m_dungeon->setPlayerWieldString("");

    // Determine whether the Player will regain 1 hit point before its turn
    int currentHP = m_dungeon->player()->getHP();
    int maxHP = m_dungeon->player()->getMaxHP();

    if (currentHP < maxHP)
    {
        // The Player has a 1/10 chance of regaining 1 hit point before its turn
        bool regainHealth = trueWithProbability(1.0 / 10.0);

        if (regainHealth)
        {
            m_dungeon->player()->changeStats(1, "HP");
        }
    }

    // If the Player is asleep, decrement its sleep time and return immediately
    if (m_dungeon->player()->getSleep() > 0)
    {
        m_dungeon->player()->changeStats(-1, "Sleep");
        return;
    }
    
    // Determine the movement commands for the Player's turn
    int drow = 0;
    int dcol = 0;

    // Obtain the Player's coordinates
    int playerrow = m_dungeon->player()->row();
    int playercol = m_dungeon->player()->col();

    // Obtain strength and hit points statistics for cheating purposes
    int currentStrength = m_dungeon->player()->getStrength();
    int modifier = 9 - currentStrength;
    int deltaMaxHP = 50 - m_dungeon->player()->getMaxHP();
    int deltaHP = 50 - m_dungeon->player()->getHP();

    // Switch on valid keyboard input
    switch (ckey)
    {
        case 'c':
            if (cheat)
            {
                break; // In the executable, you are only allowed to cheat once!
            }
            else
            {
                cheat = true;
            }

            // Increase the Player's hit points and maximum hit points to 50
            if (deltaMaxHP > 0)
            {
                m_dungeon->player()->changeStats(deltaMaxHP, "Max HP");
                m_dungeon->player()->changeStats(deltaHP, "HP");
            }

            // Increase the Player's strength up to 9, if possible
            if (modifier > 0)
            {
                m_dungeon->player()->changeStats(modifier, "Strength");
            }
            break;
        case ARROW_LEFT: // Move left
            dcol--;
            break;
        case ARROW_DOWN: // Move down
            drow++;
            break;
        case ARROW_UP: // Move up
            drow--;
            break;
        case ARROW_RIGHT: // Move right
            dcol++;
            break;
        case 'i': // Display the Player's inventory
            m_dungeon->player()->dispInventory();
            break;
        case 'g': // Player attempts to pick up Item
            m_dungeon->player()->pickUp(playerrow, playercol, true);
            break;
        case 'r': // Player attempts to read Scroll
            m_dungeon->player()->dispInventory();
            break;
        case 'w': // Player attempts to wield Weapon
            m_dungeon->player()->dispInventory();
            break;
        case '>': // Player attempts to descend stairway
            m_dungeon->player()->pickUp(playerrow, playercol, false);
            break;
        default: // Invalid keyboard input
            return; // Player does nothing on their turn
            break;
    }

    // If Player is moving, ensure that new cell is a valid move position
    if (m_dungeon->getStatus(playerrow + drow, playercol + dcol) == EMPTY ||
        m_dungeon->getStatus(playerrow + drow, playercol + dcol) == ITEM)
    {
        m_dungeon->player()->move(drow, dcol);
    }
    else if (m_dungeon->getStatus(playerrow + drow, playercol + dcol) == ACTOR 
        || m_dungeon->getStatus(playerrow + drow, playercol + dcol) == ACTORITEM)
    {
        m_dungeon->player()->attack(playerrow + drow, playercol + dcol);
    }
}

// Mutator: the monsters take their turn
void Game::monsterTurn()
{
    // If the Player already won, return
    if (m_dungeon->player()->isWinner())
    {
        return;
    }
    
    // Obtain the Dungeon's vector of all monsters
    std::vector<Actor*> ai = m_dungeon->getMonsters();

    // Reset the monsters' result string on each overall turn
    m_dungeon->setEmptyMonsterString();

    // Loop through the vector of monsters and take their turn appropriately
    for (size_t i = 0; i < ai.size(); i++)
    {
        // Obtain the monster's current coordinates
        int monsterRow = ai[i]->row();
        int monsterCol = ai[i]->col();

        // If the Player is dead, return immediately as the Game is automatically over
        if (m_dungeon->player()->getHP() <= 0)
        {
            break;
        }
        
        // Check whether each Dragon will regenerate 1 hit point
        if (ai[i]->getActorName() == "Dragon")
        {
            int currentHP = ai[i]->getHP();
            int maxHP = ai[i]->getMaxHP();

            // Dragons have a 1/10 chance of generating 1 hit point up to their maximum
            bool regainHealth = trueWithProbability(1.0 / 10.0);

            if (currentHP < maxHP && regainHealth)
            {
                ai[i]->changeStats(1, "HP");
            }
        }

        // Check whether each monster is asleep, in which case it does nothing
        if (ai[i]->getSleep() > 0)
        {
            ai[i]->changeStats(-1, "Sleep");
        }
        else
        {
            ai[i]->move(monsterRow, monsterCol);
        }
    }
}

// Mutator: play the actual Game
void Game::play()
{
    // Display the initial Game state
    m_dungeon->display();
    char key = getCharacter(); // Obtain the very first keyboard input
    bool flag = true; // Keeps track of whether we should enter the inventory
    bool cheatFlag = false; // Keeps track of whether we have already cheated

    // Game loop
    while (key != 'q')
    {
        clearScreen(); // Clear the screen at the beginning of each turn

        if (key == 'i' && flag)
        {
            playerTurn(key, cheatFlag); // Display the Player's inventory
            flag = false; // Set the inventory flag to be false

            key = getCharacter(); // Retrieve the next turn keyboard input
            monsterTurn(); // Let monsters all take their turn

            // Display the new Game state
            clearScreen();
            m_dungeon->display();
            flag = true; // After one turn, we can reenter the inventory if we wish

            key = getCharacter(); // Retrieve the next turn keyboard input
        }
        else if (key == 'r' && flag)
        {
            playerTurn(key, cheatFlag); // Display the Player's inventory
            flag = false; // Set the inventory flag to be false

            key = getCharacter(); // Retrieve the next turn keyboard input
            m_dungeon->player()->readScroll(key); // Player attempts to read Scroll
            monsterTurn(); // Let monsters all take their turn

            // Display the new Game state
            clearScreen();
            m_dungeon->display();
            flag = true; // After one turn, we can reenter the inventory if we wish

            key = getCharacter(); // Retrieve the next turn keyboard input
        }
        else if (key == 'w' && flag)
        {
            playerTurn(key, cheatFlag); // Display the Player's inventory
            flag = false; // Set the inventory flag to be false

            key = getCharacter(); // Retrieve the next turn keyboard input
            m_dungeon->player()->wieldWeapon(key); // Player attempts to wield Weapon
            monsterTurn(); // Let monsters all take their turn

            // Display the new Game state
            clearScreen();
            m_dungeon->display();
            flag = true; // After one turn, we can reenter the inventory if we wish
            key = getCharacter(); // Retrieve the next turn keyboard input
        }
        else
        {
            // Obtain the Player's current coordinates
            int prow = m_dungeon->player()->row();
            int pcol = m_dungeon->player()->col();

            // If we descend deeper, descend the stairwell but monsters do not take a turn
            if (m_dungeon->getStatus(prow, pcol) == ACTORITEM && key == '>')
            {
                int index = m_dungeon->itemAtPos(prow, pcol);
                std::string itemType = m_dungeon->getAvailItems()[index]->getType();

                if (itemType == "Stairway")
                {
                    playerTurn(key, cheatFlag); // Player descends down the stairway
                    clearScreen(); // Clear the screen
                }
                else
                {
                    playerTurn(key, cheatFlag); // Make a normal move
                    monsterTurn(); // Let monsters all take their turn
                }
            }
            else
            {
                // Normal Game turn ensues
                playerTurn(key, cheatFlag);
                monsterTurn();
            }
            
            flag = true; // After one turn, we can reenter the inventory if we wish

            // If the Player won the game, break out of the loop
            if (m_dungeon->player()->isWinner())
            {
                // Display the final Game state
                clearScreen();
                m_dungeon->display();
                
                std::cout << "Congratulations, you won!" << std::endl;
                break;
            }

            // Display the new Game state
            clearScreen();
            m_dungeon->display();

            // If the Player is dead, break out of the loop
            if (m_dungeon->player()->isDead())
            {
                std::cout << "You died!" << std::endl;
                break;
            }

            key = getCharacter(); // Retrieve the next turn keyboard input
        }
    }
}