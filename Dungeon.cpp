// Dungeon.cpp

#include "Actor.h"
#include "Dungeon.h"
#include "Item.h"
#include "utilities.h"
#include <iostream>

// Dungeon constructor with parameters Game level and Goblin smell distance
Dungeon::Dungeon(int level, int smellDistance)
{
	// Set the Dungeon's data members accordingly
	m_rows = DUNGEONROWS;
	m_cols = DUNGEONCOLS;
	m_level = level;
	m_smellDistance = smellDistance;
	m_player = nullptr; // Player is added by the Game class

	// Set the Dungeon's output strings accordingly
	m_playerAttackResult = "";
	m_playerPickedUpItem = "";
	m_playerReadScroll = "";
	m_playerWieldWeapon = "";
	m_monsterAttackResult = "";

	// Generates the Dungeon layout and sets remaining data members accordingly
	generateLayout(level);
}

// Dungeon destructor
Dungeon::~Dungeon()
{	
	// Delete Dungeon's vector of available items
	for (size_t i = 0; i < m_availItems.size(); i++)
	{
		delete m_availItems[i];
	}

	// Delete Dungeon's vector of monsters
	for (size_t i = 0; i < m_monsterList.size(); i++)
	{
		delete m_monsterList[i];
	}

	delete m_player; // Delete Dungeon's player
}

// Accessor: Returns the Dungeon's row dimension
int Dungeon::getRow() const
{
	return(m_rows);
}

// Accessor: Returns the Dungeon's column dimension
int Dungeon::getCol() const
{
	return(m_cols);
}

// Accessor: Returns the Dungeon's level
int Dungeon::getLevel() const
{
	return(m_level);
}

// Accessor: Returns the Dungeon's cell status
int Dungeon::getStatus(int r, int c) const
{
	// If the coordinates are valid return the cell status
	if (inBounds(r, c))
	{
		return(m_grid[r][c]);
	}
	else
	{
		return(-1); // While this should never trigger, it safeguards against bad calls
	}
}

// Accessor: Returns the index of the Dungeon's Actor
int Dungeon::actorAtPos(int r, int c) const
{
	int correctIndex = -1; // Initially, assume an invalid index

	// Loop through the monsters vector, checking for matching coordinates
	for (size_t i = 0; i < m_monsterList.size(); i++)
	{
		// Retrieve the Actor's coordinates
		int monsterr = m_monsterList[i]->row();
		int monsterc = m_monsterList[i]->col();

		// If the Actor is the target, return the correct index
		if (monsterr == r && monsterc == c)
		{
			correctIndex = i;
			break;
		}
	}

	return(correctIndex); // Return the vector index of the Actor, if it exists
}

// Accessor: Rreturns the index of the Dungeon's Item
int Dungeon::itemAtPos(int r, int c) const
{
	int correctIndex = -1; // Initially, assume an invalid index

	// Loop through the Items vector, checking for matching coordinates
	for (size_t i = 0; i < m_availItems.size(); i++)
	{
		// Retrieve the Actor's coordinates
		int itemr = m_availItems[i]->getR();
		int itemc = m_availItems[i]->getC();

		// If the Item is the target, return the correct index
		if (itemr == r && itemc == c)
		{
			correctIndex = i;
			break;
		}
	}

	return(correctIndex); // Return the vector index of the Item, if it exists
}

// Accessor: Checks if the coordinates are valid
bool Dungeon::inBounds(int r, int c) const
{
	if (r >= 0 && c >= 0 && r < m_rows && c < m_cols)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

// Accessor: Displays the Dungeon status
void Dungeon::display() const
{
	// Initialize our output grid
	char displayGrid[DUNGEONROWS][DUNGEONCOLS];

	// Set and display our Dungeon
	for (int r = 0; r < m_rows; r++)
	{
		for (int c = 0; c < m_cols; c++)
		{
			// Retrieve the Dungeon cell status
			int state = getStatus(r, c);

			// Set the output grid appropriately
			if (state == EMPTY)
			{
				displayGrid[r][c] = ' '; // Display an empty cell
			}
			else if (state == ACTOR || state == ACTORITEM)
			{
				// Determine the type of the Actor
				int actorIndex = actorAtPos(r, c);

				if (actorIndex == -1)
				{
					// This is the Player, as it is not in the monsters list
					displayGrid[r][c] = '@'; // Display the Player
				}
				else
				{
					// Retrieve the type of monster present
					std::string actorClass = m_monsterList[actorIndex]->getActorName();

					if (actorClass == "Goblin")
					{
						displayGrid[r][c] = 'G'; // Display the Goblin
					}
					else if (actorClass == "Bogeyman")
					{
						displayGrid[r][c] = 'B'; // Display the Bogeyman
					}
					else if (actorClass == "Snakewoman")
					{
						displayGrid[r][c] = 'S'; // Display the Snakewoman
					}
					else if (actorClass == "Dragon")
					{
						displayGrid[r][c] = 'D'; // Display the Dragon
					}
				}
			}
			else if (state == ITEM)
			{
				int itemClassIndex = itemAtPos(r, c);

				if (itemClassIndex != -1)
				{
					// Retrieve the type of Item present
					std::string itemClass = m_availItems[itemClassIndex]->getType();

					if (itemClass == "Weapon")
					{
						displayGrid[r][c] = ')'; // Display the weapon
					}
					else if (itemClass == "Scroll")
					{
						displayGrid[r][c] = '?'; // Display the Scroll
					}
					else if (itemClass == "Stairway")
					{
						displayGrid[r][c] = '>'; // Display the Stairway
					}
					else if (itemClass == "Golden Idol")
					{
						displayGrid[r][c] = '&'; // Display the Golden Idol
					}
				}
			}
			else
			{
				displayGrid[r][c] = '#'; // Display the wall
			}

			// Output display character to the screen
			std::cout << displayGrid[r][c];
		}
		std::cout << std::endl;
	}

	// Retrieve the Player's hit points level
	int currentHP = m_player->getHP();

	if (currentHP < 0)
	{
		currentHP = 0; // Don't display negative hit points
	}

	// Display Player's statistics
	std::cout << "Dungeon Level: " << m_level << ", Hit Points: "
		<< currentHP << ", Armor: " << m_player->getArmor()
		<< ", Strength: " << m_player->getStrength() << ", Dexterity: "
		<< m_player->getDexterity() << std::endl;
	std::cout << std::endl;

	// Display the result strings, when appropriate
	if (m_playerAttackResult != "")
	{
		std::cout << m_playerAttackResult << std::endl;
	}
	if (m_playerPickedUpItem != "")
	{
		std::cout << m_playerPickedUpItem << std::endl;
	}
	if (m_playerWieldWeapon != "")
	{
		std::cout << m_playerWieldWeapon << std::endl;
	}
	if (m_playerReadScroll != "")
	{
		std::cout << m_playerReadScroll << std::endl;
	}
	if (m_monsterAttackResult != "")
	{
		std::cout << m_monsterAttackResult << std::endl;
	}
}

// Accessor: Returns the Dungeon's Player pointer
Actor* Dungeon::player() const
{
	return(m_player);
}

// Accessor: Returns the Dungeon's vector of available Items
std::vector<Item*> Dungeon::getAvailItems() const
{
	return(m_availItems);
}

// Accessor: Returns the Dungeon's vector of available monsters
std::vector<Actor*> Dungeon::getMonsters() const
{
	return(m_monsterList);
}

// Mutator: Adds the Dungeon's Player
bool Dungeon::addPlayer(int r, int c)
{
	// N.B. By design, assume that the higher code calls appropriately

	// If the coordinates are not valid, return false
	if (!inBounds(r, c))
	{
		return(false);
	}

	// If the Player already exists, return false
	if (m_player != nullptr)
	{
		return(false);
	}

	m_player = new Player(this, r, c); // Adds the Player to the Dungeon

	return(true);
}

// Mutator: Adds a random Item to the Dungeon
bool Dungeon::addrandItem(int r, int c, bool sflag, bool gflag)
{
	// N.B. By design, assume that the higher code calls appropriately
	// N.B. sflag indicates whether a SpecialItem needs to be added.
	// N.B. gflag indicates whether a Golden Idol needs to be added.

	// If the coordinates are not valid, return false
	if (!inBounds(r, c))
	{
		return(false); // While this should never trigger, it safeguards against bad calls
	}

	if (!sflag)
	{
		// Randomly choose an index for a Weapon/Scroll
		int weaponClass = randInt(0, 2); // Only 3 Weapons are randomly available
		int scrollClass = randInt(0, 3); // Only 4 Scrolls are randomly available

		std::string weaponType[3] = { "short sword", "long sword", "mace" };
		std::string scrollType[4] = { "improve armor", "strength",
		"enhance health", "enhance dexterity" };

		// Determines whether we add a Weapon or Scroll
		bool isWeapon = trueWithProbability(0.5);
		Item* someItem;

		if (isWeapon)
		{
			someItem = new Weapon(this, nullptr, weaponType[weaponClass], r, c);
		}
		else
		{
			someItem = new Scroll(this, nullptr, scrollType[scrollClass], r, c);
		}

		m_availItems.push_back(someItem); // Adds Item to Dungeon's vector of available Items
		return(true);
	}
	else if (sflag && !gflag)
	{
		// Adds the stairway to the Dungeon
		Item* stairwell = new SpecialItem(this, nullptr, "Stairway", r, c);
		m_availItems.push_back(stairwell);
		return(true);
	}
	else
	{
		// Adds the Golden Idol to the Dungeon
		Item* goldenIdol = new SpecialItem(this, nullptr, "Golden Idol", r, c);
		m_availItems.push_back(goldenIdol);
		return(true);
	}

	return(false);
}

// Mutator: Adds the monster Item drop to the Dungeon
void Dungeon::addDrop(int r, int c, std::string name, std::string weaponClass)
{
	// If the coordinates are not valid, return false
	if (!inBounds(r, c))
	{
		return; // While this should never trigger, it safeguards against bad calls
	}

	int cellState = getStatus(r, c); // Obtain the Dungeon's current cell status

	// Double check: ensure that current cell can accomodate Item
	if (cellState == ITEM || cellState == WALLS || cellState == ACTORITEM)
	{
		return;
	}

	Item* droppedItem;
	if (weaponClass == "Weapon")
	{
		droppedItem = new Weapon(this, nullptr, name, r, c);
	}
	else
	{
		droppedItem = new Scroll(this, nullptr, name, r, c);
	}

	m_availItems.push_back(droppedItem);
	setStatus(r, c, ITEM); // If drop occurs, the monster dies, thus the status is ITEM
}

// Mutator: Generates unique Dungeon layout
void Dungeon::generateLayout(int level)
{
	int rectangleNum = randInt(4, level + 4); // Dungeons contain at least four rooms

	// Vectors are used below to keep track of room coordinates
	std::vector<int> roomRows;
	std::vector<int> roomCols;

	// Initialize the Dungeon to be all walls, then "eat out" the rooms appropriately
	for (int r = 0; r < m_rows; r++)
	{
		for (int c = 0; c < m_cols; c++)
		{
			setStatus(r, c, WALLS);
		}
	}

	// Generate Dungeon rooms
	while (rectangleNum > 0)
	{
		// For each room, generate a suitable coordinate for the top left corner
		int rowcorner = randInt(1, DUNGEONROWS - 2);
		int colcorner = randInt(1, DUNGEONCOLS - 2);

		// For each room, generate a suitable size
		int width = randInt(5, DUNGEONCOLS / 2);
		int height = randInt(3, DUNGEONROWS / 2);

		// While the room overlaps with another, keep resetting the room's statistics
		while (overlap(rowcorner, colcorner, width, height))
		{
			rowcorner = randInt(1, DUNGEONROWS - 2);
			colcorner = randInt(1, DUNGEONCOLS - 2);

			width = randInt(5, DUNGEONCOLS / 2);
			height = randInt(3, DUNGEONROWS / 2);
		}

		// Add rows and columns to the vectors to generate corridors later
		int rowOffset = randInt(0, height / 2);
		int colOffset = randInt(0, width / 2);
		roomRows.push_back(rowcorner + rowOffset); roomCols.push_back(colcorner + colOffset);

		// "Eat out" the Dungeon rooms
		for (int r = rowcorner; r < rowcorner + height; r++)
		{
			for (int c = colcorner; c < colcorner + width; c++)
			{
				setStatus(r, c, EMPTY);
			}
		}

		rectangleNum--;
	}

	// Generate a random number of Items per level
	bool isTwo = trueWithProbability(0.5);
	int itemNum;
	if (isTwo)
	{
		itemNum = 2;
	}
	else
	{
		itemNum = 3;
	}

	// Place the Items randomly
	for (int i = 0; i < itemNum; i++)
	{
		// Retrieve the coordinates to place Items
		int randrow = randInt(1, m_rows - 2);
		int randcol = randInt(1, m_cols - 2);

		// While the coordinates are invalid, keep resetting
		while (getStatus(randrow, randcol) != EMPTY
			&& getStatus(randrow, randcol) != ACTOR)
		{
			randrow = randInt(1, m_rows - 2);
			randcol = randInt(1, m_cols - 2);
		}

		// Add a random Item to the Dungeon and set cell status accordingly
		addrandItem(randrow, randcol, false, false);
		setStatus(randrow, randcol, ITEM);
	}

	// Check whether to add a stairway or golden idol
	if (m_level == 4)
	{
		// Retrieve coordinates to place the golden idol
		int goldrow = randInt(1, m_rows - 2);
		int goldcol = randInt(1, m_cols - 2);

		// While the coordinates are invalid, keep resetting
		while (getStatus(goldrow, goldcol) != EMPTY
			&& getStatus(goldrow, goldcol) != ACTOR)
		{
			goldrow = randInt(1, m_rows - 2);
			goldcol = randInt(1, m_cols - 2);
		}

		// Adds the golden idol to the Dungeon
		addrandItem(goldrow, goldcol, true, true);
		setStatus(goldrow, goldcol, ITEM);
	}
	else
	{
		// Retrieve coordinates to place the stairwell
		int stairrow = randInt(1, m_rows - 2);
		int staircol = randInt(1, m_cols - 2);

		// While the coordinates are invalid, keep resetting
		while (getStatus(stairrow, staircol) != EMPTY
			&& getStatus(stairrow, staircol) != ACTOR)
		{
			stairrow = randInt(1, m_rows - 2);
			staircol = randInt(1, m_cols - 2);
		}

		// Adds the stairway to the Dungeon
		addrandItem(stairrow, staircol, true, false);
		setStatus(stairrow, staircol, ITEM);
	}

	// Generate the Dungeon's corridors
	for (size_t i = 0; i < roomRows.size() - 1; i++)
	{
		// Locate the starting coordinates for corridor digging
		int startRow = roomRows[i];
		int startCol = roomCols[i];

		// Locate the target coordinates to dig the corridor to
		int destRow = roomRows[i + 1];
		int destCol = roomCols[i + 1];

		bool isHigher = false; bool toTheRight = false;

		// Determine relative direction between the start and end coordinates
		if (destRow <= startRow)
		{
			isHigher = true;
		}
		if (destCol >= startCol)
		{
			toTheRight = true;
		}

		// Vertical corridor digging
		while (true)
		{
			// Dig out Dungeon walls along the corridor
			if (getStatus(startRow, startCol) == WALLS)
			{
				setStatus(startRow, startCol, EMPTY);
			}

			// If we have dug out the vertical corridor to the end, break
			if (startRow == destRow)
			{
				break;
			}

			if (isHigher)
			{
				startRow--;
			}
			else
			{
				startRow++;
			}

		}

		// Horizontal corridor digging
		while (true)
		{
			// Dig out Dungeon walls along the corridor
			if (getStatus(startRow, startCol) == WALLS)
			{
				setStatus(startRow, startCol, EMPTY);
			}

			// If we have dug out the horizontal corridor to the end, break
			if (startCol == destCol)
			{
				break;
			}

			if (toTheRight)
			{
				startCol++;
			}
			else
			{
				startCol--;
			}
		}
	}

	// Generate a random number of monsters per level
	int maxMonsters = 5 * (level + 1) + 1;
	m_monsterNum = randInt(2, maxMonsters);

	// Place the monsters randomly
	for (int i = 0; i < m_monsterNum; i++)
	{
		// Retrieve the coordinates to place monsters
		int randrow = randInt(1, m_rows - 2);
		int randcol = randInt(1, m_cols - 2);

		// While the coordinates are invalid, keep resetting
		while (getStatus(randrow, randcol) != EMPTY
			&& getStatus(randrow, randcol) != ITEM)
		{
			randrow = randInt(1, m_rows - 2);
			randcol = randInt(1, m_cols - 2);
		}

		// Determine which monster to add
		int chooser = randInt(0, level);

		// For level zero, we can add either Snakewomen or Goblins
		if (level == 0)
		{
			chooser = randInt(0, 1);
		}

		// Adds the appropriately selected monster
		Actor* myMonster;
		switch (chooser)
		{
			case 0:
				myMonster = new Snakewoman(this, randrow, randcol);
				break;
			case 1:
				myMonster = new Goblin(this, randrow, randcol, m_smellDistance);
				break;
			case 2:
				myMonster = new Bogeyman(this, randrow, randcol);
				break;
			default: // We are at level three or higher
				myMonster = new Dragon(this, randrow, randcol);
				break;
		}

		m_monsterList.push_back(myMonster);

		// Set monster's Dungeon cell status
		if (getStatus(randrow, randcol) == ITEM)
		{
			setStatus(randrow, randcol, ACTORITEM);
		}
		else
		{
			setStatus(randrow, randcol, ACTOR);
		}
	}
}

// Mutator: Generates new level if previous level was cleared
void Dungeon::newLevel(int level)
{
	// Deletes the vector of available Items
	for (size_t i = 0; i < m_availItems.size(); i++)
	{
		delete m_availItems[i];
	}

	// Deletes the vector of monsters
	for (size_t i = 0; i < m_monsterList.size(); i++)
	{
		delete m_monsterList[i];
	}

	m_level = level; // Set the new level accordingly
	m_availItems.clear(); // Reset the vector of available Items
	m_monsterList.clear(); // Reset the vector of monsters

	// Generate new Dungeon layout
	generateLayout(level);

	// Move the Player to a random location
	int rnew = randInt(1, m_rows - 2);
	int cnew = randInt(1, m_cols - 2);

	// While the coordinates are invalid, keep resetting
	while (getStatus(rnew, cnew) != EMPTY && getStatus(rnew, cnew) != ITEM)
	{
		rnew = randInt(1, m_rows - 2);
		cnew = randInt(1, m_cols - 2);
	}

	// Calculate the difference between the new and current Player coordinates
	int drow = rnew - m_player->row();
	int dcol = cnew - m_player->col();
	m_player->move(drow, dcol);
}

// Mutator: Removes Item from vector of available Items
void Dungeon::removeItem(int index)
{
	std::vector<Item*>::iterator it; 
	int counter = 0; // Keeps track of how much to increment iterator

	// Loop through the vector to increment the iterator to our target
	for (it = m_availItems.begin(); it < m_availItems.end(); it++)
	{
		if (counter == index)
		{
			break;
		}
		counter++;
	}

	m_availItems.erase(it); // Cannot delete (*it), as we shift the Item to the inventory
}

// Mutator: Removes monster from vector of monsters
void Dungeon::removeMonster(int index)
{
	std::vector<Actor*>::iterator it;
	int counter = 0; // keeps track of how much to increment iterator

	// Loop through the vector to increment the iterator to our target
	for (it = m_monsterList.begin(); it < m_monsterList.end(); it++)
	{
		if (counter == index)
		{
			break;
		}
		counter++;
	}

	delete (*it); // Delete (*it), as the monster dies and we remove it from the Dungeon
	it = m_monsterList.erase(it);
}

// Mutator: Sets the Dungeon's cell status accordingly
void Dungeon::setStatus(int r, int c, int status)
{
	// N.B. By design, assume that the higher code calls appropriately

	// If the coordinates are valid, set the Dungeon cell status
	if (inBounds(r, c))
	{
		m_grid[r][c] = status;
	}
}

// Mutator: Sets the Player attack string
void Dungeon::setPlayerAttackString(std::string s)
{
	m_playerAttackResult = s;
}

// Mutator: Sets the Player pick up string
void Dungeon::setPlayerPickUpString(std::string s)
{
	m_playerPickedUpItem = s;
}

// Mutator: Sets the Player Scroll string
void Dungeon::setPlayerScrollString(std::string s)
{
	m_playerReadScroll = s;
}

// Mutator: Sets the Player wield Weapon string
void Dungeon::setPlayerWieldString(std::string s)
{
	m_playerWieldWeapon = s;
}

// Mutator: Sets the monster attack string
void Dungeon::setMonsterAttackString(std::string s)
{
	m_monsterAttackResult += s;
}

// Mutator: Sets empty monster attack string
void Dungeon::setEmptyMonsterString()
{
	m_monsterAttackResult = "";
}

// Private Helper Function: Determines if room placement is valid
bool Dungeon::overlap(int crow, int ccol, int dwidth, int dheight)
{
	// N.B. Assumes the coordinates are the top left corner of the rectangle
	bool isOverlap = false;

	// If our room has gone out of bounds, return true
	if (crow + dheight >= DUNGEONROWS - 1 || ccol + dwidth >= DUNGEONCOLS - 1)
	{
		return(true); 
	}

	// Loop through the Dungeon's cells, checkingif any are invalid
	for (int i = crow; i < crow + dheight; i++)
	{
		for (int j = ccol; j < ccol + dwidth; j++)
		{
			if (i == crow)
			{
				if (getStatus(i - 1, j) == EMPTY)
				{
					return(true); // We have infringed upon another room
				}
			}

			if (i == crow + dheight - 1)
			{
				if (getStatus(i + 1, j) == EMPTY)
				{
					return(true); // We have infringed upon another room
				}
			}

			if (j == ccol + dwidth - 1)
			{
				if (getStatus(i, j + 1) == EMPTY)
				{
					return(true); // We have infringed upon another room
				}
			}

			if (j == ccol)
			{
				if (getStatus(i, j - 1) == EMPTY)
				{
					return(true); // We have infringed upon another room
				}
			}

			if (getStatus(i, j) == EMPTY)
			{
				return(true); // We have infringed onto an existing room!
			}
		}
	}

	return(isOverlap);
}