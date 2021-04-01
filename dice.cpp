#include <stdlib.h>
#include <time.h>

#include "dice.h"

Dice::Dice()
{
    base = 0;
    num = 0;
    sides = 0;
    srand(time(NULL));
}

Dice::Dice(const int base, const int num, const int sides)
{
    this->base = base;
    this->num = num;
    this->sides = sides;
    srand(time(NULL));
}

int Dice::toss()
{
    int i;
    int sum = base;
    for (i = 0; i < num; i++) {
        sum += 1 + rand() % sides;
    }
    return sum;
}
