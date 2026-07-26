/*

*/

#include "VitalityMgr.h"
#include "Entities/Player.h"

void VitalityMgr::Update(Player* player, uint32 diff)
{
    m_hunger.Update(player, diff); // Call the Update method of the HungerSystem instance, passing the player and time difference.
}