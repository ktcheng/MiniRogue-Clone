// Item.cpp

#include "Actor.h"
#include "Dungeon.h"
#include "Item.h"
#include "utilities.h"
#include <iostream>

/////////////////////////////////////
// Item Class Implementation
/////////////////////////////////////

// Item constructor with parameters dungeon, owner, name and starting coordinates
Item::Item(Dungeon* d, Actor* a, std::string name, int sr, int sc)
{
	// Set Item's data members accordingly
	m_dungeon = d;
	m_player = a;
	m_name = name;
	m_rcoord = sr;
	m_ccoord = sc;

	// Initially, Weapon's bonuses are 0
	m_ddexterity = 0;
	m_ddamage = 0;
}

// Item destructor
Item::~Item()
{
	// Item has nothing special to delete. By design, when the program terminates,
	// dungeon and actors will be deleted, such that Item need not worry about them.
}

// Accessor: returns the Item's row coordinate
int Item::getR() const
{
	return(m_rcoord);
}

// Accessor: returns the Item's column coordinate
int Item::getC() const
{
	return(m_ccoord);
}

// Accessor: returns the Item's damage bonus
int Item::getWeaponDamage() const
{
	return(m_ddamage); // N.B. By design, assume that the higher code calls appropriately
}

// Accessor: returns the Item's dexterity bonus
int Item::getWeaponDexterity() const
{
	return(m_ddexterity); // N.B. By design, assume that the higher code calls appropriately
}

// Accessor: returns the Item's owner
Actor* Item::getItemOwner() const
{
	return(m_player);
}

// Accessor: returns the Item's dungeon
Dungeon* Item::getItemDungeon() const
{
	return(m_dungeon);
}

// Accessor: returns the Item's name
std::string Item::getName() const
{
	return(m_name);
}

// Accessor: returns the Item's class
std::string Item::getType() const
{
	return(m_itemType);
}

// Mutator: sets the Item's type
void Item::setItemType(std::string type)
{
	m_itemType = type;
}

// Mutator: sets the Item's owner
void Item::setOwner(Actor* player)
{
	m_player = player;
}

// Mutator: sets the Item's damage bonus
void Item::setWeaponDamage(int damage)
{
	m_ddamage = damage;
}

// Mutator: sets the Item's dexterity bonus
void Item::setWeaponDexterity(int dexterity)
{
	m_ddexterity = dexterity;
}

/////////////////////////////////////
// Weapon Class Implementation
/////////////////////////////////////

// Weapon constructor with parameters dungeon, owner, name and starting coordinates
Weapon::Weapon(Dungeon* d, Actor* a, std::string name, int sr, int sc)
	: Item(d, a, name, sr, sc)
{
	// Set the class of Item to be a Weapon
	setItemType("Weapon");

	// Set the weapon boosts accordingly
	if (name == "mace" || name == "short sword")
	{
		setWeaponDamage(2);
	}
	else if (name == "long sword")
	{
		setWeaponDexterity(2);
		setWeaponDamage(4);
	}
	else if (name == "magic axe")
	{
		setWeaponDexterity(5);
		setWeaponDamage(5);
	}
	else // name == "magic fangs"
	{
		setWeaponDexterity(3);
		setWeaponDamage(2);
	}
}

// Weapon destructor
Weapon::~Weapon()
{
	// Weapon has nothing special to delete, therefore its destructor body is empty.
}

// Mutator: In Weapon's case, it is declared to ensure Weapon is not an abstract base class
void Weapon::wieldItem()
{
	// As a weapon is not destroyed upon usage, this function does nothing.
}

/////////////////////////////////////
// Scroll Class Implementation
/////////////////////////////////////

// Scroll constructor with parameters dungeon, owner, name and starting coordinates
Scroll::Scroll(Dungeon* d, Actor* a, std::string name, int sr, int sc)
	: Item(d, a, name, sr, sc)
{
	// Set the class of Item to be a scroll
	setItemType("Scroll");
	
	// Set the scroll boosts accordingly
	m_sdexterity = 0;
	m_smaxHP = 0;
	m_sstrength = 0;
	m_sarmor = 0;

	// N.B. Teleportation scrolls have no boosts. They merely move the player.
	if (name == "improve armor")
	{
		m_sarmor = randInt(1, 3);
	}
	else if (name == "strength")
	{
		m_sstrength = randInt(1, 3);
	}
	else if (name == "enhance health")
	{
		m_smaxHP = randInt(3, 8);
	}
	else if (name == "enhance dexterity")
	{
		m_sdexterity = 1;
	}
}

// Scroll destructor
Scroll::~Scroll()
{
	// Scroll has nothing special to delete, therefore its destructor body is empty.
}

// Mutator: In Scroll's case, the scroll is read
void Scroll::wieldItem()
{
	// N.B. As only the player may call this, we will not follow the nullptr
	
	// Get the name of the scroll being read
	std::string name = getName();
	
	// Depending on the scroll type, adjust the player accordingly
	if (name == "improve armor")
	{
		getItemOwner()->changeStats(m_sarmor, "Armor");
	}
	else if (name == "strength")
	{
		getItemOwner()->changeStats(m_sstrength, "Strength");
	}
	else if (name == "enhance health")
	{
		getItemOwner()->changeStats(m_smaxHP, "Max HP");
	}
	else if (name == "enhance dexterity")
	{
		getItemOwner()->changeStats(m_sdexterity, "Dexterity");
	}
	else // name == "teleportation"
	{
		// Determine new Player coordinates
		int rnew = randInt(1, getItemDungeon()->getRow() - 2);
		int cnew = randInt(1, getItemDungeon()->getCol() - 2);

		// While the coordinates are invalid, keep resetting
		while (getItemDungeon()->getStatus(rnew, cnew) != EMPTY &&
			   getItemDungeon()->getStatus(rnew, cnew) != ITEM)
		{
			rnew = randInt(1, getItemDungeon()->getRow() - 2);
			cnew = randInt(1, getItemDungeon()->getCol() - 2);
		}

		// Determine the difference between the new and current coordinates
		int drow = rnew - getItemOwner()->row();
		int dcol = cnew - getItemOwner()->col();
		getItemOwner()->move(drow, dcol); // Move the player to the new location
	}
}

/////////////////////////////////////
// SpecialItem Class Implementation
/////////////////////////////////////

// SpecialItem constructor with parameters dungeon, owner, name and starting coordinates
SpecialItem::SpecialItem(Dungeon* d, Actor* a, std::string name, int pr, int pc)
	: Item(d, a, name, pr, pc)
{
	// Set the class of Item to be a stairway or golden idol
	setItemType(name);
}

// SpecialItem destructor
SpecialItem::~SpecialItem()
{
	// SpecialItem has nothing special to delete, therefore its destructor body is empty.
}

// Mutator: In SpecialItem's case, it is declared to ensure it is not an abstract base class
void SpecialItem::wieldItem()
{
	// Special Items, by design, cannot be utilized. Therefore this function does nothing.
}