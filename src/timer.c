struct Timer {
    float time_started;
    float duration;
    bool is_done;
};

inline void start_timer(struct Timer *timer, float duration) {
    timer->time_started = GetTime();
    timer->duration = duration;
    timer->is_done = false;
}

inline void update_timer(struct Timer *timer) {
    timer->is_done = GetTime() - timer->time_started >= timer->duration;
}