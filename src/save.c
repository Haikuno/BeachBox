// TODO: add save description and icon

struct PlayerUpgradeLevels {
    bool teleport_unlocked;
    bool slowdown_unlocked;
    uint8_t player_speed_level;
    uint8_t meter_level;
    uint8_t teleport_cooldown_level;
    uint8_t teleport_distance_level;
    uint8_t slowdown_cost_level;
};

struct Save {
    struct PlayerUpgradeLevels player_upgrade_levels;
    uint16_t total_coins;
    uint16_t total_runs;
    uint16_t high_score;
    char reserved[100];  // reserve 100 bytes for future expansion just in case
} save;

// Returns 1 on success, 0 on not enough space, -1 on no VMU found
int save_game() {
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        return -1;
    }

    int free_blocks = vmufs_free_blocks(vmu);
    int save_size = sizeof(save);

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