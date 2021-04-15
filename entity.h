#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>

#include "vertex.h"
#include "dice.h"

class entity_t {
    public:
        uint8_t xpos;
        uint8_t ypos;
        char symbol;
        uint8_t color;
};

class monster_template_t {
    public:
        std::string name;
        char symbol;
        uint8_t color;
        std::vector<std::string> description;
        Dice_t speed;
        uint16_t abilities;
        Dice_t hitpoints;
        Dice_t damage;
        uint8_t rarity;

        void *instantiate();
};

class character_t : public entity_t {
    public:
        monster_template_t *entry;
        uint16_t data;
        uint16_t sequence;
        uint16_t speed;
        int hitpoints;
};

class monster_t : public character_t {
    public:
        vertex_t known_location;
};

class item_template_t {
    public:
        std::string name;
        char symbol;
        uint8_t color;
        std::vector<std::string> description;
        Dice_t speed_bonus;
        Dice_t attributes;
        uint8_t data;
        Dice_t hitpoint_bonus;
        Dice_t damage_bonus;
        Dice_t dodge_bonus;
        Dice_t defence_bonus;
        Dice_t weight;
        Dice_t value;
        uint8_t rarity;

        void *instantiate();
};

class item_t : public entity_t {
    public:
        item_template_t *entry;
        uint32_t speed_bonus;
        uint32_t attributes;
        uint32_t hitpoint_bonus;
        uint32_t dodge_bonus;
        uint32_t defence_bonus;
        uint32_t weight;
        uint32_t value;
};

#endif
