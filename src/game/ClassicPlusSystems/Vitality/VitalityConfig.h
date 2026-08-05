/*

*/

#ifndef VITALITY_CONFIG_H
#define VITALITY_CONFIG_H

// -------------------- Hunger --------------------

constexpr uint8 MAX_HUNGER = 100;
constexpr uint8 START_HUNGER = 50;

constexpr uint32 HUNGER_UPDATE_MS = 10000; // in milliseconds

constexpr uint8 THRIVING_THRESHOLD = 85;
constexpr uint8 WELLFED_THRESHOLD = 60;
constexpr uint8 NORMAL_THRESHOLD = 40;
constexpr uint8 HUNGRY_THRESHOLD = 15;

// Meal rewards

constexpr uint8 FULL_MEAL_GAIN = 20;

constexpr uint32 MEAL_10_PERCENT = 10; // in milliseconds
constexpr uint32 MEAL_30_PERCENT = 30; // in milliseconds
constexpr uint32 MEAL_60_PERCENT = 60; // in milliseconds
constexpr uint32 MEAL_FULL = 100; // in milliseconds

#endif