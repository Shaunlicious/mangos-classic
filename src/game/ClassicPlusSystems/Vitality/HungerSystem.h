/*

*/

#ifndef HUNGER_SYSTEM_H
#define HUNGER_SYSTEM_H

#include <cstdint>
#include "EatDrinkTracker.h" 

class Player; // Forward declaration of the Player class, instead of including the entire Player.h header file. Makes the code more efficient and reduces dependencies.
class Item;

enum class HungerState : uint8
{
    Starving,
    Hungry,
    Normal,
    WellFed,
    Thriving
};

class HungerSystem
{
public:
    
    void Update(Player* player, uint32 diff); // called every server update. diff is milliseconds since last update (normally 50ms or 100ms).
    
    void SendStatus(Player* player);

    void UpdateBuffs(Player* player);

    void BeginEat(Player* player, EatDrinkSession const& session);

    void EndEat(Player* player, EatDrinkSession const& session, uint32 elapsedMs);

    void BeginDrink(Player* player, EatDrinkSession const& session);

    void EndDrink(Player* player, EatDrinkSession const& session, uint32 elapsedMs);

    uint8 GetNutritionValue(EatDrinkSession const& session);

    const char* GetStateName(HungerState state);
    HungerState GetState(Player* player);

    void RemoveHungerBuffs(Player* player, HungerState state);

    void ApplyStatModifier(Player* player, UnitMods stat, float percent, bool apply);

    void ApplyThriving(Player* player);
    void RemoveThriving(Player* player);

    void ApplyWellFed(Player* player);
    void RemoveWellFed(Player* player);

    void ApplyHungry(Player* player);
    void RemoveHungry(Player* player);

    void ApplyStarving(Player* player);
    void RemoveStarving(Player* player);

    void ApplyStateAura(Player* player, HungerState state);
    void RemoveStateAuras(Player* player);

private:

};

#endif