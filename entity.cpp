
#include "entity.h"

void *monster_template_t::instantiate()
{
    monster_t *monster;
    monster = (monster_t *) malloc(sizeof(monster_t));
    monster->symbol = symbol;
    monster->color = color;
    monster->data = abilities;
    monster->speed = speed.toss();
    monster->hitpoints = hitpoints.toss();
    monster->known_location.xpos = 0;
    monster->known_location.ypos = 0;
    return monster;
}

void *item_template_t::instantiate()
{
    item_t *item;
    item = (item_t *) malloc(sizeof(item_t));
    item->symbol = symbol;
    item->color = color;
    item->speed_bonus = speed_bonus.toss();
    item->attributes = attributes.toss();
    item->hitpoint_bonus = hitpoint_bonus.toss();
    item->dodge_bonus = dodge_bonus.toss();
    item->defence_bonus = defence_bonus.toss();
    item->weight = weight.toss();
    item->value = value.toss();
    return item;
}
