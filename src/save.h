#ifndef BBOX_SAVE_H
#define BBOX_SAVE_H

#include <stdint.h>
#include "hats.h"

typedef struct PlayerUpgradeLevels {
        bool    teleport_unlocked;
        bool    slowdown_unlocked;
        uint8_t player_speed_level;
        uint8_t meter_level;
        uint8_t teleport_cooldown_level;
        uint8_t teleport_distance_level;
        uint8_t slowdown_cost_level;
} player_upgrade_levels_t;

typedef struct Save {
        // HEADER //
        // Text fields are padded with space ($20). String fileds are padded with NUL ($00).
        struct {
                char    vms_menu_description[16]; // Description of file (shown in VMS file menu)
                char    dc_description[32];       // Description of file (shown in DC boot ROM file manager)
                char    app_identifier[16];       // Identifier of application that created the file (STRING)
                int16_t number_icons;             // Number of icons (>1 for animated icons)
                int16_t icon_animation_speed;     // Icon animation speed

                // Graphic eyecatch type
                // 0 = none
                // 1 = 16-bit true colour | 8064 bytes total
                // 2 = 256 colour palette | 4544 bytes total | (same format as icon palette, but 256 entries. the bitmap is 1 byte per pixel instead)
                // 3 = 16 colour palette  | 2048 bytes total | same as icon palette
                int16_t graphic_eyecatch_type;

                int16_t crc;                   // CRC (for error checking)
                int32_t bytes_after_header;    // Number of bytes of actual file data following header, icon(s) and eyecatch.
                char    header_reserved[20];   // Reserved (fill with zeros)
                int16_t icon_palette[16];      // Icon palette (16 16-bit integers)
                uint8_t icon_bitmaps[512 * 3]; // Icon bitmaps (Nybbles) (512 bytes per frame)

                // We use the type 3 eyecatch
                int16_t eyecatch_palette[16];  // Graphic eyecatch palette (32 8-bit integers)
                uint8_t eyecatch_bitmap[2016]; // Graphic eyecatch bitmap (Nybbles) (2016 bytes, 72×56 image)
        } header;

        // GAME SAVE DATA //
        player_upgrade_levels_t player_upgrade_levels;
        uint16_t                total_coins;
        uint16_t                total_runs;
        uint16_t                high_score;
        uint8_t                 color_index; // Selected color from player_colors array
        uint8_t                 hat_index;   // Selected hat from hats array
        bool                    hats_unlocked[HAT_MAX];
        char                    reserved_data[359]; // Reserved for (potential) future use

} __attribute__((__packed__)) save_t;

// This is only drawn if needed
void draw_save_popup(void);

// Returns 1 on success, 0 on not enough space, -1 on no VMU found, -2 on unknown error
int save_game(void);

// Returns 1 on success, 0 on no savefile found, -1 on no VMU found
int load_game(void);

// This creates a new savefile
void new_game(void);

// Returns true if a save is in progress
bool save_in_progress(void);

// Returns true if a load is in progress
bool load_in_progress(void);

#endif