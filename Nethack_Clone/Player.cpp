#include "Player.h"
#include "Consumable.h"
#include "Creature.h"
#include "Potion.h"
#include <string>
#include <iostream>

using namespace std;

Player::Player()
{
	m_iExperience = 0;
	m_iLevel = 0;
	regenCount = 1;
	score = 0;
	weapon = new Weapon("Bare Hands", 1, 0);
}

bool Player::canMove(int row, int col, Floor* fl)
{
	//Check to see if the tile to move to is out of range
	if (row < 0 || row >= fl->getHeight() || col < 0 || col >= fl->getWidth())
	{
		return false;
	}

	//Check to see if there is a character
	Character * ch = fl->getTile(row, col)->getCharacter();
	if (ch != NULL)
	{
		attack(ch);
		return false;
	}

	char symbol = fl->getTile(row, col)->getSymbol();
	if (symbol == ' ') {return false;}
	return true;
}

//If the desired tile to move to has another character then we attack (called in the canMove function)
void Player::move(Floor* fl, char dir)
{
	//Check if valid position
	bool valid;
	int newRow;
	int newCol;

	int iRow = getRow();
	int iCol = getCol();
	if (dir == 'w')
	{
		newRow = iRow - 1;
		newCol = iCol;
		valid = canMove(newRow, newCol, fl);
	}
	else if (dir == 'a') 
	{
		newRow = iRow;
		newCol = iCol - 1;
		valid = canMove(newRow, newCol, fl);
	}
	else if (dir == 's')
	{
		newRow = iRow + 1;
		newCol = iCol;
		valid = canMove(newRow, newCol, fl);
	}
	else if (dir == 'd')
	{
		newRow = iRow;
		newCol = iCol + 1;
		valid = canMove(newRow, newCol, fl);
	}

	//Move op
	if (valid)
	{
		//Change the map of the dungeon floor
		vector<std::vector<Tile *>> levelMap = fl->getMap();
		levelMap[iRow][iCol]->clearCharacter();

		setRow(newRow);
		setCol(newCol);
		levelMap[newRow][newCol]->setCharacter(this);
	}
}

void Player::attack(Character * target)
{
	target->setHealth(target->getHealth() - 1);
	cout << "Attacked a " << target->getName() << " with your " << weapon->getName() << "!" << endl;

	if (target->getHealth() > 0)
	{
		cout << "The " << target->getName() << " has " << target->getHealth() << "HP left." << endl;
	}
	else
	{
		cout << "The " << target->getName() << " has been killed!" << endl;
		Creature * cr = dynamic_cast<Creature *>(target);
		if (cr != NULL)
		{
			cr->giveExp(this);
		}
	}
}

//Regenerate 1 HP every 4 turns
void Player::regen()
{
	regenCount++;
	if (regenCount % 2 == 0 && getHealth() != getMaxHealth())
	{
		setHealth(getHealth() + 1);
		regenCount = 0;
	}
}

//Sets level after every turn. Level up after every 10 exp points. Also sets the players max health based
//on the level
void Player::levelUp()
{
	int currLevel = getLevel();
	int newLevel = getExperience() / 10;

	if (newLevel == currLevel) {}	//Do nothing
	else
	{
		cout << "You have leveled up! You are now level " << newLevel << "!" << endl;
		setLevel(newLevel);
		setMaxHealth(10 + 3 * (getLevel() - 1));
	}
}

void Player::use(Floor * fl)
{
	//Fetch the vector of items that is currently on the ground and attempt to use all of them
	vector<Item *> & items = fl->getMap()[getRow()][getCol()]->getItems();

	for (int i = 0; i < items.size(); i++)
	{
		Item * pIt = items[i];

		//Is consumable
		Consumable * cons = dynamic_cast<Consumable*>(pIt);
		if (cons != NULL)
		{
			cons->use(static_cast<Character*>(this));
			fl->getMap()[getRow()][getCol()]->removeItem(i);
		}

		//Is Armor
		Armor * pArm = dynamic_cast<Armor*>(pIt);
		if (pArm != NULL)
		{
			equip(pArm);
			fl->getMap()[getRow()][getCol()]->removeItem(i);
		}

		//Is Weapon
		Weapon * pWeap = dynamic_cast<Weapon*>(pIt);
		if (pWeap != NULL)
		{
			equip(pWeap);
			fl->getMap()[getRow()][getCol()]->removeItem(i);
		}

		//Else add to inventory
		vInventory.push_back(pIt);
		fl->getMap()[getRow()][getCol()]->removeItem(i);
	}
}

void Player::equip(Equipment * eq)
{
	Armor * pArm = dynamic_cast<Armor*>(eq);
	if (pArm != NULL)
	{
		vInventory.push_back(pArm);
		armor = pArm;
		cout << "You have equipped a " << pArm->getName() << "!" << endl;
	}
	Weapon * pWeap = dynamic_cast<Weapon *>(eq);
	if (pWeap != NULL)
	{
		vInventory.push_back(pWeap);
		weapon = pWeap;
		cout << "You have equipped a " << pWeap->getName() << "!" << endl;
	}
}

int Player::getExperience()
{
	return m_iExperience;
}

void Player::setExperience(int iExperience)
{
	m_iExperience = iExperience;
}

int Player::getLevel()
{
	return m_iLevel;
}

void Player::setLevel(int iLevel)
{
	m_iLevel = iLevel;
}

int Player::getScore()
{
	return 4 * getExperience();
}

Weapon * Player::getWeapon()
{
	return weapon;
}

Armor * Player::getArmor()
{
	return armor;
}

void Player::setWeapon(Weapon * wp)
{
	weapon = wp;
}

void Player::setArmor(Armor * ar)
{
	armor = ar;
}

void Player::dumpObject()
{
	cout << "Player:" << endl;

	dumpObjectData();
}

void Player::dumpObjectData()
{
	Character::dumpObjectData();

	cout << "Experience: " << getExperience() << endl;
	cout << "     Level: " << getLevel() << endl;
}

void Player::writeFragment(ostream & output)
{
	output << " <Player>" << endl;

	writeDataAsFragment(output);

	output << " </Player>" << endl;
}

void Player::writeDataAsFragment(ostream & output)
{
	Character::writeDataAsFragment(output);

	output << "	<experience>"
		<< getExperience()
		<< "</experience>"
		<< endl
		<< "	<level>"
		<< getLevel()
		<< "</level>"
		<< endl;
}

void Player::setElementData(string sElementName, string sValue)
{
	Character::setElementData(sElementName, sValue);
	
	if (sElementName == "experience")
	{
		setExperience(atoi(sValue.c_str()));
	}
	else if (sElementName == "level")
	{
		setLevel(atoi(sValue.c_str()));
	}
}

