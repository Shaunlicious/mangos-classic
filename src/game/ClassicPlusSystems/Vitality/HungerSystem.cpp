/*

*/

#include "HungerSystem.h"
#include "Entities/Player.h"
#include "Entities/Item.h"
#include "Entities/Unit.h"
#include "Server/WorldSession.h"
#include "VitalityConfig.h"
#include "VitalityDefines.h"


void HungerSystem::BeginEat(Player* player, EatDrinkSession const& session)
{
    // Nothing needed yet.

    // We'll use this later if we want to
    // start meal animations, remember meal
    // state, etc.
}

void HungerSystem::EndEat(Player* player, EatDrinkSession const& session, uint32 elapsedMs)
{
    uint8 nutritionValue = GetNutritionValue(session); // Temporary value for every food
    uint8 hungerGain = 0;

    if (elapsedMs >= 20000)
        hungerGain = nutritionValue;

    else if (elapsedMs >= 14000)
        hungerGain = nutritionValue * MEAL_60_PERCENT / 100;

    else if (elapsedMs >= 10000)
        hungerGain = nutritionValue * MEAL_30_PERCENT / 100;

    else if (elapsedMs >= 5000)
        hungerGain = nutritionValue * MEAL_10_PERCENT / 100;

    if (hungerGain == 0)
        return;

    uint8 hunger = player->GetHunger();

    if (hunger + hungerGain > MAX_HUNGER)
        hunger = 100;
    else
        hunger += hungerGain;

    player->SetHunger(hunger);
    UpdateBuffs(player);

    //char buffer[64];
    //snprintf(buffer, sizeof(buffer), "Meal +%u Hunger", hungerGain);
    //player->GetSession()->SendNotification(buffer);

    UpdateBuffs(player);
    SendStatus(player);

}

uint8 HungerSystem::GetNutritionValue(EatDrinkSession const& session)
{
    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(session.itemEntry);

    if (!proto)
        return 20;

    switch (session.source) 
    {
        case EatDrinkSource::Conjured:
            return 10;

        case EatDrinkSource::Regular:
            return 25;

        default:
            return 20;
    }
}

void HungerSystem::SendStatus(Player* player)
{
    ChatHandler(player).PSendSysMessage("|cff00ff00[Vitality]|r Hunger: %u/100 (%s)", player->GetHunger(), GetStateName(player->GetHungerState())); // Display hunger level in chat log (in game)
}

const char* HungerSystem::GetStateName(HungerState state)
{
    switch (state)
    {
        case HungerState::Thriving:
            return "Thriving";

        case HungerState::WellFed:
            return "Well Fed";

        case HungerState::Hungry:
            return "Hungry";

        case HungerState::Starving:
            return "Starving";

        default:
            return "Normal";
    }
}

void HungerSystem::Update(Player* player, uint32 diff)
{

    player->GetHungerTimer() += diff;

    if (player->GetHungerTimer() < HUNGER_UPDATE_MS)
        return;

    player->GetHungerTimer() = 0;

    uint8 hunger = player->GetHunger();

    if (hunger > 0)
    {
        hunger--;

        player->SetHunger(hunger);
        UpdateBuffs(player);
    }
    
    //char buffer[64];
    //snprintf(buffer, sizeof(buffer), "hunger level %u", hunger);
    //player->GetSession()->SendNotification(buffer);

    SendStatus(player);

}

HungerState HungerSystem::GetState(Player* player)
{
    uint8 hunger = player->GetHunger();

    if (hunger >= THRIVING_THRESHOLD)
        return HungerState::Thriving;

    if (hunger >= WELLFED_THRESHOLD)
        return HungerState::WellFed;

    if (hunger >= NORMAL_THRESHOLD)
        return HungerState::Normal;

    if (hunger >= HUNGRY_THRESHOLD)
        return HungerState::Hungry;

    return HungerState::Starving;
}


void HungerSystem::UpdateBuffs(Player* player)
{
    HungerState newState = GetState(player);

    if (newState == player->GetHungerState())
        return;

    RemoveHungerBuffs(player, player->GetHungerState()); // Stop buffs from previous states stacking

    player->SetHungerState(newState);
    ApplyStateAura(player, newState);

    switch (newState)
    {
        case HungerState::Thriving:

            ApplyThriving(player);

            break;

        case HungerState::WellFed:

            ApplyWellFed(player);

            break;

        case HungerState::Hungry:

            ApplyHungry(player);

            break;

        case HungerState::Starving:

            ApplyStarving(player);

            break;

        default:

            break;
    }
}

void HungerSystem::RemoveHungerBuffs(Player* player, HungerState state)
{
    switch (state)
    {
        case HungerState::Thriving:

            RemoveThriving(player);

            break;

        case HungerState::WellFed:

            RemoveWellFed(player);

            break;

        case HungerState::Hungry:

            RemoveHungry(player);

            break;

        case HungerState::Starving:

            RemoveStarving(player);

            break;

        default:

            break;
    }

    //player->GetSession()->SendNotification("Removing Hunger Buffs");
}

void HungerSystem::ApplyStatModifier(Player* player, UnitMods stat, float percent, bool apply)
{
    player->HandleStatModifier(stat, TOTAL_PCT, percent, apply);
}

float thrivingPercentage = 15.0f; // 10% increase for Thriving
float wellFedPercentage = 10.0f;   // 5% increase for Well Fed
float hungryPercentage = -10.0f;   // 5% decrease for Hungry
float starvingPercentage = -15.0f; // 10% decrease for Starving

void HungerSystem::ApplyThriving(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, thrivingPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, thrivingPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, thrivingPercentage, true);
}

void HungerSystem::RemoveThriving(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, thrivingPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, thrivingPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, thrivingPercentage, false);
}

void HungerSystem::ApplyWellFed(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, wellFedPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, wellFedPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, wellFedPercentage, true);
}

void HungerSystem::RemoveWellFed(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, wellFedPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, wellFedPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, wellFedPercentage, false);
}

void HungerSystem::ApplyHungry(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, hungryPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, hungryPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, hungryPercentage, true);
}

void HungerSystem::RemoveHungry(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, hungryPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, hungryPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, hungryPercentage, false);
}

void HungerSystem::ApplyStarving(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, starvingPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, starvingPercentage, true);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, starvingPercentage, true);
}

void HungerSystem::RemoveStarving(Player* player)
{
    ApplyStatModifier(player, UNIT_MOD_STAT_STRENGTH, starvingPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_STAMINA, starvingPercentage, false);
    ApplyStatModifier(player, UNIT_MOD_STAT_SPIRIT, starvingPercentage, false);
}

void HungerSystem::RemoveStateAuras(Player* player)
{
    player->RemoveAurasDueToSpell(SPELL_HUNGER_THRIVING);
    player->RemoveAurasDueToSpell(SPELL_HUNGER_WELL_FED);
    player->RemoveAurasDueToSpell(SPELL_HUNGER_HUNGRY);
    player->RemoveAurasDueToSpell(SPELL_HUNGER_STARVING);
}

void HungerSystem::ApplyStateAura(Player* player, HungerState state)
{
    RemoveStateAuras(player);

    switch (state)
    {
        case HungerState::Thriving:

            player->CastSpell(player, SPELL_HUNGER_THRIVING, VITALITY_TRIGGER_FLAGS);
            break;

        case HungerState::WellFed:

            player->CastSpell(player, SPELL_HUNGER_WELL_FED, VITALITY_TRIGGER_FLAGS);
            break;

        case HungerState::Hungry:

            player->CastSpell(player, SPELL_HUNGER_HUNGRY, VITALITY_TRIGGER_FLAGS);
            break;

        case HungerState::Starving:

            player->CastSpell(player, SPELL_HUNGER_STARVING, VITALITY_TRIGGER_FLAGS);
            break;

        default:
            break;
    }

    /*
    if (state == HungerState::WellFed)
    {
        SpellCastResult result = player->CastSpell(player, SPELL_HUNGER_WELL_FED, VITALITY_TRIGGER_FLAGS);
        sLog.outString("Hunger spell cast result = %u", result);
    }
    */
}