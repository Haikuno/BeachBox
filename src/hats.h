#ifndef BBOX_HATS_H
#define BBOX_HATS_H

typedef enum Hats {
    HAT_NIL,
    HAT_SLIME_RED,
    HAT_SLIME_BLUE,
    HAT_BOX,
    HAT_M,
    HAT_L,
    HAT_Z,
    HAT_F,
    HAT_MUPRH,
    HAT_CROWN,
    HAT_MAX,
} hats_t;

void load_hats(void);

#endif // BBOX_HATS_H