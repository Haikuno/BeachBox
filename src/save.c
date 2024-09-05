const uint16_t vmu_blue = 0b1111001100111111;
const uint16_t vmu_black = 0b1111000000000000;
const uint16_t vmu_red = 0b1111111100000000;
const uint16_t vmu_yellow = 0b1111110111010000;
const uint16_t vmu_dark_yellow = 0b1111011101110000;

const int16_t icon_palette[16] = {
    vmu_blue,
    vmu_black,
    vmu_red,
    vmu_yellow,
    vmu_dark_yellow,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

const uint8_t vmu_icon[] = {
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
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x43, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x43,
    0x33, 0x33, 0x31, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x13, 0x33, 0x33,
    0x33, 0x33, 0x31, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x13, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x43, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x34, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};

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
    char reserved[20];              // Reserved (fill with zeros)
    int16_t icon_palette[16];       // Icon palette (16 16-bit integers)
    uint8_t icon_bitmaps[512];      // Icon bitmaps (Nybbles)
    // Text fields are padded with space ($20). String fileds are padded with NUL ($00). Integer fields are little endian.

    // SAVE DATA //
    struct PlayerUpgradeLevels player_upgrade_levels;
    uint16_t total_coins;
    uint16_t total_runs;
    uint16_t high_score;
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

// Returns 1 on success, 0 on not enough space, -1 on no VMU found
int save_game() {
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        return -1;
    }
    int save_size = sizeof(save);

    // strcpy(save.vms_menu_description, "BeachBox");
    strcpy(save.dc_description, "BeachBox - PsyOps");
    strcpy(save.app_identifier, "Psyops");
    save.number_icons = 1;
    save.icon_animation_speed = 0;
    save.graphic_eyecatch_type = 0;
    save.crc = calc_CRC((unsigned char *)&save, save_size);
    save.bytes_after_header = save_size - 642;  // 642 is the amount of bytes in the header
    memcpy(save.reserved, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20);
    memcpy(save.icon_palette, icon_palette, sizeof(icon_palette));
    memcpy(save.icon_bitmaps, vmu_icon, sizeof(vmu_icon));

    int free_blocks = vmufs_free_blocks(vmu);

    if (free_blocks * 512 < save_size) {
        return 0;
    }

    return 0 == vmufs_write(vmu, "BeachBox", &save, save_size, VMUFS_OVERWRITE);
}

// Returns 1 on success, 0 on no savefile found, -1 on no VMU found
int load_game() {
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
    return 0;
}

// This function also saves the game
// Returns 1 on success, 0 on not enough space, -1 on no VMU found
int new_game() {
    save.high_score = 0;
    save.total_coins = 0;
    save.total_runs = 0;
    save.player_upgrade_levels.teleport_unlocked = false;
    save.player_upgrade_levels.slowdown_unlocked = false;
    save.player_upgrade_levels.player_speed_level = 0;
    save.player_upgrade_levels.meter_level = 0;
    save.player_upgrade_levels.teleport_cooldown_level = 0;
    save.player_upgrade_levels.teleport_distance_level = 0;
    save.player_upgrade_levels.slowdown_cost_level = 0;

    return save_game();
}