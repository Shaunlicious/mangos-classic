/*

*/

#include "VitalityMgr.h"

VitalityMgr sVitalityMgr;

void VitalityMgr::Update(Player* player, uint32 diff)
{
    m_hungerSystem.Update(player, diff);
}

void VitalityMgr::BeginEat(Player* player, EatDrinkSession const& session)
{
    m_hungerSystem.BeginEat(player, session);
}

void VitalityMgr::EndEat(Player* player, EatDrinkSession const& session, uint32 elapsedMs)
{
    m_hungerSystem.EndEat(player, session, elapsedMs);
}

void VitalityMgr::BeginDrink(Player* player, EatDrinkSession const& session)
{
    // We'll implement thirst later.
}

void VitalityMgr::EndDrink(Player* player, EatDrinkSession const& session, uint32 elapsedMs)
{
    // We'll implement thirst later.
}
