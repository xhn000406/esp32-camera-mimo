#include "burn_in_schedule.h"

#define CATCAM_BURN_IN_CHECKPOINT_INTERVAL_US (5ULL * 60ULL * 1000ULL * 1000ULL)

void catcam_burn_in_schedule_init(catcam_burn_in_schedule_t *schedule, uint64_t started_at_us)
{
    schedule->last_checkpoint_at_us = started_at_us;
}

bool catcam_burn_in_schedule_checkpoint_due(catcam_burn_in_schedule_t *schedule,
                                            uint64_t now_us)
{
    if (now_us - schedule->last_checkpoint_at_us < CATCAM_BURN_IN_CHECKPOINT_INTERVAL_US) {
        return false;
    }

    schedule->last_checkpoint_at_us = now_us;
    return true;
}
