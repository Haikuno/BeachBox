#ifndef BBOX_TIMER_H
#define BBOX_TIMER_H

#include <raylib.h>

typedef struct Timer {
    float duration;
    float progress;
    bool  is_running;
} bbox_timer_t;

inline void start_timer(bbox_timer_t *timer, float duration) {
    timer->duration   = duration;
    timer->progress   = 0.0f;
    timer->is_running = true;
}

inline void update_timer(bbox_timer_t *timer) {
    if (!timer->is_running) return;

    timer->progress   += GetFrameTime();
    timer->is_running  = timer->progress < timer->duration;
}

#endif