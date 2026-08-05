/*

*/

#ifndef Vitality_Mgr_H
#define Vitality_Mgr_H

#include "HungerSystem.h" // forward declaration of HungerSystem class
#include "EatDrinkTracker.h"

class Player; // forward declaration of Player class
//class HungerSystem; // forward declaration of HungerSystem class
class Item;

class VitalityMgr
{
public:

    void Update(Player* player, uint32 diff);

    void BeginEat(Player* player, EatDrinkSession const& session);

    void EndEat(Player* player, EatDrinkSession const& session, uint32 elapsedMs);

    void BeginDrink(Player* player, EatDrinkSession const& session);

    void EndDrink(Player* player, EatDrinkSession const& session, uint32 elapsedMs);

private:
    //HungerSystem m_hunger; // instance of HungerSystem to manage hunger updates
    HungerSystem m_hungerSystem;
}; 

extern VitalityMgr sVitalityMgr;

#endif