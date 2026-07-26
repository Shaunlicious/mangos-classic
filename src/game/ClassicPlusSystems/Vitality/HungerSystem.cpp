/*

*/

#include "HungerSystem.h"
#include "Entities/Player.h"
#include "Server/WorldSession.h"

//HungerSystem::HungerSystem()
//{
    //m_updateTimer = 0; // Initialize the update timer to 0 when the HungerSystem is created.
//}

void HungerSystem::Update(Player* player, uint32 diff)
{
    /*
    m_updateTimer += diff;     // Increment the update timer by the time difference since the last update.
    if (m_updateTimer < 6000) // Check if 6 second have passed (1000 milliseconds).
        return;

    m_updateTimer = 0; // Reset the update timer.
    */
    
    //player->GetSession()->SendNotification("Hunger Tick!");
    
    player->GetHungerTimer() += diff;

    if (player->GetHungerTimer() < 10000)
        return;

    player->GetHungerTimer() = 0;

    uint8 hunger = player->GetHunger();

    if (hunger > 0)
    {
        hunger--;

        player->SetHunger(hunger);
    }

    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Hunger: %u", player->GetHunger());

    player->GetSession()->SendNotification(buffer);

    
    
}