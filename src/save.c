#include "save.h"
#include "timer.h"
#include "config.h"
#include <kos.h>
#include <dc/maple.h>
#include <dc/vmufs.h>
#include <stdatomic.h>

// Most of the information in this file and the calc_CRC function was taken from:
// https://mc.pp.se/dc/vms/fileheader.html , credits to Marcus Comstedt

// Colors must be in ARGB4444 format, so I've converted the RGB values from my image to ARGB4444 below.

// The palette consists of 16 16-bit integers, one for each colour in the palette. Each integer consists of four
// four-bit fields:

// 0-3 : Alpha (15 is fully opaque, 0 is fully transparent)
// 4-7 : Red
// 8-11 : Green
// 12-15 : Blue
// So a value of 0b1111'0000'1111'0000 would be pure green with no transparency.

static const uint16_t vmu_light_blue  = 0b1111'1011'1101'1111;
static const uint16_t vmu_black       = 0b1111'0000'0000'0000;
static const uint16_t vmu_red         = 0b1111'1100'0001'0001;
static const uint16_t vmu_periwinkle  = 0b1111'1000'1001'1111;
static const uint16_t vmu_blue        = 0b1111'0100'0110'1111;
static const uint16_t vmu_sand        = 0b1111'1100'1010'0101;
static const uint16_t vmu_darksand    = 0b1111'1100'0111'0010;
static const uint16_t vmu_white       = 0b1111'1111'1111'1111;
static const uint16_t vmu_purple      = 0b1110'1010'0001'1010;
static const uint16_t vmu_gold        = 0b1111'1110'1010'0000;
static const uint16_t vmu_transparent = 0b0000'0000'0000'0000;

static const uint16_t bios_save_palette[16] = {
    vmu_light_blue,  vmu_black,       vmu_red,         vmu_periwinkle,  vmu_blue,        vmu_sand,        vmu_darksand,    vmu_transparent,
    vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent,
};

static const uint16_t bios_eyecatch_palette[16] = {
    vmu_purple,      vmu_gold,        vmu_black,       vmu_white,       vmu_red,         vmu_transparent, vmu_transparent, vmu_transparent,
    vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent,
};

// The below array contains the frames of the animation shown in the Dreamcast BIOS.
// Each value is a nybble corresponding to a colour index in the palette above.
// For example, 0x01 would be a light blue pixel on the left and a black pixel on the right.

// clang-format off
static const int8_t bios_save_animation[512 * 3] = {
    ///////////////////////////////////   FRAME 1    /////////////////////////////////////////////
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x43, 0x43, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x34, 0x34,
    0x34, 0x44, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x44, 0x43,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x45, 0x54, 0x51, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x55, 0x45,
    0x55, 0x55, 0x51, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x15, 0x55, 0x55,
    0x55, 0x55, 0x51, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x15, 0x55, 0x55,
    0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55,
    ///////////////////////////////////   FRAME 2    /////////////////////////////////////////////
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x34, 0x34, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x43, 0x43,
    0x43, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x34, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x14, 0x44, 0x44,
    0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55,
    ///////////////////////////////////   FRAME 3    /////////////////////////////////////////////
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x43, 0x43, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x34, 0x34,
    0x44, 0x34, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x43, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x41, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x14, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55, 0x45, 0x54, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65
};
// clang-format on

// Converted with Crayon VMU tools by Protofall
static const uint8_t bios_eyecatch_bitmap[2016] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x22, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x02, 0x23, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x32, 0x21, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33,
    0x33, 0x33, 0x32, 0x23, 0x33, 0x33, 0x33, 0x21, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x22, 0x22, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x32, 0x22, 0x22, 0x23, 0x33, 0x33, 0x22, 0x21, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x32, 0x22, 0x22, 0x23, 0x33, 0x33, 0x23, 0x22, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x12, 0x22, 0x22, 0x22, 0x00, 0x00, 0x02, 0x23, 0x33, 0x32, 0x22, 0x22, 0x33, 0x33, 0x33, 0x23, 0x32, 0x21, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x02, 0x23, 0x33, 0x33, 0x22, 0x21, 0x11, 0x12, 0x23, 0x33, 0x33, 0x22, 0x23, 0x33, 0x33, 0x33, 0x23, 0x33, 0x20, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x32, 0x11, 0x22, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x23,
    0x33, 0x32, 0x20, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x23, 0x32, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x23, 0x33, 0x33, 0x22, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33,
    0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x32, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x32, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x23, 0x33, 0x33, 0x33, 0x33, 0x20, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x23, 0x33,
    0x33, 0x32, 0x22, 0x33, 0x33, 0x32, 0x33, 0x33, 0x32, 0x23, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x23, 0x33, 0x33, 0x32, 0x22, 0x23, 0x33, 0x32, 0x33, 0x33, 0x33, 0x32, 0x22, 0x22, 0x22, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x23, 0x33, 0x33, 0x22, 0x22, 0x23, 0x33, 0x33, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x23, 0x33, 0x33, 0x22, 0x22, 0x23, 0x33, 0x33, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x32, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x23, 0x33, 0x33, 0x32, 0x22, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x23, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x22,
    0x33, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x22, 0x32, 0x22, 0x23, 0x33, 0x32, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x02, 0x22, 0x33, 0x33, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x22, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x20, 0x11, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x20, 0x11, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x34, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x23, 0x33, 0x32, 0x20, 0x11, 0x11, 0x23, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x23, 0x33, 0x32, 0x00,
    0x11, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32,
    0x23, 0x33, 0x32, 0x00, 0x11, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x32, 0x00, 0x11, 0x12, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x23, 0x33, 0x33, 0x22, 0x00, 0x11, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x33, 0x33, 0x33, 0x20, 0x00, 0x11, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x32, 0x20, 0x00, 0x11, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x33, 0x33, 0x33,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x22, 0x33, 0x33, 0x22, 0x11, 0x11, 0x02, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x34, 0x43, 0x33, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x23, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x12, 0x33, 0x32, 0x21, 0x11, 0x11, 0x02, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x33, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x12, 0x22, 0x22, 0x11, 0x11, 0x11,
    0x02, 0x23, 0x33, 0x33, 0x33, 0x43, 0x33, 0x33, 0x33, 0x34, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x02, 0x23, 0x33, 0x33, 0x33, 0x33, 0x32, 0x21, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x23, 0x33, 0x33, 0x33, 0x44, 0x33, 0x33, 0x33, 0x33, 0x34, 0x43, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x22, 0x33, 0x33, 0x33, 0x33,
    0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x23, 0x33, 0x33, 0x33, 0x34, 0x43, 0x33, 0x33, 0x33, 0x33, 0x44, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x02,
    0x22, 0x23, 0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x22, 0x33, 0x33, 0x33, 0x33, 0x44, 0x33, 0x33, 0x33, 0x33, 0x34, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x02, 0x23, 0x33, 0x33, 0x33, 0x34, 0x43, 0x33, 0x33, 0x33, 0x33, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x22, 0x33, 0x33, 0x33, 0x33, 0x34, 0x33, 0x33, 0x33, 0x33
};

static int calc_CRC(const unsigned char *buf, int size) {
    int i, c, n = 0;
    for (i = 0; i < size; i++) {
        n ^= (buf[i] << 8);
        for (c = 0; c < 8; c++)
            if (n & 0x8000) n = (n << 1) ^ 4129;
            else n = (n << 1);
    }
    return n & 0xffff;
}

save_t       save;
bbox_timer_t save_popup_timer;
char         save_popup_text[64];

// This is only drawn if needed
void draw_save_popup(void) {
    if (!save_popup_timer.is_running) return;
    DrawRectangle(SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.85, SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.15, (Color){ 1, 17, 34, 220 });
    DrawText(save_popup_text, SCREEN_WIDTH * 0.7 + SCREEN_WIDTH * 0.3 / 2 - MeasureText(save_popup_text, 22) / 2, SCREEN_HEIGHT * 0.9, 22, RAYWHITE);
}

// This prevents VMU animations from being drawn while a save is in progress
// So that we make sure saving doesn't fail
static atomic_bool save_in_progress_ = false;

bool save_in_progress(void) {
    return save_in_progress_;
}

int save_game(void) {
    save_in_progress_ = true;
    snprintf(save_popup_text, sizeof(save_popup_text), "Saving...");
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        snprintf(save_popup_text, sizeof(save_popup_text), "No VMU found!");
        save_in_progress_ = false;
        return -1;
    }

    const size_t save_size   = sizeof(save);
    const size_t header_size = sizeof(save.header);

    snprintf(save.header.dc_description, sizeof(save.header.dc_description), "BeachBox - PsyOps");
    snprintf(save.header.app_identifier, sizeof(save.header.app_identifier), "Psyops");
    save.header.number_icons          = 3;
    save.header.icon_animation_speed  = 14;
    save.header.graphic_eyecatch_type = 3;
    save.header.crc                   = calc_CRC((unsigned char *)&save, save_size);
    save.header.bytes_after_header    = (int32_t)(save_size - header_size);
    memcpy(save.header.icon_palette, bios_save_palette, sizeof(bios_save_palette));
    memcpy(save.header.icon_bitmaps, bios_save_animation, sizeof(bios_save_animation));
    memcpy(save.header.eyecatch_palette, bios_eyecatch_palette, sizeof(bios_eyecatch_palette));
    memcpy(save.header.eyecatch_bitmap, bios_eyecatch_bitmap, sizeof(bios_eyecatch_bitmap));

    const int free_blocks = vmufs_free_blocks(vmu);

    if (free_blocks * 512 < save_size) {
        snprintf(save_popup_text, sizeof(save_popup_text), "Not enough space!");
        save_in_progress_ = false;
        return 0;
    }

    if (0 == vmufs_write(vmu, "BeachBox", &save, save_size, VMUFS_OVERWRITE)) {
        snprintf(save_popup_text, sizeof(save_popup_text), "Game Saved!");
        save_in_progress_ = false;
        return 1;
    }

    snprintf(save_popup_text, sizeof(save_popup_text), "Failed to save!");
    save_in_progress_ = false;
    return -2; // unknown error
}

// This prevents VMU animations from being drawn while a load is in progress
// So that we make sure loading doesn't fail
static atomic_bool load_in_progress_ = false;

bool load_in_progress(void) {
    return load_in_progress_;
}

int load_game(void) {
    load_in_progress_ = true;
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        load_in_progress_ = false;
        return -1;
    }
    void *save_buffer = malloc(sizeof(save));

    if (vmufs_read(vmu, "BeachBox", &save_buffer, NULL) == 0) { // If loading was successful
        memcpy(&save, save_buffer, sizeof(save));
        free(save_buffer);
        load_in_progress_ = false;
        return 1;
    }
    free(save_buffer);
    load_in_progress_ = false;
    return -1;
}

void new_game(void) {
    memset(&save, 0, sizeof(save));
    save.hats_unlocked[0] = true;
}