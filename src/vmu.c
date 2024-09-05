static const char vmu_wave_frame_1[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00110011,
    0b11001100,
    0b00000000,
};

static const char vmu_wave_frame_2[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11001100,
    0b00110011,
    0b00000000,
};

static const char vmu_box_bottom_left[] = {
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b11111111,
};

static const char vmu_box_bottom_right[] = {
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b11111111,
};

static const char vmu_box_top_left[] = {
    0b11111111,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b10000000,
};
static const char vmu_box_top_right[] = {
    0b11111111,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001,
};

static const char vmu_sand_1[] = {
    0b00000000,
    0b00000010,
    0b00000000,
    0b00000000,
    0b00000000,
    0b10000000,
    0b00000000,
    0b00000100,
};

static const char vmu_sand_2[] = {
    0b00000000,
    0b00100000,
    0b00000000,
    0b00000001,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000000,
};

int vmu_sand_pos_x = 0;
int vmu_wave_pos_y = 13;

bool up_or_down = 0;

void vmu_wave_calc() {
    vmu_wave_pos_y += up_or_down ? 1 : -1;
    if (vmu_wave_pos_y <= 4 || vmu_wave_pos_y >= 14) {
        up_or_down = !up_or_down;
    }
}

void vmu_sand_calc() {
    vmu_sand_pos_x--;
    if (vmu_sand_pos_x == -8) {
        vmu_sand_pos_x = 0;
    }
}

vmufb_t vmu_fb;

static float vmuUpdate = 0;

void draw_vmu_animation() {
    float now = GetTime();
    if (now - vmuUpdate < 0.20f) return;
    vmuUpdate = now;

    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    if (!vmu) {
        return;
    }

    vmu_wave_calc();
    vmu_sand_calc();

    // Sand
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x, 16, 8, 8, vmu_sand_2);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x, 24, 8, 8, vmu_sand_1);

    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 8, 16, 8, 8, vmu_sand_2);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 16, 16, 8, 8, vmu_sand_2);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 24, 16, 8, 8, vmu_sand_2);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 32, 16, 8, 8, vmu_sand_2);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 40, 16, 8, 8, vmu_sand_2);

    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 8, 24, 8, 8, vmu_sand_1);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 16, 24, 8, 8, vmu_sand_1);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 24, 24, 8, 8, vmu_sand_1);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 32, 24, 8, 8, vmu_sand_1);
    vmufb_paint_area(&vmu_fb, vmu_sand_pos_x + 40, 24, 8, 8, vmu_sand_1);

    // Wave
    vmufb_paint_area(&vmu_fb, 0, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);
    vmufb_paint_area(&vmu_fb, 8, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);
    vmufb_paint_area(&vmu_fb, 16, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);
    vmufb_paint_area(&vmu_fb, 24, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);
    vmufb_paint_area(&vmu_fb, 32, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);
    vmufb_paint_area(&vmu_fb, 40, vmu_wave_pos_y, 8, 8, vmu_wave_frame_1);

    vmufb_paint_area(&vmu_fb, 28, vmu_wave_pos_y - 8, 8, 8, vmu_wave_frame_2);
    vmufb_paint_area(&vmu_fb, 36, vmu_wave_pos_y - 8, 8, 8, vmu_wave_frame_2);

    vmufb_paint_area(&vmu_fb, 4, vmu_wave_pos_y - 8, 8, 8, vmu_wave_frame_2);
    vmufb_paint_area(&vmu_fb, 12, vmu_wave_pos_y - 8, 8, 8, vmu_wave_frame_2);

    // Player
    vmufb_paint_area(&vmu_fb, 16, 10, 8, 8, vmu_box_top_left);
    vmufb_paint_area(&vmu_fb, 24, 10, 8, 8, vmu_box_top_right);
    vmufb_paint_area(&vmu_fb, 16, 18, 8, 8, vmu_box_bottom_left);
    vmufb_paint_area(&vmu_fb, 24, 18, 8, 8, vmu_box_bottom_right);

    vmufb_present(&vmu_fb, vmu);
}
