#ifndef DICE_H
#define DICE_H

typedef class Dice {
    public:
        int base;
        int num;
        int sides;
        Dice();
        Dice(const int base, const int num, const int sides);
        int toss();
} Dice_t;

#endif
