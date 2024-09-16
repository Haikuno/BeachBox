// Most of the information in this file and the calc_CRC function was taken from: https://mc.pp.se/dc/vms/fileheader.html , credits to Marcus Comstedt

// Colors must be in ARGB4444 format, so I've converted the RGB values from my image to ARGB4444 below.

// The palette consists of 16 16-bit integers, one for each colour in the palette. Each integer consists of four four-bit fields:

// 0-3 : Alpha (15 is fully opaque, 0 is fully transparent)
// 4-7 : Red
// 8-11 : Green
// 12-15 : Blue
// So a value of 0b1111'0000'1111'0000 would be pure green with no transparency.

const uint16_t vmu_light_blue = 0b1111'1011'1101'1111;
const uint16_t vmu_black = 0b1111'0000'0000'0000;
const uint16_t vmu_red = 0b1111'1100'0001'0001;
const uint16_t vmu_periwinkle = 0b1111'1000'1001'1111;
const uint16_t vmu_blue = 0b1111'0100'0110'1111;
const uint16_t vmu_sand = 0b1111'1100'1010'0101;
const uint16_t vmu_darksand = 0b1111'1100'0111'0010;
const uint16_t vmu_color_blank = 0b0000'0000'0000'0000;

const uint16_t bios_save_palette[] = {
    vmu_light_blue,
    vmu_black,
    vmu_red,
    vmu_periwinkle,
    vmu_blue,
    vmu_sand,
    vmu_darksand,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
    vmu_color_blank,
};

// The below array contains the frames of the animation shown in the Dreamcast BIOS.
// Each value is a nybble corresponding to a colour index in the palette above.
// For example, 0x01 would be a light blue pixel on the left and a black pixel on the right.

// TODO: Explain to the user how to get their pngs to an array like this, possibly point them to crayon tools
const uint8_t bios_save_animation[] = {
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
    0x55, 0x55, 0x55, 0x55, 0x65, 0x55, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65};

struct PlayerUpgradeLevels {
    bool teleport_unlocked;
    bool slowdown_unlocked;
    uint8_t player_speed_level;
    uint8_t meter_level;
    uint8_t teleport_cooldown_level;
    uint8_t teleport_distance_level;
    uint8_t slowdown_cost_level;
};

struct __attribute__((packed)) Save {
    // HEADER //
    char vms_menu_description[16];  // Description of file (shown in VMS file menu)
    char dc_description[32];        // Description of file (shown in DC boot ROM file manager)
    char app_identifier[16];        // Identifier of application that created the file (STRING)
    int16_t number_icons;           // Number of icons (>1 for animated icons)
    int16_t icon_animation_speed;   // Icon animation speed
    int16_t graphic_eyecatch_type;  // Graphic eyecatch type (0 = none)
    int16_t crc;                    // CRC (ignored for game files.)
    int32_t bytes_after_header;     // Number of bytes of actual file data following header, icon(s) and graphic eyecatch. (Ignored for game files.)
    char header_reserved[20];       // Reserved (fill with zeros)
    int16_t icon_palette[16];       // Icon palette (16 16-bit integers)
    uint8_t icon_bitmaps[512 * 3];  // Icon bitmaps (Nybbles) (512 bytes per frame)
    // Text fields are padded with space ($20). String fileds are padded with NUL ($00).

    // SAVE DATA //
    struct PlayerUpgradeLevels player_upgrade_levels;
    uint16_t total_coins;
    uint16_t total_runs;
    uint16_t high_score;
    uint8_t color_index;  // Selected color from player_colors array
    uint8_t hat_index;    // Selected hat from hats array
    bool hats_unlocked[HAT_MAX];
    char reserved_data[300];  // reserve 300 bytes for future expansion just in case, since we already are using less than 2 blocks
} save;

int calc_CRC(const unsigned char *buf, int size) {
    int i, c, n = 0;
    for (i = 0; i < size; i++) {
        n ^= (buf[i] << 8);
        for (c = 0; c < 8; c++)
            if (n & 0x8000)
                n = (n << 1) ^ 4129;
            else
                n = (n << 1);
    }
    return n & 0xffff;
}

struct Timer save_popup_timer;
char saved_text[64];

// This is only drawn if needed
void draw_save_popup(void) {
    if (save_popup_timer.is_done) return;
    DrawRectangle(SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.85, SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.15, (Color){1, 17, 34, 220});
    DrawText(saved_text, SCREEN_WIDTH * 0.7 + SCREEN_WIDTH * 0.3 / 2 - MeasureText(saved_text, 22) / 2, SCREEN_HEIGHT * 0.9, 22, RAYWHITE);
}

// Returns 1 on success, 0 on not enough space, -1 on no VMU found, -2 on unknown error
int save_game(void) {
    if (current_scene != LOADING) start_timer(&save_popup_timer, 2.0f);
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        snprintf(saved_text, sizeof(saved_text), "No VMU found!");
        return -1;
    }

    snprintf(saved_text, sizeof(saved_text), "Saving...");

    size_t save_size = sizeof(save);
    size_t header_size = sizeof(save.vms_menu_description) +
                         sizeof(save.dc_description) +
                         sizeof(save.app_identifier) +
                         sizeof(save.number_icons) +
                         sizeof(save.icon_animation_speed) +
                         sizeof(save.graphic_eyecatch_type) +
                         sizeof(save.crc) +
                         sizeof(save.bytes_after_header) +
                         sizeof(save.header_reserved) +
                         sizeof(save.icon_palette) +
                         sizeof(save.icon_bitmaps);

    snprintf(save.dc_description, sizeof(save.dc_description), "BeachBox - PsyOps");
    snprintf(save.app_identifier, sizeof(save.app_identifier), "Psyops");
    save.number_icons = 3;
    save.icon_animation_speed = 14;
    save.graphic_eyecatch_type = 0;
    save.crc = calc_CRC((unsigned char *)&save, save_size);
    save.bytes_after_header = save_size - header_size;
    memcpy(save.header_reserved, 00000000000000000000, 20);
    memcpy(save.icon_palette, bios_save_palette, sizeof(bios_save_palette));
    memcpy(save.icon_bitmaps, bios_save_animation, sizeof(bios_save_animation));

    int free_blocks = vmufs_free_blocks(vmu);

    if (free_blocks * 512 < save_size) {
        snprintf(saved_text, sizeof(saved_text), "Not enough space!");
        return 0;
    }

    if (0 == vmufs_write(vmu, "BeachBox", &save, save_size, VMUFS_OVERWRITE)) {
        snprintf(saved_text, sizeof(saved_text), "Game Saved!");
        return 1;
    }

    snprintf(saved_text, sizeof(saved_text), "Failed to save!");
    return -2;  // unknown error
}

// Returns 1 on success, 0 on no savefile found, -1 on no VMU found
int load_game(void) {
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        return -1;
    }
    void *save_buffer = malloc(sizeof(save));

    if (vmufs_read(vmu, "BeachBox", &save_buffer, NULL) == 0) {  // If loading was successful
        memcpy(&save, save_buffer, sizeof(save));
        free(save_buffer);
        return 1;
    }
    free(save_buffer);
    return -1;
}

// This function also saves the game
// Returns 1 on success, 0 on not enough space, -1 on no VMU found
int new_game(void) {
    memset(&save, 0, sizeof(save));
    save.hats_unlocked[HAT_NIL] = true;
    return save_game();
}