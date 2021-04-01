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
#include "stack.h"
#include "heap.h"
#include "dice.h"

#define W_WIDTH 80
#define W_HEIGHT 21

#define SAVE 0x01
#define LOAD 0x02
#define NUM_MON 0x04

#define SMART 0x0001
#define TELE 0x0002
#define TUNNEL 0x0004
#define ERRATIC 0x0008
#define PASS 0x0010
#define PICKUP 0x0020
#define DESTROY 0x0040
#define UNIQ 0x0080
#define BOSS 0x0100

/* Used for Logic */
#define ALIVE 0x0200
#define KNOWN 0x0400
#define SEE 0x0800

#define QUIT 0x01
#define WALK_DOWN 0x02
#define WALK_UP 0x04
#define RESET 0x08
/* ALIVE is reserved */
#define MON_LIST 0x20
#define ESCAPE 0x40
#define FOG 0x80
#define TELEPORT 0x100

typedef class room {
    public:
        uint8_t xpos;
        uint8_t ypos;
        uint8_t xsize;
        uint8_t ysize;
} room_t;

typedef class custom_vector {
    public:
        int xpos;
        int ypos;
} custom_vector_t;

typedef struct monster_template {
    public:
        std::string name;
        char symbol;
        uint8_t color;
        char *description;
        Dice_t speed;
        uint16_t abilities;
        Dice_t hitpoints;
        Dice_t damage;
        uint8_t rarity;
} monster_template_t;

typedef class character {
    public:
        uint8_t xpos;
        uint8_t ypos;
        char symbol;
        uint16_t sequence;
        uint16_t characteristics;
        uint8_t speed;
        vertex_t known_location;
} character_t;

int real_main(int argc, char *argv[]);

void get_args(int argc, char *argv[], uint8_t *args, uint16_t *num_init_monsters);

void generate(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, character_t **u_stairs, uint16_t *num_d_stairs, character_t **d_stairs);

void save(character_t *player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, character_t *u_stairs, uint16_t num_d_stairs, character_t *d_stairs);
void load(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, character_t **u_stairs, uint16_t *num_d_stairs, character_t **d_stairs);

int load_monster_dictionary(std::vector<monster_template_t> *monster_dictionary);
void print_monster_dictionary(std::vector<monster_template_t> monster_dictionary);

void map_rooms(char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms);

void init_ncurses();
void deinit_ncurses();

void init_map(char map[W_HEIGHT][W_WIDTH], char c);
void sketch_map(char display[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH]);

void init_character_map(character_t *character_map[W_HEIGHT][W_WIDTH]);
void init_player(heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], character_t *player);
void generate_monsters(heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], room_t *rooms, uint16_t num_rooms, uint16_t num_init_monsters);

void dijkstra(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], character_t *player, uint16_t distance[W_HEIGHT][W_WIDTH], uint8_t tunnel);
void get_neighbors(stack_t *s, vertex_t *v, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint8_t tunnel);

void derive_next_turn_heap(heap_t *characters, heap_t *next_turn, uint32_t *priority);
void derive_move_stack(stack_t *move_stack, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], character_t *character, vertex_t *hither, vertex_t **thither);

void player_command(character_t *player, vertex_t *thither, const char visible_map[W_HEIGHT][W_WIDTH], const char display[W_HEIGHT][W_WIDTH]);
void monster_turn(stack_t *move_stack, character_t *character_map[W_HEIGHT][W_WIDTH], character_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t floor_distance[W_HEIGHT][W_WIDTH], uint16_t tunnel_distance[W_HEIGHT][W_WIDTH], vertex_t *hither, vertex_t **thither, uint16_t *num_monsters);

void line_of_sight(character_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH]);
void smart_move(stack_t *move_stack, character_t *character, vertex_t **thither, uint16_t distance[W_HEIGHT][W_WIDTH]);
void dumb_move(stack_t *move_stack, character_t *character, character_t *player, vertex_t *hither, vertex_t **thither);
void random_move(stack_t *move_stack, vertex_t **thither);

void tunnel(uint8_t hardness[W_HEIGHT][W_WIDTH], character_t *player, uint16_t floor_distance[W_HEIGHT][W_WIDTH], uint16_t tunnel_distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], vertex_t *thither);
void move_character(character_t *character_map[W_HEIGHT][W_WIDTH], character_t *character, vertex_t *hither, vertex_t *thither, uint16_t *num_monsters);

void teleport_command(character_t *player, vertex_t *thither, char display[W_HEIGHT][W_WIDTH]);

void monster_list(character_t *character_map[W_HEIGHT][W_WIDTH], character_t *player, uint16_t num_init_monsters);
void monster_list_command(character_t *player);

void place(char display[W_HEIGHT][W_WIDTH], character_t *entities, uint16_t count);
void place_characters(char display[W_HEIGHT][W_WIDTH], character_t *characters[W_HEIGHT][W_WIDTH]);

void sketch_visible_map(char visible_map[W_HEIGHT][W_WIDTH], const char map[W_HEIGHT][W_WIDTH], const char display[W_HEIGHT][W_WIDTH], const character_t *player);

void draw_distance(uint16_t distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH]);
void draw(const char display[W_HEIGHT][W_WIDTH]);

void draw_quit();
void draw_win();
void draw_loose();

int main(int argc, char *argv[])
{
    std::vector<monster_template_t> monster_dictionary;
    int error = load_monster_dictionary(&monster_dictionary);
    if (error) {
        std::cout << "Load Failed: " << error << std::endl;
    }
    print_monster_dictionary(monster_dictionary);

    return 0;
}

int real_main(int argc, char *argv[])
{
    /* Initializing Ncurses */
    init_ncurses();

    /* Seeding */
    srand(time(NULL));

    /* Declarations */
    uint8_t args = 0x00;
    character_t *player;
    uint8_t hardness[W_HEIGHT][W_WIDTH];
    uint16_t floor_distance[W_HEIGHT][W_WIDTH];
    uint16_t tunnel_distance[W_HEIGHT][W_WIDTH];
    uint16_t num_rooms;
    room_t *rooms;
    uint16_t num_u_stairs;
    character_t *u_stairs;
    uint16_t num_d_stairs;
    character_t *d_stairs;
    uint16_t num_init_monsters;
    uint16_t num_monsters;
    heap_t characters;
    uint32_t priority;
    heap_t next_turn;
    character_t *character;
    vertex_t hither;
    vertex_t *thither;
    stack_t move_stack;
    character_t *character_map[W_HEIGHT][W_WIDTH];
    char map[W_HEIGHT][W_WIDTH];
    char display[W_HEIGHT][W_WIDTH];
    char visible_map[W_HEIGHT][W_WIDTH];

    /* Switch Processing */
    get_args(argc, argv, &args, &num_init_monsters);

    /* Saving and Loading */
    if (args & LOAD)
        load(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    else
        generate(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    if (args & SAVE)
        save(player, hardness, num_rooms, rooms, num_u_stairs, u_stairs, num_d_stairs, d_stairs);

    /* Sketching Map */
    init_map(map, ' ');
    init_map(visible_map, ' ');
    map_rooms(map, hardness, num_rooms, rooms);
    place(map, u_stairs, num_u_stairs);
    place(map, d_stairs, num_d_stairs);

    /* Generating Characters */
    heap_init(&characters);
    init_character_map(character_map);
    init_player(&characters, character_map, player);
    if (!(args & NUM_MON))
        num_init_monsters = 10;
    num_init_monsters = num_init_monsters < 1481 ? num_init_monsters : 1481;
    generate_monsters(&characters, character_map, map, rooms, num_rooms, num_init_monsters);
    num_monsters = num_init_monsters;

    /* Finding Initial Distances */
    dijkstra(hardness, map, player, floor_distance, 0);
    dijkstra(hardness, map, player, tunnel_distance, 1);

    /* Main Loop */
    while (player->characteristics & ALIVE && num_monsters && !(player->characteristics & QUIT)) {
        derive_next_turn_heap(&characters, &next_turn, &priority);
        /* Character Turn */
        while (heap_size(&next_turn) > 0 && !(player->characteristics & RESET)) {
            player->characteristics &= ~RESET;
            heap_extract_min(&next_turn, (void**) &character);
            if (character->characteristics & ALIVE) {
                if (character == player) {
                    do {
                        player->characteristics &= ~MON_LIST;
                        player->characteristics &= ~TELEPORT;
                        sketch_map(display, map);
                        place_characters(display, character_map);
                        sketch_visible_map(visible_map, map, display, player);
                        if (player->characteristics & FOG)
                            draw(visible_map);
                        else
                            draw(display);
                        refresh();
                        hither.xpos = player->xpos;
                        hither.ypos = player->ypos;
                        thither = (vertex_t *) malloc(sizeof(vertex_t));
                        player_command(player, thither, visible_map, display);
                        if (map[thither->ypos][thither->xpos] != ' ') {
                            move_character(character_map, character, &hither, thither, &num_monsters);
                        }
                        dijkstra(hardness, map, player, floor_distance, 0);
                        dijkstra(hardness, map, player, tunnel_distance, 1);
                        free(thither);
                        if (player->characteristics & MON_LIST) {
                            monster_list(character_map, player, num_monsters);
                        }
                        if ((player->characteristics & WALK_UP && player->xpos == u_stairs->xpos && player->ypos == u_stairs->ypos) ||
                            (player->characteristics & WALK_DOWN && player->xpos == d_stairs->xpos && player->ypos == d_stairs->ypos)) {
                            /* new floor */
                            free(player);
                            free(rooms);
                            free(u_stairs);
                            free(d_stairs);
                            heap_delete(&characters);
                            generate(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
                            player->characteristics |= RESET;
                            init_map(map, ' ');
                            init_map(visible_map, ' ');
                            map_rooms(map, hardness, num_rooms, rooms);
                            place(map, u_stairs, num_u_stairs);
                            place(map, d_stairs, num_d_stairs);
                            heap_init(&characters);
                            init_character_map(character_map);
                            init_player(&characters, character_map, player);
                            generate_monsters(&characters, character_map, map, rooms, num_rooms, num_init_monsters);
                            num_monsters = num_init_monsters;
                            dijkstra(hardness, map, player, floor_distance, 0);
                            dijkstra(hardness, map, player, tunnel_distance, 1);
                        }
                        else if (!(player->characteristics & (MON_LIST | TELEPORT) && !(player->characteristics & QUIT))){
                            heap_add(&characters, character, priority + 1000 / character->speed);
                        }
                        if (player->characteristics & TELEPORT) {
                            thither = (vertex_t *) malloc(sizeof(vertex_t));
                            thither->ypos = player->ypos;
                            thither->xpos = player->xpos;
                            teleport_command(player, thither, display);
                            if (!(player->characteristics & ESCAPE)) {
                                move_character(character_map, character, &hither, thither, &num_monsters);
                            }
                            free(thither);
                        }
                    } while (player->characteristics & (MON_LIST | TELEPORT) && !(player->characteristics & QUIT) && num_monsters);
                }
                else {
                    heap_add(&characters, character, priority + 1000 / character->speed);
                    derive_move_stack(&move_stack, map, hardness, character, &hither, &thither);
                    monster_turn(&move_stack, character_map, character, player, map, hardness, floor_distance, tunnel_distance, &hither, &thither, &num_monsters);
                    stack_delete(&move_stack);
                    free(thither);
                }
            }
            else {
                if (character_map[character->ypos][character->xpos] == character)
                    character_map[character->ypos][character->xpos] = NULL;
                free(character);
            }
        }
        heap_delete(&next_turn);
    }

    /* Printing Results */
    clear();
    if (player->characteristics & QUIT)
        draw_quit();
    else if (player->characteristics & ALIVE)
        draw_win();
    else
        draw_loose();

    /* Cleaning Memory */
    free(rooms);
    free(u_stairs);
    free(d_stairs);
    heap_delete(&characters);

    /* Waiting for a final character */
    getch();

    /* Deinitializing Ncurses */
    deinit_ncurses();

    return 0;
}

void get_args(int argc, char *argv[], uint8_t *args, uint16_t *num_init_monsters)
{
    uint8_t i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--save")) {
            *args |= SAVE;
        }
        else if (!strcmp(argv[i], "--load")) {
            *args |= LOAD;
        }
        else if (!strcmp(argv[i], "--nummon")) {
            *args |= NUM_MON;
            sscanf(argv[++i], "%hd", num_init_monsters);
        }
    }
}

void generate(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, character_t **u_stairs, uint16_t *num_d_stairs, character_t **d_stairs)
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
    *u_stairs = (character_t *) malloc(sizeof(character_t));
    (*u_stairs)->xpos = (*rooms)[2].xpos + 1;
    (*u_stairs)->ypos = (*rooms)[2].ypos + 1;
    (*u_stairs)->symbol = '<';
    *num_d_stairs = 1;
    *d_stairs = (character_t *) malloc(sizeof(character_t));
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

void save(character_t *player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, character_t *u_stairs, uint16_t num_d_stairs, character_t *d_stairs)
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

void load(character_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, character_t **u_stairs, uint16_t *num_d_stairs, character_t **d_stairs)
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
    *u_stairs = (character_t *) malloc(sizeof(character_t) * *num_u_stairs);
    for (i = 0; i < *num_u_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].ypos = r_byte;
        (*u_stairs)[i].symbol = '<';
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_d_stairs = be16toh(r_short);
    *d_stairs = (character_t *) malloc(sizeof(character_t) * *num_d_stairs);
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

int load_monster_dictionary(std::vector<monster_template_t> *monster_dictionary)
{
    uint16_t i, j, l, check;
    monster_template_t monster_template;
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
            monster_template.name = "";
            do {
                f >> s;
                if (s == "NAME") {
                    check |= 0x0001;
                    f >> s;
                    monster_template.name += s;
                    while (f.peek() != '\n') {
                        f >> s;
                        monster_template.name += " ";
                        monster_template.name += s;
                    }
                }
                else if (s == "SYMB") {
                    check |= 0x0002;
                    f >> s;
                    monster_template.symbol = s[0];
                }
                else if (s == "COLOR") {
                    check |= 0x0004;
                    monster_template.color = 0x00;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "BLACK")
                            monster_template.color |= 0x01;
                        else if (s == "RED")
                            monster_template.color |= 0x02;
                        else if (s == "GREEN")
                            monster_template.color |= 0x04;
                        else if (s == "YELLOW")
                            monster_template.color |= 0x08;
                        else if (s == "BLUE")
                            monster_template.color |= 0x10;
                        else if (s == "MAGENTA")
                            monster_template.color |= 0x20;
                        else if (s == "CYAN")
                            monster_template.color |= 0x40;
                        else if (s == "WHITE")
                            monster_template.color |= 0x80;
                        else
                            return 2;
                    }
                }
                else if (s == "DESC") {
                    check |= 0x0008;
                    l = 10;
                    monster_template.description = (char *) malloc(sizeof(char) * 78 * l);
                    for (j = 0; s != "."; j++) {
                        getline(f, s);
                        if (j >= l) {
                            l *= 2;
                            monster_template.description = (char *) realloc(monster_template.description, sizeof(char) * 78 * l);
                        }
                        for (i = 0; s[i] != '\0' && i < 77; i++) {
                            monster_template.description[j*78+i] = s[i];
                        }
                        monster_template.description[j*78+i] = '\0';
                    }
                }
                else if (s == "SPEED") {
                    check |= 0x0010;
                    f >> s;
                    i = 0;
                    monster_template.speed.base = 0;
                    monster_template.speed.num = 0;
                    monster_template.speed.sides = 0;
                    while (s[i] != '+') {
                        monster_template.speed.base *= 10;
                        monster_template.speed.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        monster_template.speed.num *= 10;
                        monster_template.speed.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        monster_template.speed.sides *= 10;
                        monster_template.speed.sides += s[i++] - '0';
                    }
                }
                else if (s == "DAM") {
                    check |= 0x0020;
                    f >> s;
                    i = 0;
                    monster_template.damage.base = 0;
                    monster_template.damage.num = 0;
                    monster_template.damage.sides = 0;
                    while (s[i] != '+') {
                        monster_template.damage.base *= 10;
                        monster_template.damage.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        monster_template.damage.num *= 10;
                        monster_template.damage.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        monster_template.damage.sides *= 10;
                        monster_template.damage.sides += s[i++] - '0';
                    }
                }
                else if (s == "HP") {
                    check |= 0x0040;
                    f >> s;
                    i = 0;
                    monster_template.hitpoints.base = 0;
                    monster_template.hitpoints.num = 0;
                    monster_template.hitpoints.sides = 0;
                    while (s[i] != '+') {
                        monster_template.hitpoints.base *= 10;
                        monster_template.hitpoints.base += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != 'd') {
                        monster_template.hitpoints.num *= 10;
                        monster_template.hitpoints.num += s[i++] - '0';
                    }
                    i++;
                    while (s[i] != '\0') {
                        monster_template.hitpoints.sides *= 10;
                        monster_template.hitpoints.sides += s[i++] - '0';
                    }
                }
                else if (s == "ABIL") {
                    check |= 0x0080;
                    monster_template.abilities = 0x0000;
                    while (f.peek() != '\n') {
                        f >> s;
                        if (s == "SMART")
                            monster_template.abilities |= SMART;
                        else if (s == "TELE")
                            monster_template.abilities |= TELE;
                        else if (s == "TUNNEL")
                            monster_template.abilities |= TUNNEL;
                        else if (s == "ERRATIC")
                            monster_template.abilities |= ERRATIC;
                        else if (s == "PASS")
                            monster_template.abilities |= PASS;
                        else if (s == "PICKUP")
                            monster_template.abilities |= PICKUP;
                        else if (s == "DESTROY")
                            monster_template.abilities |= DESTROY;
                        else if (s == "UNIQ")
                            monster_template.abilities |= UNIQ;
                        else if (s == "BOSS")
                            monster_template.abilities |= BOSS;
                        else
                            return 3;
                    }
                }
                else if (s == "RRTY") {
                    check |= 0x0100;
                    f >> s;
                    i = 0;
                    monster_template.rarity = 0;
                    while (s[i] != '\0') {
                        monster_template.rarity *= 10;
                        monster_template.rarity += s[i++] - '0';
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
            (*monster_dictionary).push_back(monster_template);
        }
    } while (!f.eof());
    return 0;
}

void print_monster_dictionary(std::vector<monster_template_t> monster_dictionary)
{
    int i, j;
    std::vector<monster_template_t>::iterator it;
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
        for (j = 0; !((*it).description[j*78] == '.' && (*it).description[j*78+1] == '\0'); j++) {
            for (i = 0; (*it).description[j*78+i] != '\0'; i++) {
                std::cout << (char) (*it).description[j*78+i];
            }
            std::cout << std::endl;
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
        /* display[rooms[i].ypos][rooms[i].xpos] = '0' + i; */
    }
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (hardness[i][j] == 0 && map[i][j] == ' ') {
                map[i][j] = '#';
            }
        }
    }
}

void init_ncurses()
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
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

void sketch_map(char display[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            display[i][j] = map[i][j];
        }
    }
}

void init_character_map(character_t *character_map[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            character_map[i][j] = NULL;
        }
    }
}

void init_player(heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], character_t *player)
{
    player->sequence = 0;
    player->characteristics = 0x0000;
    player->characteristics |= ALIVE;
    player->characteristics |= FOG;
    player->speed = 10;
    heap_add(characters, player, 0);
    character_map[player->ypos][player->xpos] = player;
}

void generate_monsters(heap_t *characters, character_t *character_map[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], room_t *rooms, uint16_t num_rooms, uint16_t num_init_monsters)
{
    uint16_t i, j;
    character_t *monster;
    for (i = 1; i <= num_init_monsters; i++) {
        monster = (character_t *) malloc(sizeof(character_t));
        do {
            monster->characteristics = 0x00;
            monster->characteristics |= rand() % 2 ? SMART : 0x00;
            monster->characteristics |= rand() % 2 ? TELE : 0x00;
            monster->characteristics |= rand() % 2 ? TUNNEL : 0x00;
            monster->characteristics |= rand() % 2 ? ERRATIC : 0x00;
            monster->symbol = monster->characteristics < 10 ? '0' + monster->characteristics : monster->characteristics - 10 + 'a';
            monster->characteristics |= ALIVE;
            monster->characteristics &= ~KNOWN;
            monster->characteristics &= ~SEE;
            monster->known_location.xpos = 0;
            monster->known_location.ypos = 0;
            monster->sequence = i;
            monster->speed = 5 + rand() % 16;
            if (monster->characteristics & TUNNEL) {
                monster->xpos = 1 + rand() % (W_WIDTH - 2);
                monster->ypos = 1 + rand() % (W_HEIGHT - 2);
            }
            else {
                j = rand() % num_rooms;
                monster->xpos = rooms[j].xpos + rand() % rooms[j].xsize;
                monster->ypos = rooms[j].ypos + rand() % rooms[j].ysize;
            }
        } while (character_map[monster->ypos][monster->xpos] != NULL || (!(monster->characteristics & TUNNEL) && (map[monster->ypos][monster->xpos] == ' ')));
        heap_add(characters, monster, 0);
        character_map[monster->ypos][monster->xpos] = monster;
    }
}

void dijkstra(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], character_t* player, uint16_t distance[W_HEIGHT][W_WIDTH], uint8_t tunnel)
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
            if ((tunnel || map[i][j] != ' ') && hardness[i][j] != 0xFF) {
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
        get_neighbors(&s, u, map, hardness, tunnel);
        while (stack_size(&s) > 0) {
            stack_pop(&s, (void**) &v);
            if (map[v->ypos][v->xpos] == ' ')
                temp = distance[u->ypos][u->xpos] + 1 + hardness[v->ypos][v->xpos] / 85;
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

void get_neighbors(stack_t *s, vertex_t *v, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint8_t tunnel)
{
    vertex_t *u;
    if (v->ypos > 1) {
        if (v->xpos > 1) {
            if (hardness[v->ypos - 1][v->xpos - 1] != 0xFF && (tunnel || map[v->ypos - 1][v->xpos - 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos - 1;
                u->xpos = v->xpos - 1;
                stack_push(s, u);
            }
        }
        if (hardness[v->ypos - 1][v->xpos] != 0xFF && (tunnel || map[v->ypos - 1][v->xpos] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos - 1;
            u->xpos = v->xpos;
            stack_push(s, u);
        }
        if (v->xpos < W_WIDTH - 2) {
            if (hardness[v->ypos - 1][v->xpos + 1] != 0xFF && (tunnel || map[v->ypos - 1][v->xpos + 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos - 1;
                u->xpos = v->xpos + 1;
                stack_push(s, u);
            }
        }
    }
    if (v->xpos > 1) {
        if (hardness[v->ypos][v->xpos - 1] != 0xFF && (tunnel || map[v->ypos][v->xpos - 1] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos;
            u->xpos = v->xpos - 1;
            stack_push(s, u);
        }
    }
    if (v->xpos < W_WIDTH - 2) {
        if (hardness[v->ypos][v->xpos + 1] != 0xFF && (tunnel || map[v->ypos][v->xpos + 1] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos;
            u->xpos = v->xpos + 1;
            stack_push(s, u);
        }
    }
    if (v->ypos < W_HEIGHT) {
        if (v->xpos > 1) {
            if (hardness[v->ypos + 1][v->xpos - 1] != 0xFF && (tunnel || map[v->ypos + 1][v->xpos - 1] != ' ')) {
                u = (vertex_t *) malloc(sizeof(vertex_t));
                u->ypos = v->ypos + 1;
                u->xpos = v->xpos - 1;
                stack_push(s, u);
            }
        }
        if (hardness[v->ypos + 1][v->xpos] != 0xFF && (tunnel || map[v->ypos + 1][v->xpos] != ' ')) {
            u = (vertex_t *) malloc(sizeof(vertex_t));
            u->ypos = v->ypos + 1;
            u->xpos = v->xpos;
            stack_push(s, u);
        }
        if (v->xpos < W_WIDTH - 2) {
            if (hardness[v->ypos + 1][v->xpos + 1] != 0xFF && (tunnel || map[v->ypos + 1][v->xpos + 1] != ' ')) {
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
    get_neighbors(move_stack, hither, map, hardness, character->characteristics & TUNNEL);
    *thither = (vertex_t *) malloc(sizeof(vertex_t));
    (*thither)->xpos = hither->xpos;
    (*thither)->ypos = hither->ypos;
    stack_push(move_stack, *thither);
}

void player_command(character_t *player, vertex_t *thither, const char visible_map[W_HEIGHT][W_WIDTH], const char display[W_HEIGHT][W_WIDTH])
{
    int cmd;
    thither->xpos = player->xpos;
    thither->ypos = player->ypos;
    player->characteristics &= ~(WALK_UP & WALK_DOWN);
    do {
        cmd = getch();
        player->characteristics &= ~RESET;
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
                player->characteristics |= WALK_DOWN;
                break;
            case '<':
                player->characteristics |= WALK_UP;
                break;
            case '5':
            case ' ':
                break;
            case 'm':
                player->characteristics |= MON_LIST;
                break;
            case 'f':
                if (player->characteristics & FOG) {
                    player->characteristics &= ~FOG;
                    draw(display);
                }
                else {
                    player->characteristics |= FOG;
                    draw(visible_map);
                }
                refresh();
                player->characteristics |= RESET;
                break;
            case 'g':
                player->characteristics |= TELEPORT;
                break;
            case 'Q':
                player->characteristics |= QUIT;
                break;
            default:
                player->characteristics |= RESET;
                break;
        }
    } while (player->characteristics & RESET);
    player->characteristics &= ~RESET;
}

void monster_turn(stack_t *move_stack, character_t *character_map[W_HEIGHT][W_WIDTH], character_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t floor_distance[W_HEIGHT][W_WIDTH], uint16_t tunnel_distance[W_HEIGHT][W_WIDTH], vertex_t *hither, vertex_t **thither, uint16_t *num_monsters)
{
    uint16_t custom_distance[W_HEIGHT][W_WIDTH];
    character_t ghost_player;
    if (character->characteristics & ERRATIC && rand() % 2)
        random_move(move_stack, thither);
    else {
        line_of_sight(character, player, map);
        if (character->characteristics & TELE || character->characteristics & SEE) {
            if (character->characteristics & SMART) {
                if (character->characteristics & TUNNEL)
                    smart_move(move_stack, character, thither, tunnel_distance);
                else
                    smart_move(move_stack, character, thither, floor_distance);
            }
            else {
                dumb_move(move_stack, character, player, hither, thither);
            }
        }
        else {
            if (character->characteristics & KNOWN) {
                ghost_player.xpos = character->known_location.xpos;
                ghost_player.ypos = character->known_location.ypos;
                dijkstra(hardness, map, &ghost_player, custom_distance, character->characteristics & TUNNEL);
                smart_move(move_stack, character, thither, custom_distance);
            }
            else {
                random_move(move_stack, thither);
            }
        }
    }
    if (map[(*thither)->ypos][(*thither)->xpos] == ' ' && character->characteristics & TUNNEL) {
        tunnel(hardness, player, floor_distance, tunnel_distance, map, *thither);
    }
    if (map[(*thither)->ypos][(*thither)->xpos] != ' ') {
        move_character(character_map, character, hither, *thither, num_monsters);
    }
}

void line_of_sight(character_t *character, character_t *player, char map[W_HEIGHT][W_WIDTH])
{
    int i;
    double slope;
    character->characteristics |= SEE;
    if (player->xpos - character->xpos) {
        slope = (player->ypos - character->ypos) / (player->xpos - (double) character->xpos);
        if (!(slope > 1.0 || slope < -1.0)) {
            for (i = 0; i <= (int) (player->xpos) - (int) (character->xpos); i++) {
                if (map[character->ypos + (int) (slope * i)][character->xpos + i] == ' ') {
                    character->characteristics &= ~SEE;
                }
            }
            for (i = 0; i <= (int) (character->xpos) - (int) (player->xpos); i++) {
                if (map[player->ypos + (int) (slope * i)][player->xpos + i] == ' ') {
                    character->characteristics &= ~SEE;
                }
            }
        }
    }
    if (player->ypos - character->ypos) {
        slope = (player->xpos - character->xpos) / (player->ypos - (double) character->ypos);
        if (!(slope > 1.0 || slope < -1.0)) {
            for (i = 0; i <= (int) (player->ypos) - (int) (character->ypos); i++) {
                if (map[character->ypos + i][character->xpos + (int) (slope * i)] == ' ') {
                    character->characteristics &= ~SEE;
                }
            }
            for (i = 0; i <= (int) (character->ypos) - (int) (player->ypos); i++) {
                if (map[player->ypos + i][player->xpos + (int) (slope * i)] == ' ') {
                    character->characteristics &= ~SEE;
                }
            }
        }
    }
    if (character->characteristics & SMART && character->characteristics & SEE) {
        character->characteristics |= KNOWN;
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

void move_character(character_t *character_map[W_HEIGHT][W_WIDTH], character_t *character, vertex_t *hither, vertex_t *thither, uint16_t *num_monsters)
{
    /* [COMBAT] */
    if (character_map[thither->ypos][thither->xpos] != NULL && character_map[thither->ypos][thither->xpos] != character) {
        character_map[thither->ypos][thither->xpos]->characteristics &= ~ALIVE;
        if (character_map[thither->ypos][thither->xpos]->symbol != '@')
            *num_monsters -= 1;
    }
    character_map[hither->ypos][hither->xpos] = NULL;
    character_map[thither->ypos][thither->xpos] = character;
    character->xpos = thither->xpos;
    character->ypos = thither->ypos;
}

void teleport_command(character_t *player, vertex_t *thither, char display[W_HEIGHT][W_WIDTH])
{
    int cmd;
    player->characteristics &= ~ESCAPE;
    do {
        draw(display);
        mvprintw(thither->ypos + 1, thither->xpos, "*");
        refresh();
        cmd = getch();
        player->characteristics &= ~RESET;
        switch(cmd) {
            case '7':
            case 'y':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                if (thither->xpos > 1)
                    thither->ypos -= 1;
                player->characteristics |= RESET;
                break;
            case '8':
            case 'k':
                if (thither->ypos > 1)
                    thither->ypos -= 1;
                player->characteristics |= RESET;
                break;
            case '9':
            case 'u':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                if (thither->ypos > 1)
                    thither->ypos -= 1;
                player->characteristics |= RESET;
                break;
            case '6':
            case 'l':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                player->characteristics |= RESET;
                break;
            case '3':
            case 'n':
                if (thither->xpos < W_WIDTH - 2)
                    thither->xpos += 1;
                if (thither->ypos < W_HEIGHT - 2)
                    thither->ypos += 1;
                player->characteristics |= RESET;
                break;
            case '2':
            case 'j':
                if (thither->ypos > 1)
                    thither->ypos += 1;
                player->characteristics |= RESET;
                break;
            case '1':
            case 'b':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                if (thither->ypos < W_HEIGHT - 2)
                    thither->ypos += 1;
                player->characteristics |= RESET;
                break;
            case '4':
            case 'h':
                if (thither->xpos > 1)
                    thither->xpos -= 1;
                player->characteristics |= RESET;
                break;
            case 'g':
                break;
            /* ESCAPE */
            case 27:
                player->characteristics |= ESCAPE;
                break;
            case 'Q':
                player->characteristics |= QUIT;
                break;
            default:
                player->characteristics |= RESET;
        }
    } while (player->characteristics & RESET);
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
        if (player->characteristics & WALK_UP) {
            k -= 0 < k ? 1 : 0;
        }
        if (player->characteristics & WALK_DOWN) {
            k += k + 15 < num_init_monsters ? 1 : 0;
        }
        player->characteristics &= ~(WALK_UP | WALK_DOWN);
    } while (!(player->characteristics & ESCAPE) && !(player->characteristics & QUIT));
    free(monster_list);
}

void monster_list_command(character_t *player)
{
    int cmd;
    player->characteristics &= ~(WALK_UP & WALK_DOWN);
    player->characteristics &= ~ESCAPE;
    do {
        cmd = getch();
        player->characteristics &= ~RESET;
        switch (cmd) {
            case KEY_UP:
                player->characteristics |= WALK_UP;
                break;
            case KEY_DOWN:
                player->characteristics |= WALK_DOWN;
                break;
            /* ESCAPE */
            case 27:
                player->characteristics |= ESCAPE;
                break;
            case 'Q':
                player->characteristics |= QUIT;
                break;
            default:
                player->characteristics |= RESET;
                break;
        }
    } while (player->characteristics & RESET);
}

void place_characters(char display[W_HEIGHT][W_WIDTH], character_t *characters[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (characters[i][j] != NULL)
                display[i][j] = characters[i][j]->symbol;
        }
    }
}

void sketch_visible_map(char visible_map[W_HEIGHT][W_WIDTH], const char map[W_HEIGHT][W_WIDTH], const char display[W_HEIGHT][W_WIDTH], const character_t *player)
{
    uint8_t i, j;
    custom_vector_t v;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            v.ypos = i - player->ypos;
            v.xpos = j - player->xpos;
            v.ypos *= v.ypos < 0 ? -1 : 1;
            v.xpos *= v.xpos < 0 ? -1 : 1;
            if (v.xpos <= 2 && v.ypos <= 2) {
                visible_map[i][j] = display[i][j];
            }
            else if (visible_map[i][j] != ' ') {
                visible_map[i][j] = map[i][j];
            }
        }
    }
}

void place(char display[W_HEIGHT][W_WIDTH], character_t *entities, uint16_t count)
{
    uint16_t i;
    for (i = 0; i < count; i++) {
        display[entities[i].ypos][entities[i].xpos] = entities[i].symbol;
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

void draw(const char display[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            mvprintw(i + 1, j, "%c", display[i][j]);
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
