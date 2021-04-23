#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <endian.h>

#include <ncurses.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "vertex.h"
#include "entity.h"
#include "stack.h"
#include "heap.h"
#include "dice.h"

#define W_WIDTH 80
#define W_HEIGHT 21

/* Arguments */
#define SAVE_DUNGEON 0x01
#define LOAD_DUNGEON 0x02
#define NUM_MON 0x04

/* Monster */
#define SMART 0x0001
#define TELE 0x0002
#define TUNNEL 0x0004
#define ERRATIC 0x0008
#define PASS 0x0010
#define PICKUP 0x0020
#define DESTROY 0x0040
#define UNIQ 0x0080
#define MONSTER_EXISTS 0x0100
#define BOSS 0x0200

/* Used for Logic */
#define KNOWN 0x0400
#define SEE 0x0800

/* Item */
#define RELIC 0x01
#define ITEM_EXISTS 0x02

/* Player */
#define QUIT 0x0001
#define WALK_DOWN 0x0002
#define WALK_UP 0x0004
#define RESET 0x0008
#define MON_LIST 0x0010
#define ESCAPE 0x0020
#define FOG 0x0040
#define TELEPORT 0x0080
#define KILL_BOSS 0x0100
#define INVENTORY 0x0200
#define EQUIPMENT 0x0400
#define LOOK 0x0800
#define SAVE 0x1000
#define LOAD 0x2000

/* Equipment Slots */
#define WEAPON 0
#define OFFHAND 1
#define RANGED 2
#define AMMO 3
#define ARMOR 4
#define HELMET 5
#define CLOAK 6
#define GLOVES 7
#define BOOTS 8
#define AMULET 9
#define LIGHT 10
#define RING1 11
#define RING2 12
#define OTHER 13

class room_t {
    public:
        uint8_t xpos;
        uint8_t ypos;
        uint8_t xsize;
        uint8_t ysize;
};

class custom_vector_t {
    public:
        int xpos;
        int ypos;
};

/* Initializations */
void init_ncurses();
void deinit_ncurses();

void init_map(char map[W_HEIGHT][W_WIDTH], char c);
void init_entity_map(entity_t *character_map[W_HEIGHT][W_WIDTH]);
void init_item_map(std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH]);

void init_player_template(character_template_t *player_template);
int load_monster_dictionary(std::vector<character_template_t> *monster_dictionary);
int load_item_dictionary(std::vector<item_template_t> *item_dictionary);

void get_args (
        int argc,
        char *argv[],
        uint8_t *args,
        uint16_t *num_init_monsters
);

/* Generating Terrain */
void generate (
        character_t **player,
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint16_t *num_rooms,
        room_t **rooms,
        uint16_t *num_u_stairs,
        entity_t **u_stairs,
        uint16_t *num_d_stairs,
        entity_t **d_stairs
);

void save_dungeon (
        character_t *player,
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint16_t num_rooms,
        room_t *rooms,
        uint16_t num_u_stairs,
        entity_t *u_stairs,
        uint16_t num_d_stairs,
        entity_t *d_stairs
);

void load_dungeon (
        character_t **player,
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint16_t *num_rooms,
        room_t **rooms,
        uint16_t *num_u_stairs,
        entity_t **u_stairs,
        uint16_t *num_d_stairs,
        entity_t **d_stairs
);

void map_rooms (
        char map[W_HEIGHT][W_WIDTH],
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint16_t num_rooms,
        room_t *rooms
);

/* Saving and Loading */

void save(
    uint8_t hardness[W_HEIGHT][W_WIDTH],
    char map[W_HEIGHT][W_WIDTH],
    char visible_map[W_HEIGHT][W_WIDTH],
    uint16_t num_rooms,
    room_t *rooms,
    uint16_t num_u_stairs,
    entity_t *u_stairs,
    uint16_t num_d_stairs,
    entity_t *d_stairs,
    uint32_t priority,
    uint16_t num_init_monsters,
    uint16_t num_monsters,
    uint16_t num_init_items,
    std::vector<character_template_t> *monster_dictionary,
    std::vector<item_template_t> *item_dictionary,
    character_t *player,
    heap_t *characters,
    heap_t *next_turn,
    std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH]
);

void load(
    uint8_t hardness[W_HEIGHT][W_WIDTH],
    char map[W_HEIGHT][W_WIDTH],
    char visible_map[W_HEIGHT][W_WIDTH],
    uint16_t *num_rooms,
    room_t **rooms,
    uint16_t *num_u_stairs,
    entity_t **u_stairs,
    uint16_t *num_d_stairs,
    entity_t **d_stairs,
    uint32_t *priority,
    uint16_t *num_init_monsters,
    uint16_t *num_monsters,
    uint16_t *num_init_items,
    std::vector<character_template_t> *monster_dictionary,
    std::vector<item_template_t> *item_dictionary,
    character_t *player,
    heap_t *characters,
    heap_t *next_turn,
    std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH]
);

/* Generating Entities */
void init_player(
        heap_t *characters,
        character_t *character_map[W_HEIGHT][W_WIDTH],
        character_t *player,
        character_template_t *player_template
);

void generate_monsters(
        std::vector<character_template_t> *monster_dictionary,
        heap_t *characters,
        character_t *character_map[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        uint16_t num_init_monsters,
        stack_t *monster_carry
);

void generate_items(
        std::vector<item_template_t> *item_dictionary,
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        uint16_t num_init_items,
        stack_t *item_carry
);

/* Graph Related Things */
void dijkstra(
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        character_t *player,
        uint16_t distance[W_HEIGHT][W_WIDTH],
        uint8_t type
);

void get_neighbors(
        stack_t *s,
        vertex_t *v,
        char map[W_HEIGHT][W_WIDTH],
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint8_t tunnel
);

/* Turn Mechanisms */
void derive_next_turn_heap(
        heap_t *characters,
        heap_t *next_turn,
        uint32_t *priority
);

void derive_move_stack(
        stack_t *move_stack,
        char map[W_HEIGHT][W_WIDTH],
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        character_t *character,
        vertex_t *hither,
        vertex_t **thither
);

void player_command(
        character_t *player,
        vertex_t *thither,
        char visible_map[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        uint16_t *num_monsters
);

void monster_turn(
        stack_t *move_stack,
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        monster_t *character,
        character_t *player,
        char map[W_HEIGHT][W_WIDTH],
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        uint16_t floor_distance[W_HEIGHT][W_WIDTH],
        uint16_t tunnel_distance[W_HEIGHT][W_WIDTH],
        uint16_t pass_distance[W_HEIGHT][W_WIDTH],
        vertex_t *hither,
        vertex_t **thither,
        uint16_t *num_monsters
);

void line_of_sight(
        monster_t *character,
        character_t *player,
        char map[W_HEIGHT][W_WIDTH]
);

/* Movemnet */
void smart_move(
        stack_t *move_stack,
        character_t *character,
        vertex_t **thither,
        uint16_t distance[W_HEIGHT][W_WIDTH]
);

void dumb_move(
        stack_t *move_stack,
        character_t *character,
        character_t *player,
        vertex_t *hither,
        vertex_t **thither
);

void random_move(
        stack_t *move_stack,
        vertex_t **thither
);

void tunnel(
        uint8_t hardness[W_HEIGHT][W_WIDTH],
        character_t *player,
        uint16_t floor_distance[W_HEIGHT][W_WIDTH],
        uint16_t tunnel_distance[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        vertex_t *thither
);

void move_character(
        character_t *player,
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        character_t *character,
        vertex_t *hither,
        vertex_t *thither,
        uint16_t *num_monsters
);

/* Ranged Combat */
int shoot(
        character_t *player,
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        character_t *shooter,
        character_t *shootee,
        uint16_t *num_monsters
);

/* Special Commands */
std::string equipment_num_to_slot (
        int num
);

void draw_inventory (
        character_t *player
);

void draw_equipment (
        character_t *player
);

void prompt_inventory (
        character_t *player,
        uint8_t *i
);

void prompt_equipment (
        character_t *player,
        uint8_t *i
);

void display_item(
        character_t *player,
        item_t *item
);

void display_character(
        character_t *player,
        character_t *character
);

void prompt_place(
        character_t *player,
        vertex_t *thither,
        char map[W_HEIGHT][W_WIDTH],
        char visible_map[W_HEIGHT][W_WIDTH],
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        const char c
);

void monster_list(
        character_t *character_map[W_HEIGHT][W_WIDTH],
        character_t *player,
        uint16_t num_init_monsters
);

void monster_list_command(
        character_t *player
);

/* Mapping */
void place(
        char map[W_HEIGHT][W_WIDTH],
        entity_t *entities,
        uint16_t count
);

void place_characters(
        char map[W_HEIGHT][W_WIDTH],
        character_t *characters[W_HEIGHT][W_WIDTH]
);

void sketch_visible_map(
        char visible_map[W_HEIGHT][W_WIDTH],
        char map[W_HEIGHT][W_WIDTH],
        character_t *player
);

/* Debugging */
void print_monster_dictionary(std::vector<character_template_t> monster_dictionary);
void print_item_dictionary(std::vector<item_template_t> item_dictionary);
void draw_distance( uint16_t distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH]);
void print_message(const char *str);

/* Drawing */
void draw_fog(
        char map[W_HEIGHT][W_WIDTH],
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH],
        character_t *player
);

void draw_full(
        char map[W_HEIGHT][W_WIDTH],
        character_t *character_map[W_HEIGHT][W_WIDTH],
        std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH]
);

void draw_quit();
void draw_win();
void draw_loose();

int main(int argc, char *argv[])
{
    /* Initializing Ncurses */
    init_ncurses();

    /* Seeding */
    srand(time(NULL));

    /* Declarations */
    uint8_t args = 0x00;
    uint8_t i, j;
    uint8_t hardness[W_HEIGHT][W_WIDTH];
    uint16_t floor_distance[W_HEIGHT][W_WIDTH];
    uint16_t tunnel_distance[W_HEIGHT][W_WIDTH];
    uint16_t pass_distance[W_HEIGHT][W_WIDTH];
    room_t *rooms;
    uint16_t num_rooms;
    entity_t *u_stairs;
    uint16_t num_u_stairs;
    entity_t *d_stairs;
    uint16_t num_d_stairs;
    character_template_t player_template;
    character_t *player;
    std::vector<character_template_t> monster_dictionary;
    character_t *character;
    heap_t characters;
    character_t *character_map[W_HEIGHT][W_WIDTH];
    stack_t monster_carry;
    uint16_t num_init_monsters;
    uint16_t num_monsters;
    std::vector<item_template_t> item_dictionary;
    item_t *item;
    std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH];
    stack_t item_carry;
    uint16_t num_init_items;
    character_t *dummy;
    heap_t next_turn;
    uint32_t priority;
    vertex_t hither;
    vertex_t *thither;
    stack_t move_stack;
    char map[W_HEIGHT][W_WIDTH];
    char visible_map[W_HEIGHT][W_WIDTH];

    /* Switch Processing */
    get_args(argc, argv, &args, &num_init_monsters);

    /* Saving and Loading */
    if (args & LOAD_DUNGEON)
        load_dungeon(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    else
        generate(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    if (args & SAVE_DUNGEON)
        save_dungeon(player, hardness, num_rooms, rooms, num_u_stairs, u_stairs, num_d_stairs, d_stairs);

    /* Sketching Map */
    init_map(map, ' ');
    init_map(visible_map, ' ');
    map_rooms(map, hardness, num_rooms, rooms);
    place(map, u_stairs, num_u_stairs);
    place(map, d_stairs, num_d_stairs);

    /* Generating Characters */
    init_player_template(&player_template);
    player->entry = &player_template;
    heap_init(&characters);
    init_entity_map((entity_t *(*)[80]) character_map);
    init_player(&characters, character_map, player, &player_template);
    stack_init(&monster_carry);
    if (load_monster_dictionary(&monster_dictionary)) {
        deinit_ncurses();
        return 1;
    }
    if (!(args & NUM_MON))
        num_init_monsters = 10;
    num_init_monsters = num_init_monsters < 300 ? num_init_monsters : 300;
    generate_monsters(&monster_dictionary, &characters, character_map, map, num_init_monsters, &monster_carry);
    num_monsters = num_init_monsters;

    /* Generating Items */
    init_item_map(item_map);
    stack_init(&item_carry);
    if (load_item_dictionary(&item_dictionary)) {
        deinit_ncurses();
        return 2;
    }
    num_init_items = 10;
    generate_items(&item_dictionary, item_map, map, num_init_items, &item_carry);

    /* Finding Initial Distances */
    dijkstra(hardness, map, player, floor_distance, 0);
    dijkstra(hardness, map, player, tunnel_distance, 1);
    dijkstra(hardness, map, player, pass_distance, 2);

    /* Main Loop */
    while (player->hitpoints >= 0 && !(player->data & KILL_BOSS) && !(player->data & QUIT)) {
        derive_next_turn_heap(&characters, &next_turn, &priority);
        /* Character Turn */
        while (heap_size(&next_turn) > 0 && !(player->data & RESET)) {
            player->data &= ~RESET;
            heap_extract_min(&next_turn, (void**) &character);
            if (character->hitpoints >= 0) {
                if (character == player) {
                    do {
                        player->data &= ~MON_LIST;
                        player->data &= ~TELEPORT;
                        player->data &= ~SAVE;
                        player->data &= ~LOAD;
                        sketch_visible_map(visible_map, map, player);
                        if (player->data & FOG)
                            draw_fog(visible_map, character_map, item_map, player);
                        else
                            draw_full(map, character_map, item_map);
                        refresh();
                        hither.xpos = player->xpos;
                        hither.ypos = player->ypos;
                        thither = (vertex_t *) malloc(sizeof(vertex_t));
                        player_command(player, thither, visible_map, map, character_map, item_map, &num_monsters);
                        if (map[thither->ypos][thither->xpos] != ' ') {
                            move_character(player, character_map, item_map, character, &hither, thither, &num_monsters);
                        }
                        dijkstra(hardness, map, player, floor_distance, 0);
                        dijkstra(hardness, map, player, tunnel_distance, 1);
                        dijkstra(hardness, map, player, pass_distance, 2);
                        free(thither);
                        if (player->data & MON_LIST) {
                            monster_list(character_map, player, num_monsters);
                        }
                        if ((player->data & WALK_UP && player->xpos == u_stairs->xpos && player->ypos == u_stairs->ypos) ||
                            (player->data & WALK_DOWN && player->xpos == d_stairs->xpos && player->ypos == d_stairs->ypos)) {
                            /* new floor */
                            free(rooms);
                            free(u_stairs);
                            free(d_stairs);
                            while (heap_size(&characters)) {
                                heap_extract_min(&characters, (void **) &character);
                                if (character->data & (UNIQ | BOSS) && character->hitpoints >= 0)
                                    stack_push(&monster_carry, character);
                                else {
                                    for (i = 0; i < 10; i++) {
                                        if (character->inventory[i] != NULL) {
                                            free(character->inventory[i]);
                                            character->inventory[i] = NULL;
                                        }
                                    }
                                    free(character->inventory);
                                    for (i = 0; i < OTHER; i++) {
                                        if (character->equipment[i] != NULL) {
                                            free(character->equipment[i]);
                                            character->equipment[i] = NULL;
                                        }
                                    }
                                    free(character->equipment);
                                    free(character);
                                }
                            }
                            while (heap_size(&next_turn)) {
                                heap_extract_min(&next_turn, (void **) &character);
                                if (character->data & (UNIQ | BOSS) && character->hitpoints >= 0)
                                    stack_push(&monster_carry, character);
                                else {
                                    for (i = 0; i < 10; i++) {
                                        if (character->inventory[i] != NULL) {
                                            free(character->inventory[i]);
                                            character->inventory[i] = NULL;
                                        }
                                    }
                                    free(character->inventory);
                                    for (i = 0; i < OTHER; i++) {
                                        if (character->equipment[i] != NULL) {
                                            free(character->equipment[i]);
                                            character->equipment[i] = NULL;
                                        }
                                    }
                                    free(character->equipment);
                                    free(character);
                                }
                            }
                            for (i = 0; i < W_HEIGHT; i++) {
                                for (j = 0; j < W_WIDTH; j++) {
                                    while (item_map[i][j].size() > 0) {
                                        item = item_map[i][j].back();
                                        item_map[i][j].pop_back();
                                        if (item->entry->data & RELIC)
                                            stack_push(&item_carry, item);
                                        else
                                            free(item);
                                    }
                                }
                            }
                            generate(&dummy, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
                            player->xpos = dummy->xpos;
                            player->ypos = dummy->ypos;
                            player->data &= ~WALK_UP;
                            player->data &= ~WALK_DOWN;
                            free(dummy);
                            init_map(map, ' ');
                            init_map(visible_map, ' ');
                            map_rooms(map, hardness, num_rooms, rooms);
                            place(map, u_stairs, num_u_stairs);
                            place(map, d_stairs, num_d_stairs);
                            init_entity_map((entity_t *(*)[80]) character_map);
                            init_item_map(item_map);
                            heap_add(&characters, player, 0);
                            character_map[player->ypos][player->xpos] = player;
                            generate_monsters(&monster_dictionary, &characters, character_map, map, num_init_monsters, &monster_carry);
                            generate_items(&item_dictionary, item_map, map, num_init_items, &item_carry);
                            num_monsters = num_init_monsters;
                            dijkstra(hardness, map, player, floor_distance, 0);
                            dijkstra(hardness, map, player, tunnel_distance, 1);
                            dijkstra(hardness, map, player, pass_distance, 2);
                        }
                        else if (!(player->data & (MON_LIST | TELEPORT | SAVE | LOAD) && !(player->data & QUIT))){
                            heap_add(&characters, character, priority + 1000 / character->speed);
                        }
                        if (player->data & TELEPORT) {
                            thither = (vertex_t *) malloc(sizeof(vertex_t));
                            thither->ypos = player->ypos;
                            thither->xpos = player->xpos;
                            prompt_place(player, thither, map, visible_map, character_map, item_map, 'g');
                            if (!(player->data & ESCAPE)) {
                                move_character(player, character_map, item_map, character, &hither, thither, &num_monsters);
                            }
                            free(thither);
                        }
                        if (player->data & SAVE) {
                            save(hardness, map, visible_map, num_rooms, rooms, num_u_stairs, u_stairs, num_d_stairs, d_stairs,
                                    priority, num_init_monsters, num_monsters, num_init_items,
                                    &monster_dictionary, &item_dictionary, player, &characters, &next_turn, item_map);
                        }
                        if (player->data & LOAD) {
                            free(rooms);
                            free(u_stairs);
                            free(d_stairs);
                            while (monster_dictionary.size() > 0)
                                monster_dictionary.pop_back();
                            while (item_dictionary.size() > 0)
                                item_dictionary.pop_back();
                            for (i = 0; i < 10; i++) {
                                if (player->inventory[i] != NULL) {
                                    free(player->inventory[i]);
                                    player->inventory[i] = NULL;
                                }
                            }
                            for (i = 0; i < OTHER; i++) {
                                if (player->equipment[i] != NULL) {
                                    free(player->equipment[i]);
                                    player->equipment[i] = NULL;
                                }
                            }
                            for (i = 0; i < characters.size; i++) {
                                for (j = 0; j < 10; j++) {
                                    if (((character_t *) characters.nodes[i].value)->inventory[j] != NULL) {
                                        free(((character_t *) characters.nodes[i].value)->inventory[j]);
                                        ((character_t *) characters.nodes[i].value)->inventory[j] = NULL;
                                    }
                                }
                                for (j = 0; j < OTHER; j++) {
                                    if (((character_t *) characters.nodes[i].value)->equipment[j] != NULL) {
                                        free(((character_t *) characters.nodes[i].value)->equipment[j]);
                                        ((character_t *) characters.nodes[i].value)->equipment[j] = NULL;
                                    }
                                }
                            }
                            heap_delete(&characters);
                            for (j = 0; j < next_turn.size; j++) {
                                for (i = 0; i < 10; i++) {
                                    if (((character_t *) next_turn.nodes[i].value)->inventory[j] != NULL) {
                                        free(((character_t *) next_turn.nodes[i].value)->inventory[j]);
                                        ((character_t *) next_turn.nodes[i].value)->inventory[j] = NULL;
                                    }
                                }
                                for (j = 0; j < OTHER; j++) {
                                    if (((character_t *) next_turn.nodes[i].value)->equipment[j] != NULL) {
                                        free(((character_t *) next_turn.nodes[i].value)->equipment[j]);
                                        ((character_t *) next_turn.nodes[i].value)->equipment[j] = NULL;
                                    }
                                }
                            }
                            heap_delete(&next_turn);
                            for (i = 0; i < W_HEIGHT; i++) {
                                for (j = 0; j < W_WIDTH; j++) {
                                    while (item_map[i][j].size() > 0) {
                                        item = item_map[i][j].back();
                                        item_map[i][j].pop_back();
                                        free(item);
                                    }
                                }
                            }
                            for (i = 0; i < W_HEIGHT; i++) {
                                for (j = 0; j < W_WIDTH; j++) {
                                    character_map[i][j] = NULL;
                                }
                            }
                            load(hardness, map, visible_map, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs,
                                    &priority, &num_init_monsters, &num_monsters, &num_init_items,
                                    &monster_dictionary, &item_dictionary, player, &characters, &next_turn, item_map);
                            character_map[player->ypos][player->xpos] = player;
                            for (i = 0; i < characters.size; i++) {
                                character_map[((character_t *) characters.nodes[i].value)->ypos][((character_t *) characters.nodes[i].value)->xpos] = (character_t *) characters.nodes[i].value;
                            }
                            for (i = 0; i < next_turn.size; i++) {
                                character_map[((character_t *) next_turn.nodes[i].value)->ypos][((character_t *) next_turn.nodes[i].value)->xpos] = (character_t *) next_turn.nodes[i].value;
                            }
                            sketch_visible_map(visible_map, map, player);
                            if (player->data & FOG)
                                draw_fog(visible_map, character_map, item_map, player);
                            else
                                draw_full(map, character_map, item_map);
                            refresh();
                            player->data |= LOAD;
                            player->entry = &player_template;
                        }
                    } while (player->data & (MON_LIST | TELEPORT | SAVE | LOAD) && !(player->data & QUIT) && !(player->data & KILL_BOSS));
                }
                else {
                    heap_add(&characters, character, priority + 1000 / character->speed);
                    derive_move_stack(&move_stack, map, hardness, character, &hither, &thither);
                    monster_turn(&move_stack, character_map, item_map, (monster_t *) character,
                            player, map, hardness, floor_distance, tunnel_distance,
                            pass_distance, &hither, &thither, &num_monsters
                    );
                    sketch_visible_map(visible_map, map, player);
                    if (player->data & FOG)
                        draw_fog(visible_map, character_map, item_map, player);
                    else
                        draw_full(map, character_map, item_map);
                    refresh();
                    stack_delete(&move_stack);
                    if (item_map[thither->ypos][thither->xpos].size() > 0 && character->data & DESTROY) {
                        item = item_map[thither->ypos][thither->xpos].back();
                        item_map[thither->ypos][thither->xpos].pop_back();
                        free(item);
                    }
                    else if (item_map[thither->ypos][thither->xpos].size() > 0 && character->data & PICKUP && character->num_items < 10) {
                        item = item_map[thither->ypos][thither->xpos].back();
                        item_map[thither->ypos][thither->xpos].pop_back();
                        character->inventory[character->num_items] = item;
                        if (item->entry->type < RING1) {
                            if (character->equipment[item->entry->type] != NULL) {
                                character->hitpoints -= character->equipment[item->entry->type]->hitpoint_bonus;
                                character->speed -= character->equipment[item->entry->type]->speed_bonus;
                            }
                            character->inventory[character->num_items] = character->equipment[item->entry->type];
                            character->equipment[item->entry->type] = item;
                            character->hitpoints += item->hitpoint_bonus;
                            character->speed += item->speed_bonus;
                            if (character->inventory[character->num_items] == NULL)
                                character->num_items -= 1;
                        }
                        else if (item->entry->type < OTHER && character->equipment[RING1] == NULL) {
                            if (character->equipment[RING1] != NULL) {
                                character->hitpoints -= character->equipment[RING1]->hitpoint_bonus;
                                character->speed -= character->equipment[RING1]->speed_bonus;
                            }
                            character->inventory[character->num_items] = character->equipment[RING1];
                            character->equipment[RING1] = item;
                            character->hitpoints += item->hitpoint_bonus;
                            character->speed += item->speed_bonus;
                            if (character->inventory[character->num_items] == NULL)
                                character->num_items -= 1;
                        }
                        else if (item->entry->type < OTHER) {
                            if (character->equipment[RING2] != NULL) {
                                character->hitpoints -= character->equipment[RING2]->hitpoint_bonus;
                                character->speed -= character->equipment[RING2]->speed_bonus;
                            }
                            character->inventory[character->num_items] = character->equipment[RING2];
                            character->equipment[RING2] = item;
                            character->hitpoints += item->hitpoint_bonus;
                            character->speed += item->speed_bonus;
                            if (character->inventory[character->num_items] == NULL)
                                character->num_items -= 1;
                        }
                        character->num_items++;
                    }
                    free(thither);
                }
            }
            else {
                if (character_map[character->ypos][character->xpos] == character)
                    character_map[character->ypos][character->xpos] = NULL;
                for (i = 0; i < 10; i++) {
                    if (character->inventory[i] != NULL) {
                        free(character->inventory[i]);
                        character->inventory[i] = NULL;
                    }
                }
                free(character->inventory);
                for (i = 0; i < OTHER; i++) {
                    if (character->equipment[i] != NULL) {
                        free(character->equipment[i]);
                        character->equipment[i] = NULL;
                    }
                }
                free(character->equipment);
                free(character);
            }
        }
        heap_delete_characters(&next_turn);
    }

    /* Printing Results */
    clear();
    if (player->data & QUIT)
        draw_quit();
    else if (player->hitpoints >= 0)
        draw_win();
    else
        draw_loose();

    /* Cleaning Memory */
    free(rooms);
    free(u_stairs);
    free(d_stairs);
    heap_delete_characters(&characters);
    stack_delete_characters(&monster_carry);
    stack_delete(&item_carry);

    /* Waiting for a final character */
    getch();

    /* Deinitializing Ncurses */
    deinit_ncurses();

    return 0;
}

void init_ncurses()
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void deinit_ncurses() {
    endwin();
}

void init_map(char map[W_HEIGHT][W_WIDTH], char c)
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            map[i][j] = c;
        }
    }
}

void init_entity_map(entity_t *character_map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            character_map[i][j] = NULL;
        }
    }
}

void init_item_map(std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            while (!item_map[i][j].empty()) {
                item_map[i][j].pop_back();
            }
        }
    }
}

void init_player_template(character_template_t *player_template)
{
    player_template->name = "Player";
    player_template->damage.base = 0;
    player_template->damage.num = 1;
    player_template->damage.sides = 4;
}

int load_monster_dictionary(std::vector<character_template_t> *monster_dictionary)
{
    uint16_t i, j, check;
    character_template_t character_template;
    std::string s;
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *file = "monster_desc.txt";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(file) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, file);
    std::ifstream f(path);
    free(path);
    getline(f, s);
    if (s != "RLG327 MONSTER DESCRIPTION 1")
        return 1;
    do {
        getline(f, s);
        if (s == "BEGIN MONSTER") {
            check = 0x0000;
            character_template.name = "";
            do {
                f >> s;
                if (s == "NAME") {
                    check |= 0x0001;
                    f >> s;
                    character_template.name += s;
                    while (f.peek() != '\n') {
                        f >> s;
                        character_template.name += " ";
                        character_template.name += s;
                    }
                }
                else if (s == "SYMB") {
                    check |= 0x0002;
                    f >> s;
                    character_template.symbol = s[0];
                }
                else if (s == "COLOR") {
                    check |= 0x0004;
                    character_template.color = 0x00;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "BLACK")
                            character_template.color |= 0x01;
                        else if (s == "RED")
                            character_template.color |= 0x02;
                        else if (s == "GREEN")
                            character_template.color |= 0x04;
                        else if (s == "YELLOW")
                            character_template.color |= 0x08;
                        else if (s == "BLUE")
                            character_template.color |= 0x10;
                        else if (s == "MAGENTA")
                            character_template.color |= 0x20;
                        else if (s == "CYAN")
                            character_template.color |= 0x40;
                        else if (s == "WHITE")
                            character_template.color |= 0x80;
                        else
                            return 2;
                    }
                }
                else if (s == "DESC") {
                    check |= 0x0008;
                    std::vector<std::string> des;
                    for (j = 0; s != "."; j++) {
                        getline(f, s);
                        des.push_back(std::string(s));
                    }
                    character_template.description = des;
                }
                else if (s == "SPEED") {
                    check |= 0x0010;
                    f >> s;
                    i = 0;
                    character_template.speed.base = 0;
                    character_template.speed.num = 0;
                    character_template.speed.sides = 0;
                    while (s[i] != '+') {
                        character_template.speed.base *= 10;
                        character_template.speed.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        character_template.speed.num *= 10;
                        character_template.speed.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        character_template.speed.sides *= 10;
                        character_template.speed.sides += s[i++] - '0';
                    }
                }
                else if (s == "DAM") {
                    check |= 0x0020;
                    f >> s;
                    i = 0;
                    character_template.damage.base = 0;
                    character_template.damage.num = 0;
                    character_template.damage.sides = 0;
                    while (s[i] != '+') {
                        character_template.damage.base *= 10;
                        character_template.damage.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        character_template.damage.num *= 10;
                        character_template.damage.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        character_template.damage.sides *= 10;
                        character_template.damage.sides += s[i++] - '0';
                    }
                }
                else if (s == "HP") {
                    check |= 0x0040;
                    f >> s;
                    i = 0;
                    character_template.hitpoints.base = 0;
                    character_template.hitpoints.num = 0;
                    character_template.hitpoints.sides = 0;
                    while (s[i] != '+') {
                        character_template.hitpoints.base *= 10;
                        character_template.hitpoints.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        character_template.hitpoints.num *= 10;
                        character_template.hitpoints.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        character_template.hitpoints.sides *= 10;
                        character_template.hitpoints.sides += s[i++] - '0';
                    }
                }
                else if (s == "ABIL") {
                    check |= 0x0080;
                    character_template.abilities = 0x0000;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "SMART")
                            character_template.abilities |= SMART;
                        else if (s == "TELE")
                            character_template.abilities |= TELE;
                        else if (s == "TUNNEL")
                            character_template.abilities |= TUNNEL;
                        else if (s == "ERRATIC")
                            character_template.abilities |= ERRATIC;
                        else if (s == "PASS")
                            character_template.abilities |= PASS;
                        else if (s == "PICKUP")
                            character_template.abilities |= PICKUP;
                        else if (s == "DESTROY")
                            character_template.abilities |= DESTROY;
                        else if (s == "UNIQ")
                            character_template.abilities |= UNIQ;
                        else if (s == "BOSS")
                            character_template.abilities |= BOSS;
                        else
                            return 3;
                    }
                }
                else if (s == "RRTY") {
                    check |= 0x0100;
                    f >> s;
                    i = 0;
                    character_template.rarity = 0;
                    while (s[i] != '\0') {
                        character_template.rarity *= 10;
                        character_template.rarity += s[i++] - '0';
                    }
                }
                else if (s != "END") {
                    std::cout << s << std::endl;
                    return 4;
                }
            } while (s != "END");
            if (check != 0x01FF) {
                return 5;
            }
            (*monster_dictionary).push_back(character_template);
        }
    } while (!f.eof());
    return 0;
}

int load_item_dictionary(std::vector<item_template_t> *item_dictionary)
{
    uint16_t i, j, check;
    item_template_t item_template;
    std::string s;
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *file = "object_desc.txt";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(file) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, file);
    std::ifstream f(path);
    free(path);
    getline(f, s);
    if (s != "RLG327 OBJECT DESCRIPTION 1")
        return 1;
    do {
        getline(f, s);
        if (s == "BEGIN OBJECT") {
            check = 0x0000;
            item_template.name = "";
            item_template.light.base = 0;
            item_template.light.num = 0;
            item_template.light.sides = 0;
            do {
                f >> s;
                if (s == "NAME") {
                    check |= 0x0001;
                    f >> s;
                    item_template.name += s;
                    while (f.peek() != '\n') {
                        f >> s;
                        item_template.name += " ";
                        item_template.name += s;
                    }
                }
                else if (s == "TYPE") {
                    check |= 0x0002;
                    f >> s;
                    if (s == "WEAPON") {
                        item_template.symbol = '|';
                        item_template.type = WEAPON;
                    }
                    else if (s == "OFFHAND") {
                        item_template.symbol = ')';
                        item_template.type = OFFHAND;
                    }
                    else if (s == "RANGED") {
                        item_template.symbol = '}';
                        item_template.type = RANGED;
                    }
                    else if (s == "ARMOR") {
                        item_template.symbol = '[';
                        item_template.type = ARMOR;
                    }
                    else if (s == "HELMET") {
                        item_template.symbol = ']';
                        item_template.type = HELMET;
                    }
                    else if (s == "CLOAK") {
                        item_template.symbol = '(';
                        item_template.type = CLOAK;
                    }
                    else if (s == "GLOVES") {
                        item_template.symbol = '{';
                        item_template.type = GLOVES;
                    }
                    else if (s == "BOOTS") {
                        item_template.symbol = '\\';
                        item_template.type = BOOTS;
                    }
                    else if (s == "RING") {
                        item_template.symbol = '=';
                        item_template.type = RING1;
                    }
                    else if (s == "AMULET") {
                        item_template.symbol = '"';
                        item_template.type = AMULET;
                    }
                    else if (s == "LIGHT") {
                        item_template.symbol = '_';
                        item_template.type = LIGHT;
                    }
                    else if (s == "SCROLL") {
                        item_template.symbol = '~';
                        item_template.type = OTHER;
                    }
                    else if (s == "BOOK") {
                        item_template.symbol = '?';
                        item_template.type = OTHER;
                    }
                    else if (s == "FLASK") {
                        item_template.symbol = '!';
                        item_template.type = OTHER;
                    }
                    else if (s == "GOLD") {
                        item_template.symbol = '$';
                        item_template.type = OTHER;
                    }
                    else if (s == "AMMUNITION") {
                        item_template.symbol = '/';
                        item_template.type = AMMO;
                    }
                    else if (s == "FOOD") {
                        item_template.symbol = ',';
                        item_template.type = OTHER;
                    }
                    else if (s == "WAND") {
                        item_template.symbol = '-';
                        item_template.type = OTHER;
                    }
                    else if (s == "CONTAINER") {
                        item_template.symbol = '%';
                        item_template.type = OTHER;
                    }
                    else {
                        item_template.symbol = '*';
                        item_template.type = OTHER;
                    }
                }
                else if (s == "COLOR") {
                    check |= 0x0004;
                    item_template.color = 0x00;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "BLACK")
                            item_template.color |= 0x01;
                        else if (s == "RED")
                            item_template.color |= 0x02;
                        else if (s == "GREEN")
                            item_template.color |= 0x04;
                        else if (s == "YELLOW")
                            item_template.color |= 0x08;
                        else if (s == "BLUE")
                            item_template.color |= 0x10;
                        else if (s == "MAGENTA")
                            item_template.color |= 0x20;
                        else if (s == "CYAN")
                            item_template.color |= 0x40;
                        else if (s == "WHITE")
                            item_template.color |= 0x80;
                        else
                            return 2;
                    }
                }
                else if (s == "DESC") {
                    check |= 0x0008;
                    std::vector<std::string> des;
                    for (j = 0; s != "."; j++) {
                        getline(f, s);
                        des.push_back(std::string(s));
                    }
                    item_template.description = des;
                }
                else if (s == "SPEED") {
                    check |= 0x0010;
                    f >> s;
                    i = 0;
                    item_template.speed_bonus.base = 0;
                    item_template.speed_bonus.num = 0;
                    item_template.speed_bonus.sides = 0;
                    while (s[i] != '+') {
                        item_template.speed_bonus.base *= 10;
                        item_template.speed_bonus.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.speed_bonus.num *= 10;
                        item_template.speed_bonus.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.speed_bonus.sides *= 10;
                        item_template.speed_bonus.sides += s[i++] - '0';
                    }
                }
                else if (s == "ATTR") {
                    check |= 0x0020;
                    f >> s;
                    i = 0;
                    item_template.attributes.base = 0;
                    item_template.attributes.num = 0;
                    item_template.attributes.sides = 0;
                    while (s[i] != '+') {
                        item_template.attributes.base *= 10;
                        item_template.attributes.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.attributes.num *= 10;
                        item_template.attributes.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.attributes.sides *= 10;
                        item_template.attributes.sides += s[i++] - '0';
                    }
                }
                else if (s == "ART") {
                    check |= 0x0040;
                    item_template.data = 0x00;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "TRUE")
                            item_template.data |= RELIC;
                        else if (s == "FALSE")
                            item_template.data &= ~RELIC;
                        else
                            return 3;
                    }
                }
                else if (s == "HIT") {
                    check |= 0x0080;
                    f >> s;
                    i = 0;
                    item_template.hitpoint_bonus.base = 0;
                    item_template.hitpoint_bonus.num = 0;
                    item_template.hitpoint_bonus.sides = 0;
                    while (s[i] != '+') {
                        item_template.hitpoint_bonus.base *= 10;
                        item_template.hitpoint_bonus.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.hitpoint_bonus.num *= 10;
                        item_template.hitpoint_bonus.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.hitpoint_bonus.sides *= 10;
                        item_template.hitpoint_bonus.sides += s[i++] - '0';
                    }
                }
                else if (s == "DAM") {
                    check |= 0x0100;
                    f >> s;
                    i = 0;
                    item_template.damage_bonus.base = 0;
                    item_template.damage_bonus.num = 0;
                    item_template.damage_bonus.sides = 0;
                    while (s[i] != '+') {
                        item_template.damage_bonus.base *= 10;
                        item_template.damage_bonus.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.damage_bonus.num *= 10;
                        item_template.damage_bonus.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.damage_bonus.sides *= 10;
                        item_template.damage_bonus.sides += s[i++] - '0';
                    }
                }
                else if (s == "DODGE") {
                    check |= 0x0200;
                    f >> s;
                    i = 0;
                    item_template.dodge_bonus.base = 0;
                    item_template.dodge_bonus.num = 0;
                    item_template.dodge_bonus.sides = 0;
                    while (s[i] != '+') {
                        item_template.dodge_bonus.base *= 10;
                        item_template.dodge_bonus.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.dodge_bonus.num *= 10;
                        item_template.dodge_bonus.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.dodge_bonus.sides *= 10;
                        item_template.dodge_bonus.sides += s[i++] - '0';
                    }
                }
                else if (s == "DEF") {
                    check |= 0x0400;
                    f >> s;
                    i = 0;
                    item_template.defence_bonus.base = 0;
                    item_template.defence_bonus.num = 0;
                    item_template.defence_bonus.sides = 0;
                    while (s[i] != '+') {
                        item_template.defence_bonus.base *= 10;
                        item_template.defence_bonus.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.defence_bonus.num *= 10;
                        item_template.defence_bonus.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.defence_bonus.sides *= 10;
                        item_template.defence_bonus.sides += s[i++] - '0';
                    }
                }
                else if (s == "WEIGHT") {
                    check |= 0x0800;
                    f >> s;
                    i = 0;
                    item_template.weight.base = 0;
                    item_template.weight.num = 0;
                    item_template.weight.sides = 0;
                    while (s[i] != '+') {
                        item_template.weight.base *= 10;
                        item_template.weight.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.weight.num *= 10;
                        item_template.weight.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.weight.sides *= 10;
                        item_template.weight.sides += s[i++] - '0';
                    }
                }
                else if (s == "LIT") {
                    f >> s;
                    i = 0;
                    item_template.light.base = 0;
                    item_template.light.num = 0;
                    item_template.light.sides = 0;
                    while (s[i] != '+') {
                        item_template.light.base *= 10;
                        item_template.light.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.light.num *= 10;
                        item_template.light.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.light.sides *= 10;
                        item_template.light.sides += s[i++] - '0';
                    }
                }
                else if (s == "VAL") {
                    check |= 0x1000;
                    f >> s;
                    i = 0;
                    item_template.value.base = 0;
                    item_template.value.num = 0;
                    item_template.value.sides = 0;
                    while (s[i] != '+') {
                        item_template.value.base *= 10;
                        item_template.value.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        item_template.value.num *= 10;
                        item_template.value.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        item_template.value.sides *= 10;
                        item_template.value.sides += s[i++] - '0';
                    }
                }
                else if (s == "RRTY") {
                    check |= 0x2000;
                    f >> s;
                    i = 0;
                    item_template.rarity = 0;
                    while (s[i] != '\0') {
                        item_template.rarity *= 10;
                        item_template.rarity += s[i++] - '0';
                    }
                }
                else if (s != "END") {
                    std::cout << s << std::endl;
                    return 4;
                }
            } while (s != "END");
            if (check != 0x3FFF) {
                std::cout << check << std::endl;
                return 5;
            }
            (*item_dictionary).push_back(item_template);
        }
    } while (!f.eof());
    return 0;
}

void get_args(int argc, char *argv[], uint8_t *args, uint16_t *num_init_monsters)
{
    uint8_t i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--save")) {
            *args |= SAVE_DUNGEON;
        }
        else if (!strcmp(argv[i], "--load")) {
            *args |= LOAD_DUNGEON;
        }
        else if (!strcmp(argv[i], "--nummon")) {
            *args |= NUM_MON;
            sscanf(argv[++i], "%hd", num_init_monsters);
        }
    }
}

void generate(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs)
{
    uint16_t i, j;
    uint8_t v;
    *num_rooms = 6 + rand() % 2;
    *rooms = (room_t *) malloc(sizeof(room_t) * *num_rooms);
    do {
        for (i = 0; i < *num_rooms; i++) {
            (*rooms)[i].xsize = 4 + rand() % 10;
            (*rooms)[i].ysize = 3 + rand() % 10;
            (*rooms)[i].xpos = 1 + rand() % (W_WIDTH - (*rooms)[i].xsize - 2);
            (*rooms)[i].ypos = 1 + rand() % (W_HEIGHT - (*rooms)[i].ysize - 2);
        }
        v = 1;
        for (i = 1; i < *num_rooms && v; i++) {
            for (j = 0; j < i && v; j++) {
                if (!((*rooms)[i].xpos + (*rooms)[i].xsize < (*rooms)[j].xpos || (*rooms)[j].xpos + (*rooms)[j].xsize < (*rooms)[i].xpos)
                        && !((*rooms)[i].ypos + (*rooms)[i].ysize < (*rooms)[j].ypos || (*rooms)[j].ypos + (*rooms)[j].ysize < (*rooms)[i].ypos)) {
                    v = 0;
                }
            }
        }
    } while(!v);
    *player = (character_t *) malloc(sizeof(character_t));
    i = rand() % *num_rooms;
    (*player)->xpos = (*rooms)[i].xpos + rand() % (*rooms)[i].xsize;
    (*player)->ypos = (*rooms)[i].ypos + rand() % (*rooms)[i].ysize;
    (*player)->symbol = '@';
    *num_u_stairs = 1;
    *u_stairs = (entity_t *) malloc(sizeof(character_t));
    (*u_stairs)->xpos = (*rooms)[2].xpos + 1;
    (*u_stairs)->ypos = (*rooms)[2].ypos + 1;
    (*u_stairs)->symbol = '<';
    *num_d_stairs = 1;
    *d_stairs = (entity_t *) malloc(sizeof(character_t));
    (*d_stairs)->xpos = (*rooms)[*num_rooms - 1].xpos + 1;
    (*d_stairs)->ypos = (*rooms)[*num_rooms - 1].ypos + 1;
    (*d_stairs)->symbol = '>';
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            hardness[i][j] = 0x80;
        }
    }
    for (i = 0; i < W_HEIGHT; i++) {
        hardness[i][0] = 0xFF;
        hardness[i][W_WIDTH-1] = 0xFF;
    }
    for (j = 0; j < W_WIDTH; j++) {
        hardness[0][j] = 0xFF;
        hardness[W_HEIGHT-1][j] = 0xFF;
    }
    for (i = 1; i < *num_rooms; i++) {
        for (j = (*rooms)[i-1].ypos + (*rooms)[i-1].ysize / 2; j <= (*rooms)[i].ypos + (*rooms)[i].ysize / 2; j++) {
            hardness[j][(*rooms)[i-1].xpos + (*rooms)[i-1].xsize / 2] = 0;
        }
        for (j = (*rooms)[i-1].ypos + (*rooms)[i-1].ysize / 2; j >= (*rooms)[i].ypos + (*rooms)[i].ysize / 2; j--) {
            hardness[j][(*rooms)[i-1].xpos + (*rooms)[i-1].xsize / 2] = 0;
        }
        for (j = (*rooms)[i-1].xpos + (*rooms)[i-1].xsize / 2; j <= (*rooms)[i].xpos + (*rooms)[i].xsize / 2; j++) {
            hardness[(*rooms)[i].ypos + (*rooms)[i].ysize / 2][j] = 0;
        }
        for (j = (*rooms)[i-1].xpos + (*rooms)[i-1].xsize / 2; j >= (*rooms)[i].xpos + (*rooms)[i].xsize / 2; j--) {
            hardness[(*rooms)[i].ypos + (*rooms)[i].ysize / 2][j] = 0;
        }
    }
}

void save_dungeon(character_t *player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, entity_t *u_stairs, uint16_t num_d_stairs, entity_t *d_stairs)
{
    uint16_t i;
    uint8_t j;
    uint8_t w_byte;
    uint16_t w_short;
    uint32_t w;
    char version[13] = "RLG327-@2021";
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *save = "dungeon";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, save);
    FILE *f = fopen(path, "w");
    for (i = 0; i < 12; i++) {
        fwrite(&version[i], sizeof(char), 1, f);
    }
    w = htobe32(0);
    fwrite(&w, sizeof(uint32_t), 1, f);
    w = htobe32(1708 + num_rooms * 4 + num_u_stairs * 2 + num_d_stairs * 2);
    fwrite(&w, sizeof(uint32_t), 1, f);
    w_byte = player->xpos;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    w_byte = player->ypos;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            w_byte = hardness[i][j];
            fwrite(&w_byte, sizeof(uint8_t), 1, f);
        }
    }
    w_short = htobe16(num_rooms);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_rooms; i++) {
        w_byte = rooms[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].xsize;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].ysize;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_short = htobe16(num_u_stairs);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_u_stairs; i++) {
        w_byte = u_stairs[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = u_stairs[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_short = htobe16(num_d_stairs);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_d_stairs; i++) {
        w_byte = d_stairs[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = d_stairs[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    fclose(f);
    free(path);
}

void load_dungeon(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs)
{
    uint16_t i;
    uint8_t j;
    uint16_t count = 0;
    uint8_t r_byte;
    uint16_t r_short;
    uint32_t r;
    char version[13];
    version[12] = '\0';
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *save = "dungeon";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, save);
    FILE *f = fopen(path, "r");
    fread(version, sizeof(char), 12, f);
    fread(&r, sizeof(uint32_t), 1, f);
    fread(&r, sizeof(uint32_t), 1, f);
    *player = (character_t *) malloc(sizeof(character_t));
    fread(&r_byte, sizeof(uint8_t), 1, f);
    (*player)->xpos = r_byte;
    fread(&r_byte, sizeof(uint8_t), 1, f);
    (*player)->ypos = r_byte;
    (*player)->symbol = '@';
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            hardness[i][j] = r_byte;
            count++;
        }
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_rooms = be16toh(r_short);
    *rooms = (room_t *) malloc(sizeof(room_t) * *num_rooms);
    for (i = 0; i < *num_rooms; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].ypos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].xsize = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].ysize = r_byte;
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_u_stairs = be16toh(r_short);
    *u_stairs = (entity_t *) malloc(sizeof(character_t) * *num_u_stairs);
    for (i = 0; i < *num_u_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].ypos = r_byte;
        (*u_stairs)[i].symbol = '<';
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_d_stairs = be16toh(r_short);
    *d_stairs = (entity_t *) malloc(sizeof(character_t) * *num_d_stairs);
    for (i = 0; i < *num_d_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*d_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*d_stairs)[i].ypos = r_byte;
        (*d_stairs)[i].symbol = '>';
    }
    fclose(f);
    free(path);
}

void map_rooms(char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms)
{
    uint16_t i;
    uint8_t j, k;
    for (i = 0; i < num_rooms; i++) {
        for (j = 0; j < rooms[i].ysize; j++) {
            for (k = 0; k < rooms[i].xsize; k++) {
                map[rooms[i].ypos + j][rooms[i].xpos + k] = '.';
            }
        }
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (hardness[i][j] == 0 && map[i][j] == ' ') {
                map[i][j] = '#';
            }
        }
    }
}

void save(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], char visible_map[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, entity_t *u_stairs, uint16_t num_d_stairs, entity_t *d_stairs, uint32_t priority, uint16_t num_init_monsters, uint16_t num_monsters, uint16_t num_init_items, std::vector<character_template_t> *monster_dictionary, std::vector<item_template_t> *item_dictionary, character_t *player, heap_t *characters, heap_t *next_turn, std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH])
{
    uint16_t i, j, k, l;
    item_t *item;
    char w_char;
    int w_int;
    uint8_t w_byte;
    uint16_t w_short;
    uint32_t w_long;
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *save = "save";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, save);
    FILE *f = fopen(path, "w");
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            w_byte = hardness[i][j];
            fwrite(&w_byte, sizeof(uint8_t), 1, f);
            w_char = map[i][j];
            fwrite(&w_char, sizeof(char), 1, f);
            w_char = visible_map[i][j];
            fwrite(&w_char, sizeof(char), 1, f);
        }
    }
    w_short = htobe16(num_rooms);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_rooms; i++) {
        w_byte = rooms[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].xsize;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = rooms[i].ysize;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_short = htobe16(num_u_stairs);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_u_stairs; i++) {
        w_byte = u_stairs[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = u_stairs[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_char = u_stairs[i].symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = u_stairs[i].color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_short = htobe16(num_d_stairs);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < num_d_stairs; i++) {
        w_byte = d_stairs[i].xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = d_stairs[i].ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_char = d_stairs[i].symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = d_stairs[i].color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_long = htobe32(priority);
    fwrite(&w_long, sizeof(uint32_t), 1, f);
    w_short = htobe16(num_init_monsters);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    w_short = htobe16(num_monsters);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    w_short = htobe16(num_init_items);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    w_short = htobe16((uint16_t) monster_dictionary->size());
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < monster_dictionary->size(); i++) {
        for (j = 0; (*monster_dictionary)[i].name.c_str()[j] != '\0'; j++) {
            w_char = (*monster_dictionary)[i].name.c_str()[j];
            fwrite(&w_char, sizeof(char), 1, f);
        }
        w_char = (*monster_dictionary)[i].name.c_str()[j];
        fwrite(&w_char, sizeof(char), 1, f);
        w_char = (*monster_dictionary)[i].symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = (*monster_dictionary)[i].color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_short = htobe16((*monster_dictionary)[i].description.size());
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        for (j = 0; j < (*monster_dictionary)[i].description.size(); j++) {
            for (k = 0; (*monster_dictionary)[i].description[j].c_str()[k] != '\0'; k++) {
                w_char = (*monster_dictionary)[i].description[j].c_str()[k];
                fwrite(&w_char, sizeof(char), 1, f);
            }
            w_char = (*monster_dictionary)[i].description[j].c_str()[k];
            fwrite(&w_char, sizeof(char), 1, f);
        }
        w_int = htobe32((*monster_dictionary)[i].speed.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].speed.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].speed.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_short = htobe16((*monster_dictionary)[i].abilities);
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        w_int = htobe32((*monster_dictionary)[i].hitpoints.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].hitpoints.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].hitpoints.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].damage.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].damage.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*monster_dictionary)[i].damage.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_byte = (*monster_dictionary)[i].rarity;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_short = htobe16(item_dictionary->size());
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    for (i = 0; i < item_dictionary->size(); i++) {
        for (j = 0; (*item_dictionary)[i].name.c_str()[j] != '\0'; j++) {
            w_char = (*item_dictionary)[i].name.c_str()[j];
            fwrite(&w_char, sizeof(char), 1, f);
        }
        w_char = (*item_dictionary)[i].name.c_str()[j];
        fwrite(&w_char, sizeof(char), 1, f);
        w_char = (*item_dictionary)[i].symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = (*item_dictionary)[i].type;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = (*item_dictionary)[i].color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_short = htobe16((*item_dictionary)[i].description.size());
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        for (j = 0; j < (*item_dictionary)[i].description.size(); j++) {
            for (k = 0; (*item_dictionary)[i].description[j].c_str()[k] != '\0'; k++) {
                w_char = (*item_dictionary)[i].description[j].c_str()[k];
                fwrite(&w_char, sizeof(char), 1, f);
            }
            w_char = (*item_dictionary)[i].description[j].c_str()[k];
            fwrite(&w_char, sizeof(char), 1, f);
        }
        w_int = htobe32((*item_dictionary)[i].speed_bonus.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].speed_bonus.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].speed_bonus.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].attributes.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].attributes.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].attributes.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_byte = (*item_dictionary)[i].data;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_int = htobe32((*item_dictionary)[i].hitpoint_bonus.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].hitpoint_bonus.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].hitpoint_bonus.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].damage_bonus.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].damage_bonus.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].damage_bonus.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].dodge_bonus.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].dodge_bonus.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].dodge_bonus.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].defence_bonus.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].defence_bonus.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].defence_bonus.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].light.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].light.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].light.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].weight.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].weight.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].weight.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].value.base);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].value.num);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32((*item_dictionary)[i].value.sides);
        fwrite(&w_int, sizeof(int), 1, f);
        w_byte = (*item_dictionary)[i].rarity;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
    }
    w_byte = player->xpos;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    w_byte = player->ypos;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    w_char = player->symbol;
    fwrite(&w_char, sizeof(char), 1, f);
    w_byte = player->color;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    w_short = htobe16(player->data);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    w_short = htobe16(player->sequence);
    fwrite(&w_short, sizeof(uint16_t), 1, f);
    w_int = htobe32(player->speed);
    fwrite(&w_int, sizeof(int), 1, f);
    w_int = htobe32(player->hitpoints);
    fwrite(&w_int, sizeof(int), 1, f);
    w_byte = player->num_items;
    fwrite(&w_byte, sizeof(uint8_t), 1, f);
    for (i = 0; i < 10; i++) {
        if (player->inventory[i]) {
            for (j = 0; &((*item_dictionary)[j]) != player->inventory[i]->entry && j < item_dictionary->size(); j++);
            if (j < item_dictionary->size()) {
                w_byte = 1;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
                w_short = htobe16(j);
                fwrite(&w_short, sizeof(uint16_t), 1, f);
                w_int = htobe32(player->inventory[i]->speed_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->attributes);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->hitpoint_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->dodge_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->defence_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->light);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->weight);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->inventory[i]->value);
                fwrite(&w_int, sizeof(int), 1, f);
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        else {
            w_byte = 0;
            fwrite(&w_byte, sizeof(uint8_t), 1, f);
        }
    }
    for (i = 0; i < OTHER; i++) {
        if (player->equipment[i]) {
            for (j = 0; &((*item_dictionary)[j]) != player->equipment[i]->entry && j < item_dictionary->size(); j++);
            if (j < item_dictionary->size()) {
                w_byte = 1;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
                w_short = htobe16(j);
                fwrite(&w_short, sizeof(uint16_t), 1, f);
                w_int = htobe32(player->equipment[i]->speed_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->attributes);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->hitpoint_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->dodge_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->defence_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->light);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->weight);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(player->equipment[i]->value);
                fwrite(&w_int, sizeof(int), 1, f);
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        else {
            w_byte = 0;
            fwrite(&w_byte, sizeof(uint8_t), 1, f);
        }
    }
    w_long = htobe32(characters->length);
    fwrite(&w_long, sizeof(uint32_t), 1, f);
    w_long = htobe32(characters->size);
    fwrite(&w_long, sizeof(uint32_t), 1, f);
    for (i = 0; i < characters->size; i++) {
        w_byte = ((character_t *) characters->nodes[i].value)->xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = ((character_t *) characters->nodes[i].value)->ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_char = ((character_t *) characters->nodes[i].value)->symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = ((character_t *) characters->nodes[i].value)->color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_short = htobe16(((character_t *) characters->nodes[i].value)->data);
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        w_short = htobe16(((character_t *) characters->nodes[i].value)->sequence);
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        w_int = htobe32(((character_t *) characters->nodes[i].value)->speed);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32(((character_t *) characters->nodes[i].value)->hitpoints);
        fwrite(&w_int, sizeof(int), 1, f);
        w_byte = ((character_t *) characters->nodes[i].value)->num_items;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        for (i = 0; i < 10; i++) {
            if (((character_t *) characters->nodes[i].value)->inventory[i]) {
                for (j = 0; &((*item_dictionary)[j]) != ((character_t *) characters->nodes[i].value)->inventory[i]->entry &&
                        j < item_dictionary->size(); j++);
                if (j < item_dictionary->size()) {
                    w_byte = 1;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                    w_short = htobe16(j);
                    fwrite(&w_short, sizeof(uint16_t), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->speed_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->attributes);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->hitpoint_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->dodge_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->defence_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->light);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->weight);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->inventory[i]->value);
                    fwrite(&w_int, sizeof(int), 1, f);
                }
                else {
                    w_byte = 0;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                }
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        for (i = 0; i < OTHER; i++) {
            if (((character_t *) characters->nodes[i].value)->equipment[i]) {
                for (j = 0; &((*item_dictionary)[j]) != ((character_t *) characters->nodes[i].value)->equipment[i]->entry &&
                        j < item_dictionary->size(); j++);
                if (j < item_dictionary->size()) {
                    w_byte = 1;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                    w_short = htobe16(j);
                    fwrite(&w_short, sizeof(uint16_t), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->speed_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->attributes);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->hitpoint_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->dodge_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->defence_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->light);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->weight);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) characters->nodes[i].value)->equipment[i]->value);
                    fwrite(&w_int, sizeof(int), 1, f);
                }
                else {
                    w_byte = 0;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                }
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        w_byte = ((monster_t *) characters->nodes[i].value)->known_location.xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = ((monster_t *) characters->nodes[i].value)->known_location.ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_long = htobe32(characters->nodes[i].priority);
        fwrite(&w_long, sizeof(uint32_t), 1, f);
    }
    w_long = htobe32(next_turn->length);
    fwrite(&w_long, sizeof(uint32_t), 1, f);
    w_long = htobe32(next_turn->size);
    fwrite(&w_long, sizeof(uint32_t), 1, f);
    for (i = 0; i < next_turn->size; i++) {
        w_byte = ((character_t *) next_turn->nodes[i].value)->xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = ((character_t *) next_turn->nodes[i].value)->ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_char = ((character_t *) next_turn->nodes[i].value)->symbol;
        fwrite(&w_char, sizeof(char), 1, f);
        w_byte = ((character_t *) next_turn->nodes[i].value)->color;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_short = htobe16(((character_t *) next_turn->nodes[i].value)->data);
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        w_short = htobe16(((character_t *) next_turn->nodes[i].value)->sequence);
        fwrite(&w_short, sizeof(uint16_t), 1, f);
        w_int = htobe32(((character_t *) next_turn->nodes[i].value)->speed);
        fwrite(&w_int, sizeof(int), 1, f);
        w_int = htobe32(((character_t *) next_turn->nodes[i].value)->hitpoints);
        fwrite(&w_int, sizeof(int), 1, f);
        w_byte = ((character_t *) next_turn->nodes[i].value)->num_items;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        for (i = 0; i < 10; i++) {
            if (((character_t *) next_turn->nodes[i].value)->inventory[i]) {
                for (j = 0; &((*item_dictionary)[j]) != ((character_t *) next_turn->nodes[i].value)->inventory[i]->entry &&
                        j < item_dictionary->size(); j++);
                if (j < item_dictionary->size()) {
                    w_byte = 1;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                    w_short = htobe16(j);
                    fwrite(&w_short, sizeof(uint16_t), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->speed_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->attributes);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->hitpoint_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->dodge_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->defence_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->light);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->weight);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->inventory[i]->value);
                    fwrite(&w_int, sizeof(int), 1, f);
                }
                else {
                    w_byte = 0;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                }
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        for (i = 0; i < OTHER; i++) {
            if (((character_t *) next_turn->nodes[i].value)->equipment[i]) {
                for (j = 0; &((*item_dictionary)[j]) != ((character_t *) next_turn->nodes[i].value)->equipment[i]->entry &&
                        j < item_dictionary->size(); j++);
                if (j < item_dictionary->size()) {
                    w_byte = 1;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                    w_short = htobe16(j);
                    fwrite(&w_short, sizeof(uint16_t), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->speed_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->attributes);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->hitpoint_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->dodge_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->defence_bonus);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->light);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->weight);
                    fwrite(&w_int, sizeof(int), 1, f);
                    w_int = htobe32(((character_t *) next_turn->nodes[i].value)->equipment[i]->value);
                    fwrite(&w_int, sizeof(int), 1, f);
                }
                else {
                    w_byte = 0;
                    fwrite(&w_byte, sizeof(uint8_t), 1, f);
                }
            }
            else {
                w_byte = 0;
                fwrite(&w_byte, sizeof(uint8_t), 1, f);
            }
        }
        w_byte = ((monster_t *) next_turn->nodes[i].value)->known_location.xpos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_byte = ((monster_t *) next_turn->nodes[i].value)->known_location.ypos;
        fwrite(&w_byte, sizeof(uint8_t), 1, f);
        w_long = htobe32(next_turn->nodes[i].priority);
        fwrite(&w_long, sizeof(uint32_t), 1, f);
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            w_short = htobe16(item_map[i][j].size());
            fwrite(&w_short, sizeof(uint16_t), 1, f);
            for (k = 0; k < item_map[i][j].size(); k++) {
                item = item_map[i][j][k];
                for (l = 0; &((*item_dictionary)[l]) != item->entry && l < item_dictionary->size(); l++);
                w_short = htobe16(l);
                fwrite(&w_short, sizeof(uint16_t), 1, f);
                w_int = htobe32(item->speed_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->attributes);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->hitpoint_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->dodge_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->defence_bonus);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->light);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->weight);
                fwrite(&w_int, sizeof(int), 1, f);
                w_int = htobe32(item->value);
                fwrite(&w_int, sizeof(int), 1, f);
            }
        }
    }
    fclose(f);
    free(path);
}

void load(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], char visible_map[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs, uint32_t *priority, uint16_t *num_init_monsters, uint16_t *num_monsters, uint16_t *num_init_items, std::vector<character_template_t> *monster_dictionary, std::vector<item_template_t> *item_dictionary, character_t *player, heap_t *characters, heap_t *next_turn, std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH])
{
    uint16_t i, j, k;
    uint16_t num1, num2;
    std::string s;
    character_template_t character_template;
    item_template_t item_template;
    monster_t *monster;
    item_t *item;
    char r_char;
    int r_int;
    uint8_t r_byte;
    uint16_t r_short;
    uint32_t r_long;
    char version[13];
    version[12] = '\0';
    char *home = getenv("HOME");
    const char *gamedir = ".rlg327";
    const char *save = "save";
    char *path = (char *) malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, save);
    FILE *f = fopen(path, "r");
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            hardness[i][j] = r_byte;
            fread(&r_char, sizeof(char), 1, f);
            map[i][j] = r_char;
            fread(&r_char, sizeof(char), 1, f);
            visible_map[i][j] = r_char;
        }
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_rooms = be16toh(r_short);
    *rooms = (room_t *) malloc(*num_rooms * sizeof(room_t));
    for (i = 0; i < *num_rooms; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].ypos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].xsize = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*rooms)[i].ysize = r_byte;
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_u_stairs = be16toh(r_short);
    *u_stairs = (entity_t *) malloc(*num_u_stairs * sizeof(entity_t));
    for (i = 0; i < *num_u_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].ypos = r_byte;
        fread(&r_char, sizeof(char), 1, f);
        (*u_stairs)[i].symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].color = r_byte;
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_d_stairs = be16toh(r_short);
    *d_stairs = (entity_t *) malloc(*num_d_stairs * sizeof(entity_t));
    for (i = 0; i < *num_d_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*d_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*d_stairs)[i].ypos = r_byte;
        fread(&r_char, sizeof(char), 1, f);
        (*d_stairs)[i].symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*d_stairs)[i].color = r_byte;
    }
    fwrite(&r_long, sizeof(uint32_t), 1, f);
    *priority = be32toh(r_long);
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_init_monsters = be16toh(r_short);
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_monsters = be16toh(r_short);
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_init_items = be16toh(r_short);
    fread(&r_short, sizeof(uint16_t), 1, f);
    num1 = be16toh(r_short);
    for (i = 0; i < num1; i++) {
        s = "";
        fread(&r_char, sizeof(char), 1, f);
        while (r_char != '\0') {
            s += r_char;
            fread(&r_char, sizeof(char), 1, f);
        }
        character_template.name = s;
        fread(&r_char, sizeof(char), 1, f);
        character_template.symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        character_template.color = r_byte;
        fread(&r_short, sizeof(uint16_t), 1, f);
        num2 = be16toh(r_short);
        for (j = 0; j < num2; j++) {
            s = "";
            fread(&r_char, sizeof(char), 1, f);
            while (r_char != '\0') {
                s += r_char;
                fread(&r_char, sizeof(char), 1, f);
            }
            character_template.description.push_back(s);
        }
        fread(&r_int, sizeof(int), 1, f);
        character_template.speed.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.speed.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.speed.sides = be32toh(r_int);
        fread(&r_short, sizeof(uint16_t), 1, f);
        character_template.abilities = be16toh(r_short);
        fread(&r_int, sizeof(int), 1, f);
        character_template.hitpoints.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.hitpoints.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.hitpoints.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.damage.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.damage.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        character_template.damage.sides = be32toh(r_int);
        fread(&r_byte, sizeof(uint8_t), 1, f);
        character_template.rarity = r_byte;
        monster_dictionary->push_back(character_template);
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    num1 = be16toh(r_short);
    for (i = 0; i < num1; i++) {
        s = "";
        fread(&r_char, sizeof(char), 1, f);
        while (r_char != '\0') {
            s += r_char;
            fread(&r_char, sizeof(char), 1, f);
        }
        character_template.name = s;
        fread(&r_char, sizeof(char), 1, f);
        item_template.symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        item_template.type = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        item_template.color = r_byte;
        fread(&r_short, sizeof(uint16_t), 1, f);
        num2 = be16toh(r_short);
        for (j = 0; j < num2; j++) {
            s = "";
            fread(&r_char, sizeof(char), 1, f);
            while (r_char != '\0') {
                s += r_char;
                fread(&r_char, sizeof(char), 1, f);
            }
            item_template.description.push_back(s);
        }
        fread(&r_int, sizeof(int), 1, f);
        item_template.speed_bonus.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.speed_bonus.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.speed_bonus.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.attributes.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.attributes.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.attributes.sides = be32toh(r_int);
        fread(&r_byte, sizeof(uint8_t), 1, f);
        item_template.data = r_byte;
        fread(&r_int, sizeof(int), 1, f);
        item_template.hitpoint_bonus.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.hitpoint_bonus.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.hitpoint_bonus.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.damage_bonus.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.damage_bonus.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.damage_bonus.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.dodge_bonus.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.dodge_bonus.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.dodge_bonus.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.defence_bonus.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.defence_bonus.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.defence_bonus.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.light.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.light.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.light.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.weight.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.weight.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.weight.sides = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.value.base = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.value.num = be32toh(r_int);
        fread(&r_int, sizeof(int), 1, f);
        item_template.value.sides = be32toh(r_int);
        fread(&r_byte, sizeof(uint8_t), 1, f);
        item_template.rarity = r_byte;
        item_dictionary->push_back(item_template);
    }
    fread(&r_byte, sizeof(uint8_t), 1, f);
    player->xpos = r_byte;
    fread(&r_byte, sizeof(uint8_t), 1, f);
    player->ypos = r_byte;
    fread(&r_char, sizeof(char), 1, f);
    player->symbol = r_char;
    fread(&r_byte, sizeof(uint8_t), 1, f);
    player->color = r_byte;
    fread(&r_short, sizeof(uint16_t), 1, f);
    player->data = htobe16(r_short);
    fread(&r_short, sizeof(uint16_t), 1, f);
    player->sequence = htobe16(r_short);
    fread(&r_int, sizeof(int), 1, f);
    player->speed = htobe32(r_int);
    fread(&r_int, sizeof(int), 1, f);
    player->hitpoints = htobe32(r_int);
    fread(&r_byte, sizeof(uint8_t), 1, f);
    player->num_items = r_byte;
    for (i = 0; i < 10; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        if (r_byte) {
            fread(&r_short, sizeof(uint16_t), 1, f);
            j = be16toh(r_short);
            player->inventory[i]->entry = &((*item_dictionary)[j]);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->speed_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->attributes = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->hitpoint_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->dodge_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->defence_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->light = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->weight = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->inventory[i]->value = be32toh(r_int);
        }
        else {
            player->inventory[i] = NULL;
        }
    }
    for (i = 0; i < OTHER; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        if (r_byte) {
            fread(&r_short, sizeof(uint16_t), 1, f);
            j = be16toh(r_short);
            player->equipment[i]->entry = &((*item_dictionary)[j]);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->speed_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->attributes = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->hitpoint_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->dodge_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->defence_bonus = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->light = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->weight = be32toh(r_int);
            fread(&r_int, sizeof(int), 1, f);
            player->equipment[i]->value = be32toh(r_int);
        }
        else {
            player->equipment[i] = NULL;
        }
    }
    fread(&r_long, sizeof(uint32_t), 1, f);
    characters->length = be32toh(r_long);
    fread(&r_long, sizeof(uint32_t), 1, f);
    characters->size = be32toh(r_long);
    characters->nodes = (heap_node_t *) malloc(characters->length * sizeof(heap_node_t));
    for (i = 0; i < characters->size; i++) {
        characters->nodes[i].value = (void *) malloc(sizeof(monster_t));
        monster = (monster_t *) characters->nodes[i].value;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->ypos = r_byte;
        fread(&r_char, sizeof(char), 1, f);
        monster->symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->color = r_byte;
        fread(&r_short, sizeof(uint16_t), 1, f);
        monster->data = htobe16(r_short);
        fread(&r_short, sizeof(uint16_t), 1, f);
        monster->sequence = htobe16(r_short);
        fread(&r_int, sizeof(int), 1, f);
        monster->speed = htobe32(r_int);
        fread(&r_int, sizeof(int), 1, f);
        monster->hitpoints = htobe32(r_int);
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->num_items = r_byte;
        for (i = 0; i < 10; i++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            if (r_byte) {
                fread(&r_short, sizeof(uint16_t), 1, f);
                j = be16toh(r_short);
                monster->inventory[i]->entry = &((*item_dictionary)[j]);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->speed_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->attributes = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->hitpoint_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->dodge_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->defence_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->light = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->weight = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->value = be32toh(r_int);
            }
            else {
                monster->inventory[i] = NULL;
            }
        }
        for (i = 0; i < OTHER; i++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            if (r_byte) {
                fread(&r_short, sizeof(uint16_t), 1, f);
                j = be16toh(r_short);
                monster->equipment[i]->entry = &((*item_dictionary)[j]);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->speed_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->attributes = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->hitpoint_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->dodge_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->defence_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->light = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->weight = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->value = be32toh(r_int);
            }
            else {
                monster->equipment[i] = NULL;
            }
        }
        fread(&r_byte, sizeof(uint8_t), 1, f);
        ((monster_t *) characters->nodes[i].value)->known_location.xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        ((monster_t *) characters->nodes[i].value)->known_location.ypos = r_byte;
        fread(&r_long, sizeof(uint32_t), 1, f);
        characters->nodes[i].priority = be32toh(r_long);
    }
    fread(&r_long, sizeof(uint32_t), 1, f);
    next_turn->length = be32toh(r_long);
    fread(&r_long, sizeof(uint32_t), 1, f);
    next_turn->size = be32toh(r_long);
    next_turn->nodes = (heap_node_t *) malloc(next_turn->length * sizeof(heap_node_t));
    for (i = 0; i < next_turn->size; i++) {
        next_turn->nodes[i].value = (void *) malloc(sizeof(monster_t));
        monster = (monster_t *) next_turn->nodes[i].value;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->ypos = r_byte;
        fread(&r_char, sizeof(char), 1, f);
        monster->symbol = r_char;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->color = r_byte;
        fread(&r_short, sizeof(uint16_t), 1, f);
        monster->data = htobe16(r_short);
        fread(&r_short, sizeof(uint16_t), 1, f);
        monster->sequence = htobe16(r_short);
        fread(&r_int, sizeof(int), 1, f);
        monster->speed = htobe32(r_int);
        fread(&r_int, sizeof(int), 1, f);
        monster->hitpoints = htobe32(r_int);
        fread(&r_byte, sizeof(uint8_t), 1, f);
        monster->num_items = r_byte;
        for (i = 0; i < 10; i++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            if (r_byte) {
                fread(&r_short, sizeof(uint16_t), 1, f);
                j = be16toh(r_short);
                monster->inventory[i]->entry = &((*item_dictionary)[j]);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->speed_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->attributes = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->hitpoint_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->dodge_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->defence_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->light = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->weight = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->inventory[i]->value = be32toh(r_int);
            }
            else {
                monster->inventory[i] = NULL;
            }
        }
        for (i = 0; i < OTHER; i++) {
            fread(&r_byte, sizeof(uint8_t), 1, f);
            if (r_byte) {
                fread(&r_short, sizeof(uint16_t), 1, f);
                j = be16toh(r_short);
                monster->equipment[i]->entry = &((*item_dictionary)[j]);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->speed_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->attributes = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->hitpoint_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->dodge_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->defence_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->light = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->weight = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                monster->equipment[i]->value = be32toh(r_int);
            }
            else {
                monster->equipment[i] = NULL;
            }
        }
        fread(&r_byte, sizeof(uint8_t), 1, f);
        ((monster_t *) next_turn->nodes[i].value)->known_location.xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        ((monster_t *) next_turn->nodes[i].value)->known_location.ypos = r_byte;
        fread(&r_long, sizeof(uint32_t), 1, f);
        next_turn->nodes[i].priority = be32toh(r_long);
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            fread(&r_short, sizeof(uint16_t), 1, f);
            num1 = be32toh(r_short);
            for (k = 0; k < num1; k++) {
                item = (item_t *) malloc(sizeof(item_t));
                fread(&r_short, sizeof(uint16_t), 1, f);
                num2 = be16toh(r_short);
                item->entry = &((*item_dictionary)[num2]);
                fread(&r_int, sizeof(int), 1, f);
                item->speed_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->attributes = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->hitpoint_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->dodge_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->defence_bonus = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->light = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->weight = be32toh(r_int);
                fread(&r_int, sizeof(int), 1, f);
                item->value = be32toh(r_int);
                item_map[i][j].push_back(item);
            }
        }
    }
    fclose(f);
    free(path);
}

void init_player(heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], character_t *player, character_template_t *player_template)
{
    player->symbol = '@';
    player->color = 0x80;
    player->sequence = 0;
    player->data = 0x0000;
    player->data |= FOG;
    player->speed = 10;
    player->hitpoints = 1000;
    player->num_items = 0;
    player->inventory = (item_t **) calloc(10, sizeof(item_t));
    player->equipment = (item_t **) calloc(OTHER, sizeof(item_t));
    heap_add(characters, player, 0);
    character_map[player->ypos][player->xpos] = player;
}

void generate_monsters(std::vector<character_template_t> *monster_dictionary, heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], uint16_t num_init_monsters, stack_t *monster_carry)
{
    uint16_t i;
    uint32_t rarity_index, rarity_base;
    monster_t *monster;
    std::vector<character_template_t>::iterator it;
    rarity_base = 0;
    for (it = monster_dictionary->begin(); it != monster_dictionary->end(); it++) {
        rarity_base += (*it).rarity;
        if (((*it).abilities & BOSS) && !((*it).abilities & MONSTER_EXISTS)) {
            (*it).abilities |= MONSTER_EXISTS;
            monster = (monster_t *) (*it).instantiate();
            monster->sequence = i++;
            monster->data &= ~KNOWN;
            monster->data &= ~SEE;
            monster->entry = &(*it);
            stack_push(monster_carry, monster);
        }
    }
    for (i = 1; i <= num_init_monsters; i++) {
        if (stack_size(monster_carry)) {
            stack_pop(monster_carry, (void **) &monster);
            monster->data &= ~KNOWN;
            monster->data &= ~SEE;
        }
        else {
            do {
                rarity_index = rand() % rarity_base;
                for (it = monster_dictionary->begin(); (*it).rarity < rarity_index && it != monster_dictionary->end(); it++) {
                    rarity_index -= (*it).rarity;
                }
            } while ((*it).abilities & UNIQ && (*it).abilities & MONSTER_EXISTS);
            (*it).abilities |= MONSTER_EXISTS;
            monster = (monster_t *) (*it).instantiate();
            monster->sequence = i;
            monster->data &= ~KNOWN;
            monster->data &= ~SEE;
            monster->entry = &(*it);
        }
        do {
            monster->xpos = 1 + rand() % (W_WIDTH - 2);
            monster->ypos = 1 + rand() % (W_HEIGHT - 2);
        } while (character_map[monster->ypos][monster->xpos] != NULL);
        if (!(monster->data & (TUNNEL | PASS))) {
            while (map[monster->ypos][monster->xpos] == ' ' || character_map[monster->ypos][monster->xpos] != NULL) {
                monster->xpos = 1 + rand() % (W_WIDTH - 2);
                monster->ypos = 1 + rand() % (W_HEIGHT - 2);
            }
        }
        heap_add(characters, monster, 0);
        character_map[monster->ypos][monster->xpos] = monster;
    }
}

void generate_items(std::vector<item_template_t> *item_dictionary, std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], uint16_t num_init_items, stack_t *item_carry)
{
    uint16_t i;
    uint32_t rarity_index, rarity_base;
    item_t *item;
    std::vector<item_template_t>::iterator it;
    rarity_base = 0;
    for (it = item_dictionary->begin(); it != item_dictionary->end(); it++) {
        rarity_base += (*it).rarity;
    }
    for (i = 1; i <= num_init_items; i++) {
        if (stack_size(item_carry)) {
            stack_pop(item_carry, (void **) &item);
        }
        else {
            do {
                rarity_index = rand() % rarity_base;
                for (it = item_dictionary->begin(); (*it).rarity < rarity_index && it != item_dictionary->end(); it++) {
                    rarity_index -= (*it).rarity;
                }
            } while ((*it).data & RELIC && (*it).data & ITEM_EXISTS);
            item = (item_t *) (*it).instantiate();
            (*it).data |= ITEM_EXISTS;
            item->entry = &(*it);
        }
        do {
            item->xpos = 1 + rand() % (W_WIDTH - 2);
            item->ypos = 1 + rand() % (W_HEIGHT - 2);
        } while (map[item->ypos][item->xpos] == ' ');
        item_map[item->ypos][item->xpos].push_back(item);
    }
}

void dijkstra(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], character_t* player, uint16_t distance[W_HEIGHT][W_WIDTH], uint8_t type)
{
    uint8_t i, j;
    uint32_t temp;
    vertex_t *v, *u;
    stack_t s;
    heap_t h;
    stack_init(&s);
    heap_init(&h);
    distance[player->ypos][player->xpos] = 0;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (!(i == player->ypos && j == player->xpos)) {
                distance[i][j] = 0xFFFF;
            }
            if ((type || map[i][j] != ' ') && hardness[i][j] != 0xFF) {
                v = (vertex_t *) malloc(sizeof(vertex_t));
                v->ypos = i;
                v->xpos = j;
                heap_add(&h, v, distance[v->ypos][v->xpos]);
            }
        }
    }
    while (heap_size(&h) > 0) {
        heap_peek(&h, (void**) &u, &temp);
        heap_extract_min(&h, (void**) &u);
        if (temp == 0xFFFF) {
            free(u);
            continue;
        }
        get_neighbors(&s, u, map, hardness, type);
        while (stack_size(&s) > 0) {
            stack_pop(&s, (void**) &v);
            if (map[v->ypos][v->xpos] == ' ' && type == 1)
                temp = distance[u->ypos][u->xpos] + 1 + hardness[v->ypos][v->xpos] / 85;
            else if (map[v->ypos][v->xpos] == ' ' && type == 2)
                temp = distance[u->ypos][u->xpos] + 1;
            else
                temp = distance[u->ypos][u->xpos] + 1;
            if (temp < distance[v->ypos][v->xpos]) {
                distance[v->ypos][v->xpos] = temp;
                heap_decrease_priority_vertex(&h, v, temp);
            }
            free(v);
        }
        free(u);
    }
    stack_delete(&s);
    heap_delete(&h);
}

void get_neighbors(stack_t *s, vertex_t *v, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint8_t type)
{
    vertex_t *u;
    if (v->ypos > 1) {
        if (v->xpos > 1) {
            if (hardness[v->ypos - 1][v->xpos - 1] != 0xFF && (type || map[v->ypos - 1][v->xpos - 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos - 1;
                u->xpos = v->xpos - 1;
                stack_push(s, u);
            }
        }
        if (hardness[v->ypos - 1][v->xpos] != 0xFF && (type || map[v->ypos - 1][v->xpos] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos - 1;
            u->xpos = v->xpos;
            stack_push(s, u);
        }
        if (v->xpos < W_WIDTH - 2) {
            if (hardness[v->ypos - 1][v->xpos + 1] != 0xFF && (type || map[v->ypos - 1][v->xpos + 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos - 1;
                u->xpos = v->xpos + 1;
                stack_push(s, u);
            }
        }
    }
    if (v->xpos > 1) {
        if (hardness[v->ypos][v->xpos - 1] != 0xFF && (type || map[v->ypos][v->xpos - 1] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos;
            u->xpos = v->xpos - 1;
            stack_push(s, u);
        }
    }
    if (v->xpos < W_WIDTH - 2) {
        if (hardness[v->ypos][v->xpos + 1] != 0xFF && (type || map[v->ypos][v->xpos + 1] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos;
            u->xpos = v->xpos + 1;
            stack_push(s, u);
        }
    }
    if (v->ypos < W_HEIGHT) {
        if (v->xpos > 1) {
            if (hardness[v->ypos + 1][v->xpos - 1] != 0xFF && (type || map[v->ypos + 1][v->xpos - 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos + 1;
                u->xpos = v->xpos - 1;
                stack_push(s, u);
            }
        }
        if (hardness[v->ypos + 1][v->xpos] != 0xFF && (type || map[v->ypos + 1][v->xpos] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos + 1;
            u->xpos = v->xpos;
            stack_push(s, u);
        }
        if (v->xpos < W_WIDTH - 2) {
            if (hardness[v->ypos + 1][v->xpos + 1] != 0xFF && (type || map[v->ypos + 1][v->xpos + 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos + 1;
                u->xpos = v->xpos + 1;
                stack_push(s, u);
            }
        }
    }
}

void derive_next_turn_heap(heap_t *characters, heap_t *next_turn, uint32_t *priority)
{
    uint32_t next_priority;
    character_t *character;
    heap_init(next_turn);
    heap_peek(characters, (void**) &character, priority);
    do {
        heap_extract_min(characters, (void**) &character);
        heap_add(next_turn, character, character->sequence);
        heap_peek(characters, (void**) &character, &next_priority);
    } while (next_priority == *priority && heap_size(characters) > 0);
}

void derive_move_stack(stack_t *move_stack, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], character_t *character, vertex_t *hither, vertex_t **thither)
{
    stack_init(move_stack);
    hither->xpos = character->xpos;
    hither->ypos = character->ypos;
    get_neighbors(move_stack, hither, map, hardness, character->data & (TUNNEL | PASS));
    *thither = (vertex_t *) malloc(sizeof(vertex_t));
    (*thither)->xpos = hither->xpos;
    (*thither)->ypos = hither->ypos;
    stack_push(move_stack, *thither);
}

void player_command(character_t *player, vertex_t *thither, char visible_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], uint16_t *num_monsters)
{
    int cmd, light, err;
    uint8_t i;
    vertex_t *v;
    item_t *item;
    character_t *character;
    thither->xpos = player->xpos;
    thither->ypos = player->ypos;
    player->data &= ~(WALK_UP & WALK_DOWN);
    do {
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case '7':
            case 'y':
                thither->xpos -= 1;
                thither->ypos -= 1;
                break;
            case '8':
            case 'k':
                thither->ypos -= 1;
                break;
            case '9':
            case 'u':
                thither->xpos += 1;
                thither->ypos -= 1;
                break;
            case '6':
            case 'l':
                thither->xpos += 1;
                break;
            case '3':
            case 'n':
                thither->xpos += 1;
                thither->ypos += 1;
                break;
            case '2':
            case 'j':
                thither->ypos += 1;
                break;
            case '1':
            case 'b':
                thither->xpos -= 1;
                thither->ypos += 1;
                break;
            case '4':
            case 'h':
                thither->xpos -= 1;
                break;
            case '>':
                player->data |= WALK_DOWN;
                break;
            case '<':
                player->data |= WALK_UP;
                break;
            case '5':
            case ' ':
                break;
            case 'a':
                v = (vertex_t *) malloc(sizeof(vertex_t));
                v->xpos = player->xpos;
                v->ypos = player->ypos;
                prompt_place(player, v, map, visible_map, character_map, item_map, 't');
                character = character_map[v->ypos][v->xpos];
                light = 2;
                for (i = 0; i < OTHER; i++) {
                    if (player->equipment[i] != NULL) {
                        light += player->equipment[i]->light;
                    }
                }
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (character != NULL && (!(player->data & FOG) ||
                        (player->xpos - character->xpos >= -light && player->xpos - character->xpos <= light &&
                        player->ypos - character->ypos >= -light && player->ypos - character->ypos <= light))) {
                    err = shoot(player, character_map, item_map, player, character, num_monsters);
                    if (err == 1) {
                        mvprintw(0, 0, "You don't have a ranged weapon.");
                        refresh();
                        cmd = getch();
                        if (cmd == 'Q')
                            player->data |= QUIT;
                        mvprintw(0, 0, "                                                                                ");
                    }
                    if (err == 2) {
                        mvprintw(0, 0, "You don't have any ammunition.");
                        refresh();
                        cmd = getch();
                        if (cmd == 'Q')
                            player->data |= QUIT;
                        mvprintw(0, 0, "                                                                                ");
                    }
                    if (err == 2) {
                    }
                    player->data &= ~ESCAPE;
                }
                else {
                    mvprintw(0, 0, "You didn't aim at anything.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                free(v);
                break;
            case ',':
                if (item_map[player->ypos][player->xpos].size() > 0 && player->num_items < 10) {
                    item = item_map[player->ypos][player->xpos].back();
                    prompt_inventory(player, &i);
                    if (player->data & (ESCAPE | QUIT)) {
                        player->data &= ~ESCAPE;
                    }
                    else if (player->inventory[i] == NULL) {
                        item_map[player->ypos][player->xpos].pop_back();
                        player->inventory[i] = item;
                        player->num_items += 1;
                    }
                    else {
                        mvprintw(0, 0, "You already have something in this slot.");
                        refresh();
                        cmd = getch();
                        if (cmd == 'Q')
                            player->data |= QUIT;
                        mvprintw(0, 0, "                                                                                ");
                    }
                }
                else if (item_map[player->ypos][player->xpos].size() == 0) {
                    mvprintw(0, 0, "There is nothing to pick up.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                else if (player->num_items >= 10) {
                    mvprintw(0, 0, "You are carrying too much.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'w':
                prompt_inventory(player, &i);
                item = player->inventory[i];
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL) {
                    if (item->entry->type < RING1) {
                        if (player->equipment[item->entry->type] != NULL) {
                            player->hitpoints -= player->equipment[item->entry->type]->hitpoint_bonus;
                            player->speed -= player->equipment[item->entry->type]->speed_bonus;
                        }
                        player->inventory[i] = player->equipment[item->entry->type];
                        player->equipment[item->entry->type] = item;
                        player->hitpoints += item->hitpoint_bonus;
                        player->speed += item->speed_bonus;
                        if (player->inventory[i] == NULL)
                            player->num_items -= 1;
                    }
                    else if (item->entry->type < OTHER && player->equipment[RING1] == NULL) {
                        if (player->equipment[RING1] != NULL) {
                            player->hitpoints -= player->equipment[RING1]->hitpoint_bonus;
                            player->speed -= player->equipment[RING1]->speed_bonus;
                        }
                        player->inventory[i] = player->equipment[RING1];
                        player->equipment[RING1] = item;
                        player->hitpoints += item->hitpoint_bonus;
                        player->speed += item->speed_bonus;
                        if (player->inventory[i] == NULL)
                            player->num_items -= 1;
                    }
                    else if (item->entry->type < OTHER) {
                        if (player->equipment[RING2] != NULL) {
                            player->hitpoints -= player->equipment[RING2]->hitpoint_bonus;
                            player->speed -= player->equipment[RING2]->speed_bonus;
                        }
                        player->inventory[i] = player->equipment[RING2];
                        player->equipment[RING2] = item;
                        player->hitpoints += item->hitpoint_bonus;
                        player->speed += item->speed_bonus;
                        if (player->inventory[i] == NULL)
                            player->num_items -= 1;
                    }
                    else {
                        mvprintw(0, 0, "This item cannot be equipped.");
                        refresh();
                        cmd = getch();
                        if (cmd == 'Q')
                            player->data |= QUIT;
                        mvprintw(0, 0, "                                                                                ");
                    }
                }
                else {
                    mvprintw(0, 0, "No item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 't':
                prompt_equipment(player, &i);
                item = player->equipment[i];
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL) {
                    if (player->num_items < 10) {
                        player->equipment[i] = NULL;
                        for (i = 0; player->inventory[i] != NULL; i++);
                        player->inventory[i] = item;
                        player->num_items += 1;
                        player->hitpoints -= item->hitpoint_bonus;
                        player->speed -= item->speed_bonus;
                    }
                    else {
                        mvprintw(0, 0, "There is not enough space in your inventory.");
                        refresh();
                        cmd = getch();
                        if (cmd == 'Q')
                            player->data |= QUIT;
                        mvprintw(0, 0, "                                                                                ");
                    }
                }
                else {
                    mvprintw(0, 0, "No item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'd':
                prompt_inventory(player, &i);
                item = player->inventory[i];
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL) {
                    player->inventory[i] = NULL;
                    item->xpos = player->xpos;
                    item->ypos = player->ypos;
                    item_map[player->ypos][player->xpos].push_back(item);
                    player->num_items -= 1;
                }
                else {
                    mvprintw(0, 0, "No item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'x':
                prompt_inventory(player, &i);
                item = player->inventory[i];
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL) {
                    player->inventory[i] = NULL;
                    player->num_items -= 1;
                    free(item);
                }
                else {
                    mvprintw(0, 0, "No item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'i':
                draw_inventory(player);
                refresh();
                cmd = getch();
                if (cmd == 'Q')
                    player->data |= QUIT;
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'e':
                draw_equipment(player);
                refresh();
                cmd = getch();
                if (cmd == 'Q')
                    player->data |= QUIT;
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'I':
                prompt_inventory(player, &i);
                item = player->inventory[i];
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL) {
                    display_item(player, item);
                    player->data &= ~ESCAPE;
                }
                else {
                    mvprintw(0, 0, "No item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                break;
            case 'L':
                v = (vertex_t *) malloc(sizeof(vertex_t));
                v->xpos = player->xpos;
                v->ypos = player->ypos;
                prompt_place(player, v, map, visible_map, character_map, item_map, 't');
                character = character_map[v->ypos][v->xpos];
                if (item_map[v->ypos][v->xpos].size())
                    item = item_map[v->ypos][v->xpos].back();
                else
                    item = NULL;
                light = 2;
                for (i = 0; i < OTHER; i++) {
                    if (player->equipment[i] != NULL) {
                        light += player->equipment[i]->light;
                    }
                }
                if (player->data & (ESCAPE | QUIT)) {
                    player->data &= ~ESCAPE;
                }
                else if (character != NULL && (!(player->data & FOG) ||
                        (player->xpos - character->xpos >= -light && player->xpos - character->xpos <= light &&
                        player->ypos - character->ypos >= -light && player->ypos - character->ypos <= light))) {
                    display_character(player, character);
                    player->data &= ~ESCAPE;
                }
                else if (item != NULL && (!(player->data & FOG) ||
                        (player->xpos - item->xpos >= -light && player->xpos - item->xpos <= light &&
                        player->ypos - item->ypos >= -light && player->ypos - item->ypos <= light))) {
                    display_item(player, item);
                    player->data &= ~ESCAPE;
                }
                else {
                    mvprintw(0, 0, "No character nor item was selected.");
                    refresh();
                    cmd = getch();
                    if (cmd == 'Q')
                        player->data |= QUIT;
                    mvprintw(0, 0, "                                                                                ");
                }
                sketch_visible_map(visible_map, map, player);
                if (player->data & FOG)
                    draw_fog(visible_map, character_map, item_map, player);
                else
                    draw_full(map, character_map, item_map);
                refresh();
                player->data |= RESET;
                free(v);
                break;
            case 'm':
                player->data |= MON_LIST;
                break;
            case 'f':
                if (player->data & FOG) {
                    player->data &= ~FOG;
                    draw_full(map, character_map, item_map);
                }
                else {
                    player->data |= FOG;
                    draw_fog(visible_map, character_map, item_map, player);
                }
                refresh();
                player->data |= RESET;
                break;
            case 'g':
                player->data |= TELEPORT;
                break;
                /*
            case 268:
                player->data |= SAVE;
                break;
            case 269:
                player->data |= LOAD;
                break;
                */
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                player->data |= RESET;
                break;
        }
    } while (player->data & RESET && !(player->data & QUIT));
    player->data &= ~RESET;
}

void monster_turn(stack_t *move_stack, character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], monster_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t floor_distance[W_HEIGHT][W_WIDTH], uint16_t tunnel_distance[W_HEIGHT][W_WIDTH], uint16_t pass_distance[W_HEIGHT][W_WIDTH], vertex_t *hither, vertex_t **thither, uint16_t *num_monsters)
{
    uint8_t num;
    uint16_t custom_distance[W_HEIGHT][W_WIDTH];
    int cmd;
    stack_t s1, s2;
    vertex_t *u;
    vertex_t v;
    character_t ghost_player;
    character_t *other;
    stack_init(&s1);
    stack_init(&s2);
    if (character->data & ERRATIC && rand() % 2)
        random_move(move_stack, thither);
    else {
        line_of_sight(character, player, map);
        if (character->data & TELE || character->data & SEE) {
            if (character->data & SMART) {
                if (character->data & PASS)
                    smart_move(move_stack, character, thither, pass_distance);
                else if (character->data & TUNNEL)
                    smart_move(move_stack, character, thither, tunnel_distance);
                else
                    smart_move(move_stack, character, thither, floor_distance);
            }
            else {
                dumb_move(move_stack, character, player, hither, thither);
            }
        }
        else {
            if (character->data & KNOWN) {
                ghost_player.xpos = character->known_location.xpos;
                ghost_player.ypos = character->known_location.ypos;
                if (character->data & PASS)
                    dijkstra(hardness, map, &ghost_player, custom_distance, 2);
                else if (character->data & TUNNEL)
                    dijkstra(hardness, map, &ghost_player, custom_distance, 1);
                else
                    dijkstra(hardness, map, &ghost_player, custom_distance, 0);
                smart_move(move_stack, character, thither, custom_distance);
            }
            else {
                random_move(move_stack, thither);
            }
        }
    }
    if (map[(*thither)->ypos][(*thither)->xpos] == ' ' && character->data & TUNNEL && !(character->data & PASS)) {
        tunnel(hardness, player, floor_distance, tunnel_distance, map, *thither);
    }
    if (map[(*thither)->ypos][(*thither)->xpos] != ' ' || character->data & PASS) {
        other = character_map[(*thither)->ypos][(*thither)->xpos];
        if (other != NULL && other != character && other != player) {
            character_map[character->ypos][character->xpos] = NULL;
            v.xpos = other->xpos;
            v.ypos = other->ypos;
            get_neighbors(&s1, &v, map, hardness, other->data & PASS);
            while (stack_size(&s1)) {
                stack_pop(&s1, (void **) &u);
                if (character_map[u->ypos][u->xpos] == NULL) {
                    stack_push(&s2, u);
                }
                else {
                    free(u);
                }
            }
            for (num = rand() % stack_size(&s2); num > 0; num--) {
                stack_pop(&s2, (void **) &u);
                free(u);
            }
            stack_pop(&s2, (void **) &u);
            character_map[other->ypos][other->xpos] = NULL;
            other->xpos = u->xpos;
            other->ypos = u->ypos;
            character_map[other->ypos][other->xpos] = other;
            mvprintw(0, 0, "%s has pushed %s out of its way", character->entry->name.c_str(), other->entry->name.c_str());
            refresh();
            if (!(player->data & QUIT)) {
                cmd = getch();
                if (cmd == 'Q')
                    player->data |= QUIT;
            }
            mvprintw(0, 0, "                                                                                ");
        }
        move_character(player, character_map, item_map, character, hither, *thither, num_monsters);
    }
    stack_delete(&s1);
    stack_delete(&s2);
}

void line_of_sight(monster_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH])
{
    int i;
    double slope;
    character->data |= SEE;
    if (player->xpos - character->xpos) {
        slope = (player->ypos - character->ypos) / (player->xpos - (double) character->xpos);
        if (!(slope > 1.0 || slope < -1.0)) {
            for (i = 0; i <= (int) (player->xpos) - (int) (character->xpos); i++) {
                if (map[character->ypos + (int) (slope * i)][character->xpos + i] == ' ') {
                    character->data &= ~SEE;
                }
            }
            for (i = 0; i <= (int) (character->xpos) - (int) (player->xpos); i++) {
                if (map[player->ypos + (int) (slope * i)][player->xpos + i] == ' ') {
                    character->data &= ~SEE;
                }
            }
        }
    }
    if (player->ypos - character->ypos) {
        slope = (player->xpos - character->xpos) / (player->ypos - (double) character->ypos);
        if (!(slope > 1.0 || slope < -1.0)) {
            for (i = 0; i <= (int) (player->ypos) - (int) (character->ypos); i++) {
                if (map[character->ypos + i][character->xpos + (int) (slope * i)] == ' ') {
                    character->data &= ~SEE;
                }
            }
            for (i = 0; i <= (int) (character->ypos) - (int) (player->ypos); i++) {
                if (map[player->ypos + i][player->xpos + (int) (slope * i)] == ' ') {
                    character->data &= ~SEE;
                }
            }
        }
    }
    if (character->data & SMART && character->data & SEE) {
        character->data |= KNOWN;
        character->known_location.xpos = player->xpos;
        character->known_location.ypos = player->ypos;
    }
}

void smart_move(stack_t *move_stack, character_t *character, vertex_t **thither, uint16_t distance[W_HEIGHT][W_WIDTH])
{
    heap_t move_heap;
    heap_init(&move_heap);
    while (stack_size(move_stack) > 0) {
        stack_pop(move_stack, (void**) thither);
        heap_add(&move_heap, *thither, distance[(*thither)->ypos][(*thither)->xpos]);
    }
    heap_extract_min(&move_heap, (void**) thither);
    heap_delete(&move_heap);
}

void dumb_move(stack_t *move_stack, character_t *character, character_t *player, vertex_t *hither, vertex_t **thither)
{
    int i = 1;
    custom_vector_t direction;
    direction.xpos = (int) (player->xpos) - (int) (character->xpos);
    direction.ypos = (int) (player->ypos) - (int) (character->ypos);
    direction.xpos = direction.xpos > 1 ? 1 : direction.xpos;
    direction.xpos = direction.xpos < -1 ? -1 : direction.xpos;
    direction.ypos = direction.ypos > 1 ? 1 : direction.ypos;
    direction.ypos = direction.ypos < -1 ? -1 : direction.ypos;
    while (stack_size(move_stack) > 0) {
        stack_pop(move_stack, (void**) thither);
        if (((*thither)->xpos == (direction.xpos + hither->xpos)) && ((*thither)->ypos == (direction.ypos + hither->ypos))) {
            i = 0;
            break;
        }
        free(*thither);
    }
    if (i) {
        *thither = (vertex_t *) malloc(sizeof(vertex_t));
        (*thither)->xpos = hither->xpos;
        (*thither)->ypos = hither->ypos;
    }
}

void random_move(stack_t *move_stack, vertex_t **thither)
{
    int i;
    for (i = rand() % stack_size(move_stack); i > 0; i--) {
        stack_pop(move_stack, (void**) thither);
        free(*thither);
    }
    stack_pop(move_stack, (void**) thither);
}

void tunnel(uint8_t hardness[W_HEIGHT][W_WIDTH], character_t *player, uint16_t floor_distance[W_HEIGHT][W_WIDTH], uint16_t tunnel_distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], vertex_t *thither)
{
    hardness[thither->ypos][thither->xpos] -= hardness[thither->ypos][thither->xpos] < 85 ? hardness[thither->ypos][thither->xpos] : 85;
    dijkstra(hardness, map, player, tunnel_distance, 1);
    if (hardness[thither->ypos][thither->xpos] == 0) {
        map[thither->ypos][thither->xpos] =  '#';
        dijkstra(hardness, map, player, floor_distance, 0);
    }
    else {
        map[thither->ypos][thither->xpos] =  ' ';
    }
}

void move_character(character_t *player, character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], character_t *character, vertex_t *hither, vertex_t *thither, uint16_t *num_monsters)
{
    uint8_t i;
    int dmg, cmd;
    character_t *other = character_map[thither->ypos][thither->xpos];
    if (other != NULL && other != character) {
        dmg = character->entry->damage.toss();
        for (i = 0; i < OTHER; i++) {
            if (character->equipment[i] != NULL && character->equipment[i]->entry->type != RANGED) {
                dmg += character->equipment[i]->entry->damage_bonus.toss();
            }
        }
        for (i = 0; i < OTHER; i++) {
            if (other->equipment[i] != NULL) {
                dmg -= other->equipment[i]->defence_bonus;
            }
        }
        dmg = dmg > 0 ? dmg : 0;
        other->hitpoints -= dmg;
        mvprintw(0, 0, "%s hit %s for %d, who now has %d hitpoints.",
                character->entry->name.c_str(), other->entry->name.c_str(), dmg, other->hitpoints);
        refresh();
        if (!(player->data & QUIT)) {
            cmd = getch();
            if (cmd == 'Q')
                player->data |= QUIT;
        }
        mvprintw(0, 0, "                                                                                ");
        if (other->hitpoints < 0) {
            mvprintw(0, 0, "%s has died to %s.", other->entry->name.c_str(), character->entry->name.c_str());
            refresh();
            if (!player->data & QUIT) {
                cmd = getch();
                if (cmd == 'Q')
                    player->data |= QUIT;
            }
            mvprintw(0, 0, "                                                                                ");
            if (character_map[thither->ypos][thither->xpos]->symbol != '@')
                *num_monsters -= 1;
            if (other->data & BOSS)
                character->data |= KILL_BOSS;
            if (character_map[hither->ypos][hither->xpos] == character)
                character_map[hither->ypos][hither->xpos] = NULL;
            for (i = 0; i < 10; i++) {
                if (other->inventory[i] != NULL) {
                    item_map[other->ypos][other->xpos].push_back(other->inventory[i]);
                    other->inventory[i] = NULL;
                }
            }
            for (i = 0; i < OTHER; i++) {
                if (other->equipment[i] != NULL) {
                    item_map[other->ypos][other->xpos].push_back(other->equipment[i]);
                    other->equipment[i] = NULL;
                }
            }
            character_map[thither->ypos][thither->xpos] = character;
            character->xpos = thither->xpos;
            character->ypos = thither->ypos;
        }
    }
    else {
        if (character_map[hither->ypos][hither->xpos] == character)
            character_map[hither->ypos][hither->xpos] = NULL;
        character_map[thither->ypos][thither->xpos] = character;
        character->xpos = thither->xpos;
        character->ypos = thither->ypos;
    }
}

int shoot(character_t *player, character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], character_t *shooter, character_t *shootee, uint16_t *num_monsters) {
    uint8_t i;
    int cmd, dmg;
    item_t *weapon = shooter->equipment[RANGED];
    item_t *ammo = shooter->equipment[AMMO];
    if (weapon == NULL)
        return 1;
    if (ammo == NULL)
        return 2;
    dmg = weapon->entry->damage_bonus.toss();
    dmg += ammo->entry->damage_bonus.toss();
    dmg = dmg > 0 ? dmg : 0;
    shootee->hitpoints -= dmg;
    mvprintw(0, 0, "%s shot %s for %d, who now has %d hitpoints.",
            shooter->entry->name.c_str(), shootee->entry->name.c_str(), dmg, shootee->hitpoints);
    refresh();
    if (!(player->data & QUIT)) {
        cmd = getch();
        if (cmd == 'Q')
            player->data |= QUIT;
    }
    mvprintw(0, 0, "                                                                                ");
    if (shootee->hitpoints < 0) {
        mvprintw(0, 0, "%s has died to %s.", shootee->entry->name.c_str(), shooter->entry->name.c_str());
        refresh();
        if (!player->data & QUIT) {
            cmd = getch();
            if (cmd == 'Q')
                player->data |= QUIT;
        }
        mvprintw(0, 0, "                                                                                ");
        if (shootee != player)
            *num_monsters -= 1;
        if (shootee->data & BOSS)
            shooter->data |= KILL_BOSS;
        if (character_map[shootee->ypos][shootee->xpos] == shootee)
            character_map[shootee->ypos][shootee->xpos] = NULL;
        for (i = 0; i < 10; i++) {
            if (shootee->inventory[i] != NULL) {
                item_map[shootee->ypos][shootee->xpos].push_back(shootee->inventory[i]);
                shootee->inventory[i] = NULL;
            }
        }
        for (i = 0; i < OTHER; i++) {
            if (shootee->equipment[i] != NULL) {
                item_map[shootee->ypos][shootee->xpos].push_back(shootee->equipment[i]);
                shootee->equipment[i] = NULL;
            }
        }
    }
    if ((ammo->attributes -= 1) <= 0) {
        mvprintw(0, 0, "%s has run out of ammunition.", shooter->entry->name.c_str());
        refresh();
        if (!player->data & QUIT) {
            cmd = getch();
            if (cmd == 'Q')
                player->data |= QUIT;
        }
        mvprintw(0, 0, "                                                                                ");
        player->equipment[AMMO] = NULL;
        free(ammo);
    }
    return 0;
}

std::string equipment_num_to_slot(int num)
{
    switch(num) {
        case 0: return "WEAPON";
        case 1: return "OFFHAND";
        case 2: return "RANGED";
        case 3: return "AMMUNITION";
        case 4: return "ARMOR";
        case 5: return "HELMET";
        case 6: return "CLOAK";
        case 7: return "GLOVES";
        case 8: return "BOOTS";
        case 9: return "AMULET";
        case 10: return "LIGHT";
        case 11: return "RING1";
        case 12: return "RING2";
        default: return "OTHER";
    }
    return NULL;
}

void draw_inventory(character_t *player)
{
    uint8_t i;
    for (i = 0; i < 13; i++) {
        mvprintw(4 + i, 20, "                                        ");
    }
    mvprintw(5, 20, "Number of Items: %d", player->num_items);
    for (i = 0; i < 10; i++) {
        mvprintw(6 + i, 23, "%2d) ", i);
        if (player->inventory[i] != NULL) {
            mvprintw(6 + i, 27, "%s", player->inventory[i]->entry->name.c_str());
        }
        else {
            mvprintw(6 + i, 27, "(none)");
        }
    }
}

void draw_equipment(character_t *player)
{
    uint8_t i;
    for (i = 0; i < 17; i++) {
        mvprintw(3 + i, 20, "                                        ");
    }
    mvprintw(4, 23, "Speed: %d", player->speed);
    mvprintw(5, 23, "HP: %d", player->hitpoints);
    for (i = 0; i < OTHER; i++) {
        if (player->equipment[i] != NULL && i == AMMO) {
            mvprintw(6 + i, 23, "%10s) %s: %d", equipment_num_to_slot(i).c_str(), player->equipment[i]->entry->name.c_str(), player->equipment[i]->attributes);
        }
        else if (player->equipment[i] != NULL) {
            mvprintw(6 + i, 23, "%10s) %s", equipment_num_to_slot(i).c_str(), player->equipment[i]->entry->name.c_str());
        }
        else {
            mvprintw(6 + i, 23, "%10s) (none)", equipment_num_to_slot(i).c_str());
        }
    }
}

void prompt_inventory(character_t *player, uint8_t *i)
{
    int cmd;
    *i = 0;
    do {
        draw_inventory(player);
        mvprintw(6 + *i, 21, ">");
        refresh();
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case KEY_UP:
            case 'k':
                if (*i > 0)
                    *i -= 1;
                player->data |= RESET;
                break;
            case KEY_DOWN:
            case 'j':
                if (*i < 9)
                    *i += 1;
                player->data |= RESET;
                break;
            case 's':
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                player->data |= RESET;
                break;
        }
    } while (player->data & RESET);
    player->data &= ~RESET;
}

void prompt_equipment(character_t *player, uint8_t *i)
{
    int cmd;
    *i = 0;
    do {
        draw_equipment(player);
        mvprintw(6 + *i, 21, ">");
        refresh();
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case KEY_UP:
            case 'k':
                if (*i > 0)
                    *i -= 1;
                player->data |= RESET;
                break;
            case KEY_DOWN:
            case 'j':
                if (*i < 11)
                    *i += 1;
                player->data |= RESET;
                break;
            case 's':
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                player->data |= RESET;
                break;
        }
    } while (player->data & RESET);
    player->data &= ~RESET;
}

void display_item(character_t *player, item_t *item)
{
    uint16_t i, j, l, m, num;
    int cmd;
    num = item->entry->type == AMMO ? 9 : 8;
    j = 0;
    l = item->entry->description.size() > 21 - num ? 21 : item->entry->description.size() + num;
    m = item->entry->description.size() > 21 - num ? item->entry->description.size() - 21 - num : 0;
    do {
        for (i = 0; i < 21; i++)
            mvprintw(i, 0, "                                                                                ");
        for (i = -j; i < l - j; i++) {
            if (i == 0)
                mvprintw(i + j, 0, "%s", item->entry->name.c_str());
            else if (i == 1)
                mvprintw(i + j, 0, "Speed Bonus: %d", item->speed_bonus);
            else if (i == 2)
                mvprintw(i + j, 0, "HP Bonus: %d", item->hitpoint_bonus);
            else if (i == 3)
                mvprintw(i + j, 0, "Dodge Bonus: %d", item->dodge_bonus);
            else if (i == 4)
                mvprintw(i + j, 0, "Defence Bonus: %d", item->defence_bonus);
            else if (i == 5)
                mvprintw(i + j, 0, "Damage Bonus: %d+%dd%d", item->entry->damage_bonus.base, item->entry->damage_bonus.num, item->entry->damage_bonus.sides);
            else if (i == 6)
                mvprintw(i + j, 0, "Light: %d", item->light);
            else if (i == 7)
                mvprintw(i + j, 0, "Weight: %d", item->weight);
            else if (i == 8)
                mvprintw(i + j, 0, "Value: %d", item->value);
            else if (i == 9 && item->entry->type == AMMO)
                mvprintw(i + j, 0, "Num: %d", item->attributes);
            else
                mvprintw(i + j, 0, "%s", item->entry->description[i-num].c_str());
            refresh();
        }
        refresh();
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case KEY_UP:
            case 'k':
                if (j > -m)
                    j -= 1;
                player->data |= RESET;
                break;
            case KEY_DOWN:
            case 'j':
                if (j < 0)
                    j += 1;
                player->data |= RESET;
                break;
            case 's':
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                break;
        }
    } while (player->data & RESET);
    mvprintw(0, 0, "                                                                                ");
}

void display_character(character_t *player, character_t *character)
{
    if (character == NULL) {
        mvprintw(0, 0, "character is NULL");
        refresh();
        getch();
        mvprintw(0, 0, "                                                                                ");
    }
    if (character->entry == NULL) {
        mvprintw(0, 0, "character entry is NULL");
        refresh();
        getch();
        mvprintw(0, 0, "                                                                                ");
    }
    uint16_t i, l, m;
    int j;
    int cmd;
    j = 0;
    l = character->entry->description.size() > 16 ? 21 : character->entry->description.size() + 5;
    m = character->entry->description.size() > 16 ? character->entry->description.size() - 16 : 0;
    do {
        for (i = 0; i < 21; i++)
            mvprintw(i, 0, "                                                                                ");
        for (i = -j; i < l - j; i++) {
            if (i == 0)
                mvprintw(i + j, 0, "%s", character->entry->name.c_str());
            else if (i == 1)
                mvprintw(i + j, 0, "Speed: %d", character->speed);
            else if (i == 2)
                mvprintw(i + j, 0, "HP: %d", character->hitpoints);
            else if (i == 3)
                mvprintw(i + j, 0, "Damage: %d+%dd%d", character->entry->damage.base, character->entry->damage.num, character->entry->damage.sides);
            else if (i == 4)
                mvprintw(i + j, 0, "Number of Items: %d", character->num_items);
            else
                mvprintw(i + j, 0, "%s", character->entry->description[i-5].c_str());
            refresh();
        }
        refresh();
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case KEY_UP:
            case 'k':
                if (j > -m)
                    j -= 1;
                player->data |= RESET;
                break;
            case KEY_DOWN:
            case 'j':
                if (j < 0)
                    j += 1;
                player->data |= RESET;
                break;
            case 's':
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                break;
        }
    } while (player->data & RESET);
    mvprintw(0, 0, "                                                                                ");
}

void prompt_place(character_t *player, vertex_t *thither, char map[W_HEIGHT][W_WIDTH], char visible_map[W_HEIGHT][W_WIDTH], character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], const char c)
{
    int cmd;
    player->data &= ~ESCAPE;
    do {
        sketch_visible_map(visible_map, map, player);
        if (player->data & FOG)
            draw_fog(visible_map, character_map, item_map, player);
        else
            draw_full(map, character_map, item_map);
        mvprintw(thither->ypos + 1, thither->xpos, "*");
        refresh();
        cmd = getch();
        player->data &= ~RESET;
        switch(cmd) {
            case '7':
            case 'y':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                if (thither->ypos > 1)
                    thither->ypos -= 1;
                player->data |= RESET;
                break;
            case '8':
            case 'k':
                if (thither->ypos > 1)
                    thither->ypos -= 1;
                player->data |= RESET;
                break;
            case '9':
            case 'u':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                if (thither->ypos > 1)
                    thither->ypos -= 1;
                player->data |= RESET;
                break;
            case '6':
            case 'l':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                player->data |= RESET;
                break;
            case '3':
            case 'n':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                if (thither->ypos < W_HEIGHT - 2)
                    thither->ypos += 1;
                player->data |= RESET;
                break;
            case '2':
            case 'j':
                if (thither->ypos < W_HEIGHT - 2)
                    thither->ypos += 1;
                player->data |= RESET;
                break;
            case '1':
            case 'b':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                if (thither->ypos < W_HEIGHT - 2)
                    thither->ypos += 1;
                player->data |= RESET;
                break;
            case '4':
            case 'h':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                player->data |= RESET;
                break;
            case 's':
                break;
            case 'f':
                if (player->data & FOG) {
                    player->data &= ~FOG;
                    draw_full(map, character_map, item_map);
                }
                else {
                    player->data |= FOG;
                    draw_fog(visible_map, character_map, item_map, player);
                }
                refresh();
                player->data |= RESET;
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                if (cmd != c)
                    player->data |= RESET;
        }
    } while (player->data & RESET);
}

void monster_list(character_t *character_map[W_HEIGHT][W_WIDTH], character_t *player, uint16_t num_init_monsters)
{
    uint16_t i, j, k;
    int8_t vertical_amount, horizontal_amount;
    char vertical[6], horizontal[5];
    character_t **monster_list = (character_t **) malloc(sizeof(character_t) * num_init_monsters);
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (character_map[i][j] != NULL && character_map[i][j] != player) {
                monster_list[k++] = character_map[i][j];
            }
        }
    }
    k = 0;
    do {
        for (i = 3; i < W_WIDTH - 1; i++) {
            mvprintw(i, 22, "                                  ");
        }
        for (i = 0; i < (num_init_monsters - k < 15 ? num_init_monsters - k : 15); i++) {
            monster_list[i + k]->ypos <= player->ypos ? sprintf(vertical, "north") : sprintf(vertical, "south");
            vertical_amount = monster_list[i + k]->ypos - player->ypos;
            vertical_amount = vertical_amount > 0 ? vertical_amount : -vertical_amount;
            monster_list[i + k]->xpos < player->xpos ? sprintf(horizontal, "west") : sprintf(horizontal, "east");
            horizontal_amount = monster_list[i + k]->xpos - player->xpos;
            horizontal_amount = horizontal_amount > 0 ? horizontal_amount : -horizontal_amount;
            mvprintw(i + 4, 26, "%c at %d %s and %d %s", monster_list[i + k]->symbol, vertical_amount, vertical, horizontal_amount, horizontal);
        }
        refresh();
        monster_list_command(player);
        if (player->data & WALK_UP) {
            k -= 0 < k ? 1 : 0;
        }
        if (player->data & WALK_DOWN) {
            k += k + 15 < num_init_monsters ? 1 : 0;
        }
        player->data &= ~(WALK_UP | WALK_DOWN);
    } while (!(player->data & ESCAPE) && !(player->data & QUIT));
    free(monster_list);
}

void monster_list_command(character_t *player)
{
    int cmd;
    player->data &= ~(WALK_UP & WALK_DOWN);
    player->data &= ~ESCAPE;
    do {
        cmd = getch();
        player->data &= ~RESET;
        switch (cmd) {
            case KEY_UP:
            case 'k':
                player->data |= WALK_UP;
                break;
            case KEY_DOWN:
            case 'j':
                player->data |= WALK_DOWN;
                break;
            /* ESCAPE */
            case 27:
                player->data |= ESCAPE;
                break;
            case 'Q':
                player->data |= QUIT;
                break;
            default:
                player->data |= RESET;
                break;
        }
    } while (player->data & RESET);
}

void place(char map[W_HEIGHT][W_WIDTH], entity_t *entities, uint16_t count)
{
    uint16_t i;
    for (i = 0; i < count; i++) {
        map[entities[i].ypos][entities[i].xpos] = entities[i].symbol;
    }
}

void place_characters(char map[W_HEIGHT][W_WIDTH], character_t *characters[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (characters[i][j] != NULL)
                map[i][j] = characters[i][j]->symbol;
        }
    }
}

void sketch_visible_map(char visible_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], character_t *player)
{
    uint8_t i, j;
    custom_vector_t v;
    uint32_t light = 2;
    for (i = 0; i < OTHER; i++) {
        if (player->equipment[i] != NULL) {
            light += player->equipment[i]->light;
        }
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            v.ypos = i - player->ypos;
            v.xpos = j - player->xpos;
            v.ypos *= v.ypos < 0 ? -1 : 1;
            v.xpos *= v.xpos < 0 ? -1 : 1;
            if (v.xpos <= light && v.ypos <= light) {
                visible_map[i][j] = map[i][j];
            }
        }
    }
}

void print_monster_dictionary(std::vector<character_template_t> monster_dictionary)
{
    int i;
    std::vector<character_template_t>::iterator it;
    for (it = monster_dictionary.begin(); it != monster_dictionary.end(); it++) {
        std::cout << "Begin Monster:" << std::endl;
        std::cout << "Name: " << (*it).name << std::endl;
        std::cout << "Symbol: " << (*it).symbol << std::endl;
        std::cout << "Color:";
        if ((*it).color & 0x01)
            std::cout << " Black";
        if ((*it).color & 0x02)
            std::cout << " Red";
        if ((*it).color & 0x04)
            std::cout << " Green";
        if ((*it).color & 0x08)
            std::cout << " Yellow";
        if ((*it).color & 0x10)
            std::cout << " Blue";
        if ((*it).color & 0x20)
            std::cout << " Magenta";
        if ((*it).color & 0x40)
            std::cout << " Cyan";
        if ((*it).color & 0x80)
            std::cout << " White";
        std::cout << std::endl;
        std::cout << "Description:"; 
        for (i = 0; !((*it).description[i] == std::string(".")); i++) {
            std::cout << (*it).description[i] << std::endl;
        }
        std::cout << "Speed: " << (*it).speed.base << " + " << (*it).speed.num << " d" << (*it).speed.sides << std::endl;
        std::cout << "Abilities:";
        if ((*it).abilities & SMART)
            std::cout << " Smart";
        if ((*it).abilities & TELE)
            std::cout << " Telepathic";
        if ((*it).abilities & TUNNEL)
            std::cout << " Tunneling";
        if ((*it).abilities & ERRATIC)
            std::cout << " Erratic";
        if ((*it).abilities & PASS)
            std::cout << " Non-Corporial";
        if ((*it).abilities & PICKUP)
            std::cout << " Able To Pick Up Items";
        if ((*it).abilities & DESTROY)
            std::cout << " Destroys Items";
        if ((*it).abilities & UNIQ)
            std::cout << " Unique";
        std::cout << std::endl;
        std::cout << "HP: " << (*it).hitpoints.base << " + " << (*it).hitpoints.num << " d" << (*it).hitpoints.sides << std::endl;
        std::cout << "Damage: " << (*it).damage.base << " + " << (*it).damage.num << " d" << (*it).damage.sides << std::endl;
        std::cout << "Rarity: " << (int) (*it).rarity << std::endl;
        std::cout << "End Monster" << std::endl;
        std::cout << std::endl;
    }
}

void print_item_dictionary(std::vector<item_template_t> item_dictionary)
{
    int i;
    std::vector<item_template_t>::iterator it;
    for (it = item_dictionary.begin(); it != item_dictionary.end(); it++) {
        std::cout << "Begin Item:" << std::endl;
        std::cout << "Name: " << (*it).name << std::endl;
        std::cout << "Type: " << (*it).symbol << std::endl;
        if ((*it).symbol == '*')
            std::cout << "Invalid" << std::endl;
        else if ((*it).symbol == '|')
            std::cout << "Weapon" << std::endl;
        else if ((*it).symbol == ')')
            std::cout << "Offhand" << std::endl;
        else if ((*it).symbol == '}')
            std::cout << "Ranged" << std::endl;
        else if ((*it).symbol == '[')
            std::cout << "Armor" << std::endl;
        else if ((*it).symbol == ']')
            std::cout << "Helmet" << std::endl;
        else if ((*it).symbol == '(')
            std::cout << "Cloak" << std::endl;
        else if ((*it).symbol == '{')
            std::cout << "Gloves" << std::endl;
        else if ((*it).symbol == '\\')
            std::cout << "Boots" << std::endl;
        else if ((*it).symbol == '=')
            std::cout << "Ring" << std::endl;
        else if ((*it).symbol == '"')
            std::cout << "Amulet" << std::endl;
        else if ((*it).symbol == '_')
            std::cout << "Light" << std::endl;
        else if ((*it).symbol == '~')
            std::cout << "Scroll" << std::endl;
        else if ((*it).symbol == '?')
            std::cout << "Book" << std::endl;
        else if ((*it).symbol == '!')
            std::cout << "Flask" << std::endl;
        else if ((*it).symbol == '$')
            std::cout << "Gold" << std::endl;
        else if ((*it).symbol == '/')
            std::cout << "Ammunition" << std::endl;
        else if ((*it).symbol == ',')
            std::cout << "Food" << std::endl;
        else if ((*it).symbol == '-')
            std::cout << "Wand" << std::endl;
        else if ((*it).symbol == '%')
            std::cout << "Container" << std::endl;
        std::cout << "Color:";
        if ((*it).color & 0x01)
            std::cout << " Black";
        if ((*it).color & 0x02)
            std::cout << " Red";
        if ((*it).color & 0x04)
            std::cout << " Green";
        if ((*it).color & 0x08)
            std::cout << " Yellow";
        if ((*it).color & 0x10)
            std::cout << " Blue";
        if ((*it).color & 0x20)
            std::cout << " Magenta";
        if ((*it).color & 0x40)
            std::cout << " Cyan";
        if ((*it).color & 0x80)
            std::cout << " White";
        std::cout << std::endl;
        std::cout << "Description:"; 
        for (i = 0; !((*it).description[i] == std::string(".")); i++) {
            std::cout << (*it).description[i] << std::endl;
        }
        std::cout << "Speed: " << (*it).speed_bonus.base << " + " << (*it).speed_bonus.num << " d" << (*it).speed_bonus.sides << std::endl;
        std::cout << "Attributes: " << (*it).attributes.base << " + " << (*it).attributes.num << " d" << (*it).attributes.sides << std::endl;
        std::cout << "HP Bonus: " << (*it).hitpoint_bonus.base << " + " << (*it).hitpoint_bonus.num << " d" << (*it).hitpoint_bonus.sides << std::endl;
        std::cout << "Damage Bonus: " << (*it).damage_bonus.base << " + " << (*it).damage_bonus.num << " d" << (*it).damage_bonus.sides << std::endl;
        std::cout << "Dodge Bonus: " << (*it).dodge_bonus.base << " + " << (*it).dodge_bonus.num << " d" << (*it).dodge_bonus.sides << std::endl;
        std::cout << "Defence Bonus: " << (*it).defence_bonus.base << " + " << (*it).defence_bonus.num << " d" << (*it).defence_bonus.sides << std::endl;
        std::cout << "Weight: " << (*it).weight.base << " + " << (*it).weight.num << " d" << (*it).weight.sides << std::endl;
        std::cout << "Value: " << (*it).value.base << " + " << (*it).value.num << " d" << (*it).value.sides << std::endl;
        std::cout << "Rarity: " << (int) (*it).rarity << std::endl;
        std::cout << "End Item" << std::endl;
        std::cout << std::endl;
    }
}

void draw_distance(uint16_t distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (distance[i][j] == 0xFFFF && map[i][j] == ' ')
                mvprintw(i + 1, j, " ");
            else if (distance[i][j] == 0xFFFF && map[i][j] != ' ')
                mvprintw(i + 1, j, "X");
            else if (distance[i][j] == 0)
                mvprintw(i + 1, j, "@");
            else
                mvprintw(i + 1, j, "%c", (distance[i][j] % 10) + '0');
        }
    }
}

void print_message(const char *str)
{
    mvprintw(0, 0, "%s", str);
    refresh();
    getch();
    mvprintw(0, 0, "                                                                                ");
}

void draw_fog(char map[W_HEIGHT][W_WIDTH], character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH], character_t *player)
{
    int i, j, k, l;
    int light = 2;
    for (i = 0; i < OTHER; i++) {
        if (player->equipment[i] != NULL) {
            light += player->equipment[i]->light;
        }
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if ((i >= player->ypos - light && i <= player->ypos + light &&
                        j >= player->xpos - light && j <= player->xpos + light)) {
                if (character_map[i][j] != NULL) {
                    for (k = 0, l = 1; (l & character_map[i][j]->color) == 0 && k < 7; k++, l *= 2);
                    attron(COLOR_PAIR(k));
                    mvprintw(i + 1, j, "%c", character_map[i][j]->symbol);
                    attroff(COLOR_PAIR(k));
                }
                else if (item_map[i][j].size() == 1) {
                    for (k = 0, l = 1; (l & item_map[i][j].back()->color) == 0 && k < 7; k++, l *= 2);
                    attron(COLOR_PAIR(k));
                    mvprintw(i + 1, j, "%c", item_map[i][j].back()->symbol);
                    attroff(COLOR_PAIR(k));
                }
                else if (item_map[i][j].size() > 1) {
                    for (k = 0, l = 1; (l & item_map[i][j].back()->color) == 0 && k < 7; k++, l *= 2);
                    attron(COLOR_PAIR(k));
                    mvprintw(i + 1, j, "&");
                    attroff(COLOR_PAIR(k));
                }
                else {
                    mvprintw(i + 1, j, "%c", map[i][j]);
                }
            }
            else {
                mvprintw(i + 1, j, "%c", map[i][j]);
            }
        }
    }
}

void draw_full(char map[W_HEIGHT][W_WIDTH], character_t *character_map[W_HEIGHT][W_WIDTH], std::vector<item_t *> item_map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j, k, l;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (character_map[i][j] != NULL) {
                for (k = 0, l = 1; (l & character_map[i][j]->color) == 0 && k < 7; k++, l *= 2);
                attron(COLOR_PAIR(k));
                mvprintw(i + 1, j, "%c", character_map[i][j]->symbol);
                attroff(COLOR_PAIR(k));
            }
            else if (item_map[i][j].size() == 1) {
                for (k = 0, l = 1; (l & item_map[i][j].back()->color) == 0 && k < 7; k++, l *= 2);
                attron(COLOR_PAIR(k));
                mvprintw(i + 1, j, "%c", item_map[i][j].back()->symbol);
                attroff(COLOR_PAIR(k));
            }
            else if (item_map[i][j].size() > 1) {
                for (k = 0, l = 1; (l & item_map[i][j].back()->color) == 0 && k < 7; k++, l *= 2);
                attron(COLOR_PAIR(k));
                mvprintw(i + 1, j, "&");
                attroff(COLOR_PAIR(k));
            }
            else {
                mvprintw(i + 1, j, "%c", map[i][j]);
            }
        }
    }
}

void draw_quit() {
    mvprintw(11, 30, "Thou hast quit");
}

void draw_win() {
    mvprintw(11, 30, "Thou hast won");
}

void draw_loose() {
    mvprintw(11, 30, "Thou hast lost");
}
