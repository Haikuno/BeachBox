
struct PlayerPowerUps {
    bool teleport_unlocked;
    bool slowdown_unlocked;
    uint8_t player_speed_level;
    uint8_t meter_level;
    uint8_t teleport_cooldown_level;
    uint8_t teleport_distance_level;
    uint8_t slowdown_cost_level;
};

struct Save {
    struct PlayerPowerUps player_powerups;
    uint16_t total_coins;
    uint16_t total_runs;
    uint16_t high_score;
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

// Returns 1 on success, -1 on no VMU found
int load_game() {
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        return -1;
    }
    void *save_buffer = malloc(sizeof(save));

    if (vmufs_read(vmu, "BeachBox", &save_buffer, NULL) == 0) { // If loading was successful
        memcpy(&save, save_buffer, sizeof(save));
        free(save_buffer);
        return 1;
    }
    free(save_buffer);
    return 0;
}

// Returns 1 on success, 0 on not enough space, -1 on no VMU found
int new_game() {
    save.high_score = 0;
    save.total_coins = 110; // TODO: make this 0, 110 for now for testing purposes
    save.total_runs = 0;
    save.player_powerups.teleport_unlocked = false;
    save.player_powerups.slowdown_unlocked = false;
    save.player_powerups.player_speed_level = 0;
    save.player_powerups.meter_level = 0;
    save.player_powerups.teleport_cooldown_level = 0;
    save.player_powerups.teleport_distance_level = 0;
    save.player_powerups.slowdown_cost_level = 0;

    return save_game();
}