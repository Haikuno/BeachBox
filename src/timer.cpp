struct Timer {
    float duration;
    float progress;
    bool is_done;
};

inline void start_timer(struct Timer *timer, float duration) {
    timer->duration = duration;
    timer->progress = 0.0f;
    timer->is_done = false;
}

inline void update_timer(struct Timer *timer) {
    if (timer->is_done) return;
    timer->progress += GetFrameTime();
    timer->is_done = timer->progress >= timer->duration;
}