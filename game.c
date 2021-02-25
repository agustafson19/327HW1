#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>
#include <stdint.h>

#include <endian.h>

#include "vertex.c"
#include "stack.c"
#include "priority_queue.c"

#define W_WIDTH 80
#define W_HEIGHT 21

#define SAVE 0x01
#define LOAD 0x02

typedef struct room {
    uint8_t xpos;
    uint8_t ypos;
    uint8_t xsize;
    uint8_t ysize;
} room_t;

typedef struct entity {
    uint8_t xpos;
    uint8_t ypos;
    char symbol;
} entity_t;

void get_args(int argc, char *argv[], uint8_t *args);

void generate(entity_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs);

void save(entity_t *player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, entity_t *u_stairs, uint16_t num_d_stairs, entity_t *d_stairs);
void load(entity_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs);

void map_rooms(char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms);

void init_map(char map[W_HEIGHT][W_WIDTH], char c);
void sketch_map(char display[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH]);

void dijkstra(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], entity_t *player, uint16_t distance[W_HEIGHT][W_WIDTH], uint8_t tunnel);
void get_neighbors(stack_t *s, vertex_t v, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint8_t tunnel);

void place(char display[W_HEIGHT][W_WIDTH], entity_t *entities, uint16_t count);

void draw_distance(uint16_t distance[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH]);
void draw(char display[W_HEIGHT][W_WIDTH]);

int main(int argc, char *argv[])
{
    /* Switch Processing */
    uint8_t args = 0x00;
    get_args(argc, argv, &args);

    /* Seeding */
    srand(time(NULL));

    /* Declarations */
    entity_t *player;
    uint8_t hardness[W_HEIGHT][W_WIDTH];
    uint16_t floor_distance[W_HEIGHT][W_WIDTH];
    uint16_t tunnel_distance[W_HEIGHT][W_WIDTH];
    uint16_t num_rooms;
    room_t *rooms;
    uint16_t num_u_stairs;
    entity_t *u_stairs;
    uint16_t num_d_stairs;
    entity_t *d_stairs;
    char map[W_HEIGHT][W_WIDTH];
    char display[W_HEIGHT][W_WIDTH];

    /* Saving and Loading */
    if (args & LOAD)
        load(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    else
        generate(&player, hardness, &num_rooms, &rooms, &num_u_stairs, &u_stairs, &num_d_stairs, &d_stairs);
    if (args & SAVE)
        save(player, hardness, num_rooms, rooms, num_u_stairs, u_stairs, num_d_stairs, d_stairs);

    /* Sketching Map */
    init_map(map, ' ');
    map_rooms(map, hardness, num_rooms, rooms);

    /* Distances */
    dijkstra(hardness, map, player, floor_distance, 0);
    dijkstra(hardness, map, player, tunnel_distance, 1);

    /* Displaying */
    sketch_map(display, map);
    place(display, player, 1);
    place(display, u_stairs, num_u_stairs);
    place(display, d_stairs, num_d_stairs);
    draw(display);

    /* Displaying Distances */
    draw_distance(floor_distance, map);
    draw_distance(tunnel_distance, map);

    /* Cleaning Memory */
    free(player);
    free(rooms);
    free(u_stairs);
    free(d_stairs);
}

void get_args(int argc, char *argv[], uint8_t *args)
{
    uint8_t i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--save")) {
            *args |= SAVE;
        }
        else if (!strcmp(argv[i], "--load")) {
            *args |= LOAD;
        }
    }
}

void generate(entity_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs)
{
    /* printf("CREATING:\n"); */
    uint16_t i, j;
    uint8_t v;
    *num_rooms = 6 + rand() % 2;
    *rooms = malloc(sizeof(room_t) * *num_rooms);
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
    /*
    for (i = 0; i < *num_rooms; i++) {
        printf("%d : %d : %d : %d\n", (*rooms)[i].xpos, (*rooms)[i].ypos, (*rooms)[i].xsize, (*rooms)[i].ysize);
    }
    */
    *player = malloc(sizeof(entity_t));
    (*player)->xpos = (*rooms)[0].xpos;
    (*player)->ypos = (*rooms)[0].ypos;
    (*player)->symbol = '@';
    *num_u_stairs = 1;
    *u_stairs = malloc(sizeof(entity_t));
    (*u_stairs)->xpos = (*rooms)[2].xpos + 1;
    (*u_stairs)->ypos = (*rooms)[2].ypos + 1;
    (*u_stairs)->symbol = '<';
    *num_d_stairs = 1;
    *d_stairs = malloc(sizeof(entity_t));
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

void save(entity_t *player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t num_rooms, room_t *rooms, uint16_t num_u_stairs, entity_t *u_stairs, uint16_t num_d_stairs, entity_t *d_stairs)
{
    /* printf("SAVING:\n"); */
    uint16_t i;
    uint8_t j;
    uint8_t w_byte;
    uint16_t w_short;
    uint32_t w;
    char version[13] = "RLG327-@2021";
    char *home = getenv("HOME");
    char *gamedir = ".rlg327";
    char *save = "dungeon";
    char *path = malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
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

void load(entity_t **player, uint8_t hardness[W_HEIGHT][W_WIDTH], uint16_t *num_rooms, room_t **rooms, uint16_t *num_u_stairs, entity_t **u_stairs, uint16_t *num_d_stairs, entity_t **d_stairs)
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
    char *gamedir = ".rlg327";
    char *save = "dungeon";
    char *path = malloc(strlen(home) + strlen(gamedir) + strlen(save) + 3);
    sprintf(path, "%s/%s/%s", home, gamedir, save);
    FILE *f = fopen(path, "r");
    fread(version, sizeof(char), 12, f);
    fread(&r, sizeof(uint32_t), 1, f);
    fread(&r, sizeof(uint32_t), 1, f);
    *player = malloc(sizeof(entity_t));
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
    *rooms = malloc(sizeof(room_t) * *num_rooms);
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
    *u_stairs = malloc(sizeof(entity_t) * *num_u_stairs);
    for (i = 0; i < *num_u_stairs; i++) {
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].xpos = r_byte;
        fread(&r_byte, sizeof(uint8_t), 1, f);
        (*u_stairs)[i].ypos = r_byte;
        (*u_stairs)[i].symbol = '<';
    }
    fread(&r_short, sizeof(uint16_t), 1, f);
    *num_d_stairs = be16toh(r_short);
    *d_stairs = malloc(sizeof(entity_t) * *num_d_stairs);
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

void dijkstra(uint8_t hardness[W_HEIGHT][W_WIDTH], char map[W_HEIGHT][W_WIDTH], entity_t* player, uint16_t distance[W_HEIGHT][W_WIDTH], uint8_t tunnel)
{
    uint8_t i, j;
    uint16_t temp;
    vertex_t v, u;
    stack_t s;
    priority_queue_t q;
    stack_init(&s);
    priority_queue_init(&q);
    distance[player->ypos][player->xpos] = 0;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            if (!(i == player->ypos && j == player->xpos)) {
                distance[i][j] = 0xFFFF;
            }
            if ((tunnel || map[i][j] != ' ') && hardness[i][j] != 0xFF) {
                v.ypos = i;
                v.xpos = j;
                priority_queue_add(&q, v, distance[v.ypos][v.xpos]);
            }
        }
    }
    while (priority_queue_size(&q) > 0) {
        priority_queue_extract_min(&q, &u, &temp);
        if (temp == 0xFFFF)
            continue;
        get_neighbors(&s, u, map, hardness, tunnel);
        while (stack_size(&s) > 0) {
            stack_pop(&s, &v);
            if (map[u.ypos][u.xpos] == ' ')
                temp = distance[u.ypos][u.xpos] + 1 + hardness[u.ypos][u.xpos] / 85;
            else
                temp = distance[u.ypos][u.xpos] + 1;
            if (temp < distance[v.ypos][v.xpos]) {
                distance[v.ypos][v.xpos] = temp;
                priority_queue_decrease_priority(&q, v, temp);
            }
        }
    }
    stack_delete(&s);
    priority_queue_delete(&q);
}

void get_neighbors(stack_t *s, vertex_t v, char map[W_HEIGHT][W_WIDTH], uint8_t hardness[W_HEIGHT][W_WIDTH], uint8_t tunnel)
{
    vertex_t u;
    if (v.ypos > 0) {
        u.ypos = v.ypos - 1;
        if (v.xpos > 0) {
            u.xpos = v.xpos - 1;
            if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        }
        u.xpos = v.xpos;
        if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        if (v.xpos < W_WIDTH - 1) {
            u.xpos = v.xpos + 1;
            if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        }
    }
    u.ypos = v.ypos;
    if (v.xpos > 0) {
        u.xpos = v.xpos - 1;
        if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
    }
    if (v.xpos < W_WIDTH - 1) {
        u.xpos = v.xpos + 1;
        if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
    }
    if (v.ypos < W_HEIGHT) {
        u.ypos = v.ypos + 1;
        if (v.xpos > 0) {
            u.xpos = v.xpos - 1;
            if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        }
        u.xpos = v.xpos;
        if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        if (v.xpos < W_WIDTH - 1) {
            u.xpos = v.xpos + 1;
            if (hardness[u.ypos][u.xpos] != 0xFF && (tunnel || map[u.ypos][u.xpos] != ' ')) stack_push(s, u);
        }
    }
}

void place(char display[W_HEIGHT][W_WIDTH], entity_t *entities, uint16_t count)
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
                printf(" ");
            else if (distance[i][j] == 0xFFFF && map[i][j] != ' ')
                printf("X");
            else if (distance[i][j] == 0)
                printf("@");
            else
                printf("%c", (distance[i][j] % 10) + '0');
        }
        printf("\n");
    }
}

void draw(char display[W_HEIGHT][W_WIDTH])
{
    uint8_t i, j;
    for (i = 0; i < W_HEIGHT; i++) {
        for (j = 0; j < W_WIDTH; j++) {
            printf("%c", display[i][j]);
        }
        printf("\n");
    }
}
