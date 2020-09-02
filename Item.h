// Item.h

#ifndef ITEM_INCLUDED
#define ITEM_INCLUDED

#include <string>

/*
 Item [and Derived Class] Design Notes:
 - By program design, dynamic cast is not used within this project, as I preferred to 
   keep all of my pointers of the base type.
 - Item has three derived classes: Weapons (e.g. maces, magic axes, etc.), Scrolls (e.g.
   scroll of teleportation, scroll of improve armor, etc.) and Special Items (these are
   the golden idol and stairways).
 - The pure virtual function inside the Item class is the wieldItem() function. Weapons
   and scrolls may be wielded (in scrolls' case read), but not a generic item.
 - All Items have a pointer to the dungeon they belong in, as well as the player (if it's
   owned by the player), otherwise nullptr. If owned, the coordinates will be -1, -1.
 - As protected data members are forbidden, weapon characteristics are a part of the base 
   Item class. However, they are not accessed unless the Item is a Weapon specifically.
 - Item's are made distinguishable by their m_itemType data member. This will be set to 
   either "Weapon", or "Scroll". 
 - Item's name is the actual name of the Item (e.g. "mace", "improve armor", etc.).
 - Scroll's wieldItem() function (which reads the scroll), will adjust the player's stats
   accordingly, thus there is no need for those data members to be in the Item class.
 - Special Item's have no data members, as you cannot do anything with them.
 - For any functions called for specific data members, our design choice assumes that the 
   higher up code will call these functions appropriately (so less error checking is done).
*/

class Dungeon; // Indicates to compiler that Dungeon is a class
class Actor; // Indicates to compiler that Actor is a class

// Abstract Base Class
class Item
{
public:
	// Constructors and Destructors
	Item(Dungeon* d, Actor* a, std::string name, int sr, int sc);
	virtual ~Item();

	// Accessor Functions
	int getR() const; // Retrieve the item's row
	int getC() const; // Retrieve the item's column
	int getWeaponDamage() const; // Retrieve the weapon's damage bonus
	int getWeaponDexterity() const; // Retrieve the weapon's dexterity bonus
	Actor* getItemOwner() const; // Retrieve the item's owner, or it will return nullptr
	Dungeon* getItemDungeon() const; // Retrieve the item's dungeon
	std::string getName() const; // Retrieve the item's name
	std::string getType() const; // Retrieve the item's type

	// Mutator Functions
	void setItemType(std::string type); // Sets the item to either a weapon or scroll
	void setOwner(Actor* player); // Sets the item owner to the player
	void setWeaponDamage(int damage); // Sets the weapon's damage bonus
	void setWeaponDexterity(int dexterity); // Sets the weapon's dexterity bonus

	// Pure Virtual Functions
	virtual void wieldItem() = 0; // Only weapons and scrolls can be wielded/read
private:
	int m_rcoord; // The Item's row coordinate
	int m_ccoord; // The Item's column coordinate
	Actor* m_player; // The Item's owner
	Dungeon* m_dungeon; // The Item's dungeon
	std::string m_name; // The Item's name (forms the display action string)
	std::string m_itemType; // The Item's type

	// Weapon-Specific Characteristics
	int m_ddexterity; // The Weapon's dexterity bonus
	int m_ddamage; // The Weapon's damage amount
};

// Weapon Derived Class
class Weapon : public Item
{
public:
	// Constructors aand Destructors
	Weapon(Dungeon* d, Actor* a, std::string name, int sr, int sc);
	virtual ~Weapon();

	// Mutator Functions
	virtual void wieldItem(); // Wields the weapon
};

// Scroll Derived Class
class Scroll : public Item
{
public:
	// Constructors and Destructors
	Scroll(Dungeon* d, Actor* a, std::string name, int sr, int sc);
	virtual ~Scroll();

	// Mutator Functions
	virtual void wieldItem(); // Reads the scroll
private:
	int m_sdexterity; // Bonus in dexterity
	int m_smaxHP; // Bonus in maximum HP
	int m_sstrength; // Bonus in strength
	int m_sarmor; // Bonus in armor
};

// SpecialItem Derived Class
class SpecialItem : public Item
{
public:
	// Constructors and Destructors
	SpecialItem(Dungeon* d, Actor* a, std::string name, int pr, int pc);
	virtual ~SpecialItem();

	// Mutator Functions
	virtual void wieldItem(); // Activate the stairwell or win the game
};

#endif // ITEM_INCLUDED