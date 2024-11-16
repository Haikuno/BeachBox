#include "save.h"
#include "timer.h"
#include "config.h"
#include <kos.h>
#include <dc/maple.h>
#include <dc/vmufs.h>
#include <stdatomic.h>
#include "assert.h"

// TODO: maybe save volume as well?

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

constexpr uint16_t vmu_light_blue  = 0b1111'1011'1101'1111;
constexpr uint16_t vmu_black       = 0b1111'0000'0000'0000;
constexpr uint16_t vmu_red         = 0b1111'1100'0001'0001;
constexpr uint16_t vmu_periwinkle  = 0b1111'1000'1001'1111;
constexpr uint16_t vmu_blue        = 0b1111'0100'0110'1111;
constexpr uint16_t vmu_sand        = 0b1111'1100'1010'0101;
constexpr uint16_t vmu_darksand    = 0b1111'1100'0111'0010;
constexpr uint16_t vmu_white       = 0b1111'1111'1111'1111;
constexpr uint16_t vmu_purple      = 0b1110'1010'0001'1010;
constexpr uint16_t vmu_gold        = 0b1111'1110'1010'0000;
constexpr uint16_t vmu_transparent = 0b0000'0000'0000'0000;

constexpr uint16_t bios_save_palette[16] = {
    vmu_light_blue,  vmu_black,       vmu_red,         vmu_periwinkle,  vmu_blue,        vmu_sand,        vmu_darksand,    vmu_transparent,
    vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent,
};

constexpr uint16_t bios_eyecatch_palette[16] = {
    vmu_purple,      vmu_gold,        vmu_black,       vmu_white,       vmu_red,         vmu_transparent, vmu_transparent, vmu_transparent,
    vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent, vmu_transparent,
};

// The below array contains the frames of the animation shown in the Dreamcast BIOS.
// Each value is a nybble corresponding to a color index in the palette above.
// For example, 0x01 would be a light blue pixel on the left and a black pixel on the right.

// clang-format off
constexpr int8_t bios_save_animation[512 * 3] = {
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
constexpr uint8_t bios_eyecatch_bitmap[2016] = {
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

static const int calc_CRC(const unsigned char *buf, int size) {
    int i, c, n = 0;
    for (i = 0; i < size; i++) {
        n ^= (buf[i] << 8);
        for (c = 0; c < 8; c++)
            if (n & 0x8000) n = (n << 1) ^ 4129;
            else n = (n << 1);
    }
    return n & 0xffff;
}

static save_t       save;
static bbox_timer_t save_popup_timer;
static char         save_popup_text[20];

void draw_save_popup(void) {
    if (!save_popup_timer.is_running) return;
    DrawRectangle(SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.85, SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.15, (Color){ 1, 17, 34, 220 });
    DrawText(save_popup_text, SCREEN_WIDTH * 0.7 + SCREEN_WIDTH * 0.3 / 2 - MeasureText(save_popup_text, 22) / 2, SCREEN_HEIGHT * 0.9, 22, RAYWHITE);
}

// This prevents VMU animations from being drawn while a save is in progress
// So that we make sure saving doesn't fail
static atomic_bool save_in_progress_ = false;

const bool is_save_in_progress(void) {
    return save_in_progress_;
}

const int save_game(void) {
    save_in_progress_ = true;
    snprintf(save_popup_text, sizeof(save_popup_text), "Saving...");
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        snprintf(save_popup_text, sizeof(save_popup_text), "No VMU found!");
        save_in_progress_ = false;
        return -1;
    }

    constexpr size_t save_size   = sizeof(save);
    constexpr size_t header_size = sizeof(save.header);

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

static void *save_game_wrapper(void *user_data) {
    save_game();
    return nullptr;
}

void save_game_async(void) {
    start_timer(&save_popup_timer, 2.0f);
    thd_create(1, save_game_wrapper, nullptr);
}

void update_save_game_timer(void) {
    update_timer(&save_popup_timer);
}

// This prevents VMU animations from being drawn while a load is in progress
// So that we make sure loading doesn't fail
static atomic_bool load_in_progress_ = false;

const bool is_load_in_progress(void) {
    return load_in_progress_;
}

const int load_game(void) {
    load_in_progress_   = true;
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

        for (int i = 0; i < UPGRADE_COUNT; i++) {
            assert_msg(get_upgrade_level(i) <= get_max_upgrade_level(i), "Invalid upgrade level, save is probably corrupted");
        }

        return 1;
    }

    free(save_buffer);
    load_in_progress_ = false;
    return -1;
}

void new_game(void) {
    memset(&save, 0, sizeof(save));
    save.unlocked_hats.nil = true;
}

const uint16_t get_total_coins(void) {
#ifdef DEBUG_INFINITE_COINS
    return UINT16_MAX;
#endif
    return save.total_coins;
}

void add_coins(const uint16_t n) {
    uint16_t n_ = n;
    // Check for overflow
    const int total_coins_ = get_total_coins();
    if (total_coins_ + n > UINT16_MAX) {
        n_ = UINT16_MAX - total_coins_;
    }

    save.total_coins += n_;
}

const uint16_t get_total_runs(void) {
    return save.total_runs;
}

void increment_total_runs(void) {
    // Check for overflow
    const int total_runs_ = get_total_runs();
    if (total_runs_ + 1 > UINT16_MAX) {
        return;
    }

    save.total_runs++;
}

const uint16_t get_high_score(void) {
    return save.high_score;
}

void set_high_score(const uint16_t new_high_score) {
    save.high_score = new_high_score;
}

const uint8_t get_upgrade_level(const player_upgrade_t upgrade) {
    assert_msg(upgrade < UPGRADE_COUNT && upgrade >= 0, "Upgrade passed to get_upgrade_level is out of bounds");

    switch (upgrade) {
        case UPGRADE_SPEED:
            return save.upgrade_levels.speed;
        case UPGRADE_METER:
            return save.upgrade_levels.meter;
        case UPGRADE_TELEPORT_UNLOCK:
            return save.upgrade_levels.teleport_unlocked;
        case UPGRADE_TELEPORT_COOLDOWN:
            return save.upgrade_levels.teleport_cooldown;
        case UPGRADE_TELEPORT_DISTANCE:
            return save.upgrade_levels.teleport_distance;
        case UPGRADE_SLOWDOWN_UNLOCK:
            return save.upgrade_levels.slowdown_unlocked;
        case UPGRADE_SLOWDOWN_DRAIN:
            return save.upgrade_levels.slowdown_drain;
        default:
            break;
    }

    assert_msg(false, "Invalid upgrade passed to get_upgrade_level");
    return -1;
}

const uint8_t get_max_upgrade_level(const player_upgrade_t upgrade) {
    assert_msg(upgrade < UPGRADE_COUNT && upgrade >= 0, "Upgrade passed to get_max_upgrade_level is out of bounds");

    switch (upgrade) {
        case UPGRADE_SLOWDOWN_UNLOCK:
        case UPGRADE_TELEPORT_UNLOCK:
            return 1;
        default:
            return 5;
    }
}

void increment_upgrade_level(const player_upgrade_t upgrade) {
    assert_msg(upgrade < UPGRADE_COUNT && upgrade >= 0, "Invalid upgrade passed to increment_upgrade_level");

    switch (upgrade) {
        case UPGRADE_SPEED:
            save.upgrade_levels.speed++;
            break;
        case UPGRADE_METER:
            save.upgrade_levels.meter++;
            break;
        case UPGRADE_TELEPORT_UNLOCK:
            save.upgrade_levels.teleport_unlocked = true;
            break;
        case UPGRADE_TELEPORT_COOLDOWN:
            save.upgrade_levels.teleport_cooldown++;
            break;
        case UPGRADE_TELEPORT_DISTANCE:
            save.upgrade_levels.teleport_distance++;
            break;
        case UPGRADE_SLOWDOWN_UNLOCK:
            save.upgrade_levels.slowdown_unlocked = true;
            break;
        case UPGRADE_SLOWDOWN_DRAIN:
            save.upgrade_levels.slowdown_drain++;
            break;
        default:
            break;
    }
}

const hat_t get_current_hat_type(void) {
    return save.hat_index;
}

void increment_current_hat_index(void) {
    save.hat_index = (save.hat_index + 1) % HAT_COUNT;
}

void decrement_current_hat_index(void) {
    save.hat_index = (save.hat_index - 1 + HAT_COUNT) % HAT_COUNT;
}

const bool is_hat_unlocked(hat_t hat) {
    assert_msg(hat < HAT_COUNT && hat >= 0, "Hat passed to is_hat_unlocked is out of bounds");

    switch (hat) {
        case HAT_NIL:
            return save.unlocked_hats.nil;
        case HAT_SLIME_RED:
            return save.unlocked_hats.slime_red;
        case HAT_SLIME_BLUE:
            return save.unlocked_hats.slime_blue;
        case HAT_BOX:
            return save.unlocked_hats.box;
        case HAT_M:
            return save.unlocked_hats.m;
        case HAT_L:
            return save.unlocked_hats.l;
        case HAT_Z:
            return save.unlocked_hats.z;
        case HAT_F:
            return save.unlocked_hats.f;
        case HAT_MUPRH:
            return save.unlocked_hats.muprh;
        case HAT_CROWN:
            return save.unlocked_hats.crown;
        default:
            assert_msg(false, "Invalid hat passed to is_hat_unlocked");
            break;
    }

    return false;
}

void unlock_hat(hat_t hat) {
    assert_msg(hat < HAT_COUNT && hat >= 0, "Hat passed to unlock_hat is out of bounds");

    switch (hat) {
        case HAT_NIL:
            save.unlocked_hats.nil = true;
        case HAT_SLIME_RED:
            save.unlocked_hats.slime_red = true;
        case HAT_SLIME_BLUE:
            save.unlocked_hats.slime_blue = true;
        case HAT_BOX:
            save.unlocked_hats.box = true;
        case HAT_M:
            save.unlocked_hats.m = true;
        case HAT_L:
            save.unlocked_hats.l = true;
        case HAT_Z:
            save.unlocked_hats.z = true;
        case HAT_F:
            save.unlocked_hats.f = true;
        case HAT_MUPRH:
            save.unlocked_hats.muprh = true;
        case HAT_CROWN:
            save.unlocked_hats.crown = true;
        default:
            assert_msg(false, "Invalid hat passed to unlock_hat");
            break;
    }
}

const uint8_t get_player_current_color(void) {
    return save.color_index;
}

constexpr const uint8_t player_color_count = 17; // TODO: <<< THIS IS REEST

void increment_player_color_index(void) {
    save.color_index = (save.color_index + 1) % player_color_count;
}

void decrement_player_color_index(void) {
    save.color_index = (save.color_index - 1 + player_color_count) % player_color_count;
}