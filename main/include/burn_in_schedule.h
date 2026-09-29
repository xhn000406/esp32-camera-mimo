#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint64_t last_checkpoint_at_us;
} catcam_burn_in_schedule_t;

void catcam_burn_in_schedule_init(catcam_burn_in_schedule_t *schedule, uint64_t started_at_us);
bool catcam_burn_in_schedule_checkpoint_due(catcam_burn_in_schedule_t *schedule,
                                            uint64_t now_us);
